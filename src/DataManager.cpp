#include "DataManager.hpp"
#include "Exceptions.hpp"
#include "../shared/Algorithms.hpp"

#include <sstream>                      // wczytywanie liczby do łańcucha
#include <iomanip>                      // funkcja setprecision()
#include <fstream>                      // ładowanie danych z pliku
#include <dlfcn.h>                      // ładowanie biblioteki .so

DataManager::DataManager(RefPtr<Builder> const& builder) {
    char *error;

    /*libHandle = dlopen("../debug/libiln.so.1.0", RTLD_LAZY);
    if (!libHandle)
        throw LoadSharedLibError("../debug/libiln.so.1.0", dlerror());

    lagrange = (decltype(lagrange))dlsym(libHandle, "lagrange");
    if ((error = dlerror()) != NULL)
        throw LoadSharedFunError("../debug/libiln.so.1.0", "lagrange", error);

    neville = (decltype(neville))dlsym(libHandle, "neville");
    if ((error = dlerror()) != NULL)
        throw LoadSharedFunError("../debug/libiln.so.1.0", "neville", error);

    calcFactors = (decltype(calcFactors))dlsym(libHandle, "factors");
    if ((error = dlerror()) != NULL)
        throw LoadSharedFunError("../debug/libiln.so.1.0", "factors", error);

    lagrangeI = (decltype(lagrangeI))dlsym(libHandle, "lagrangeI");
    if ((error = dlerror()) != NULL)
        throw LoadSharedFunError("../debug/libiln.so.1.0", "lagrangeI", error);

    nevilleI = (decltype(nevilleI))dlsym(libHandle, "nevilleI");
    if ((error = dlerror()) != NULL)
        throw LoadSharedFunError("../debug/libiln.so.1.0", "nevilleI", error);*/

    // Pobierz bazę danych
    dataBase = RefPtr<ListStore>::cast_static(builder->get_object("dataBase"));
    if (!builder)
        throw LoadWidgetError("dataBase");
}

DataManager::~DataManager() {
    dlclose(libHandle);                // dynamiczny unloading biblioteki
}

void DataManager::setArthmetic(Arthmetic mode) {
    this->arthm = mode;
}

void DataManager::setAlgorithm(Algorithm algorithm) {
    this->algorithm = algorithm;
}

void DataManager::setInterpolPoint(long double point) {
    this->interpolPoint = point;
}

void DataManager::loadFile(string name) {
    removeAllRecords();

    int id, i = 0;
    ifstream F(name);
    for (;;) {
        Info::DataRecord dr;
        F >> id >> dr.node >> dr.value >> dr.nodeI.a >> dr.nodeI.b >> dr.valueI.a >> dr.valueI.b;
        if (F) {
            data[id] = dr;
            addRecordToBase(id, dr);
        } else {
            if (i == 0)
                throw LoadIDFError(name);
            break;
        }
        ++i;
    }
    F.close();
}

void DataManager::saveFile(string name) {
    ofstream G(name);
    for (auto &dr : data) {
        G << dr.first << "\t" << dr.second.node     << "\t" << dr.second.value <<
                         "\t" << dr.second.nodeI.a  << "\t" << dr.second.nodeI.b <<
                         "\t" << dr.second.valueI.a << "\t" << dr.second.valueI.b << "\n";
    }
    G.close();
}

void DataManager::changeRecord(ustring const& path, ustring const& text, ColumnEdit col) {
    // Próba konwersji wpisanego tekstu na liczbę
    long double value = stold(text.raw());

    // Czy węzeł jest unikalny?
    if (col == ColumnEdit::NODE && !isNodeUnique(value))
        throw DuplicateNode(text.raw());          // jednak nie...

    // Uzyskanie iteratora ze ścieżki path
    TreeModel::iterator iter = dataBase->get_iter(TreePath(path));
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Wszystko w porządku - zmień wartość w wierszu
        switch (col) {
            case ColumnEdit::NODE:
                data[row[modelColumns.id]].node = value;
                row[modelColumns.node] = text;
                break;
            case ColumnEdit::VALUE:
                data[row[modelColumns.id]].value = value;
                row[modelColumns.value] = text;
                break;
        }
    }
}

void DataManager::addRecords(Info::AddNodes const& info) {
    int newKey = 1;
    if (data.empty() == false)
        newKey = data.rbegin()->first + 1;

    switch (arthm) {
        case Arthmetic::FLOAT_POINT: {
            for (int i = 0, added = 0; i < info.nNodes; ++i) {
                Info::DataRecord dr;
                dr.node  = info.startValue + i*info.step;
                dr.value = 0.0;
                if (!isNodeUnique(dr.node))
                    continue;

                int id = newKey + added;
                addRecordToBase(id, dr);
                data[id] = dr;
                ++added;
            }
            break;
        }
        case Arthmetic::HALF_INTERV: {
            break;
        }
        case Arthmetic::FULL_INTERV: {
            for (int i = 0, added = 0; i < info.nNodes; ++i) {
                Info::DataRecord dr;
                dr.nodeI.a  = info.startValue + i*info.step;
                dr.nodeI.b  = dr.nodeI.a + info.intervalWidth;
                dr.valueI.a = 0.0;
                dr.valueI.b = 0.1;

                //if (!isNodeUnique(dr.node))
                    //continue;

                int id = newKey + added;
                addRecordToBase(id, dr);
                data[id] = dr;
                ++added;
            }
            break;
        }
    }
}

void DataManager::removeRecords(vector<TreePath> const& sel) {
    vector<TreeIter> iters;
    for (auto &x : sel)
        iters.push_back(dataBase->get_iter(x));

    for (auto &it : iters) {
        data.erase((*it)[modelColumns.id]);
        dataBase->erase(it);
    }
}

void DataManager::removeAllRecords() {
    data.clear();
    dataBase->clear();
}

string DataManager::getRecordInfo(TreePath const& path) {
    ostringstream S;
    int id = (*dataBase->get_iter(path))[modelColumns.id];

    switch (arthm) {
        case Arthmetic::FLOAT_POINT: {
            S << showpos << "x = " << fixed << setprecision(25) << data[id].node << "\t\tf(x) = " << data[id].value;
            break;
        }
        case Arthmetic::HALF_INTERV:
            S << "Not implemented yet";
            break;
        case Arthmetic::FULL_INTERV:
            S << showpos << "x = [" << fixed << setprecision(25) << data[id].nodeI.a << "; " << data[id].nodeI.b << "]\n" <<
                            "f(x) = [" << data[id].valueI.a << "; " << data[id].valueI.b << "]";
            break;
    }
    return S.str();
}

Arthmetic DataManager::whatArthmetic() const {
    return arthm;
}

void DataManager::interpolation() {
    int status;
    switch (this->arthm) {
        case Arthmetic::FLOAT_POINT: {
            long double r = 0.0;

            switch (this->algorithm) {
                case Algorithm::LAGRANGE:
                    r = lagrange(data, interpolPoint, status);
                    break;
                case Algorithm::NEVILLE:
                    r = neville (data, interpolPoint, status);
                    break;
            }
            if (status == 1)
                throw EmptyData();
            else if (status == 2)
                throw DuplicateNode("<brak danych>");

            factors = calcFactors(data, status);
            result.value = r;
            break;
        }
        case Arthmetic::HALF_INTERV: {
            break;
        }
        case Arthmetic::FULL_INTERV: {
            interval r;

            switch (this->algorithm) {
                case Algorithm::LAGRANGE:
                    r = lagrangeI(data, interval(interpolPoint), status);
                    break;
                case Algorithm::NEVILLE:
                    r = nevilleI (data, interval(interpolPoint), status);
                    break;
            }
            if (status == 1)
                throw EmptyData();
            else if (status == 2)
                throw DuplicateNode("<brak danych>");

            // wspołczynniki wielomianu
            result.valueI = r;
            break;
        }
    }
}

ustring DataManager::getResult() const {
    ostringstream S;
    switch (this->arthm) {
        case Arthmetic::FLOAT_POINT:
            S << showpos << setprecision(25) << result.value;
            break;
        case Arthmetic::HALF_INTERV:
            break;
        case Arthmetic::FULL_INTERV:
            S << result.valueI.write();
            break;
    }
    return S.str();
}

ustring DataManager::getFactors() const {
    ostringstream S;
    switch (this->arthm) {
        case Arthmetic::FLOAT_POINT:
            for (int i = 0, n = factors.size(); i < n; ++i)
                if (factors[i] != 0)
                    S << (factors[i] > 0 && i != 0 ? "+ " : "- ")
                      <<  abs(factors[i]) << "x" << "<sup>" << n-i-1 << "</sup> "
                      << ((i+1)%5 ? "" : "\n");
            break;
    }
    return S.str();
}

bool DataManager::isNodeUnique(long double node) const {
    for (auto &x : data)
        if (x.second.node == node)
            return false;
    return true;
}

void DataManager::addRecordToBase(int id, Info::DataRecord const& dr) {
    auto row = *(dataBase->append());
    row[modelColumns.id] = id;
    row[modelColumns.node]  = to_string(dr.node);
    row[modelColumns.value] = to_string(dr.value);
    row[modelColumns.nodeLeftEnd]   = to_string(dr.nodeI.a);
    row[modelColumns.nodeRightEnd]  = to_string(dr.nodeI.b);
    row[modelColumns.valueLeftEnd]  = to_string(dr.valueI.a);
    row[modelColumns.valueRightEnd] = to_string(dr.valueI.b);
}
