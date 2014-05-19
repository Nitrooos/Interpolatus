#include "DataManager.hpp"
#include "Exceptions.hpp"
#include "../shared/Algorithms.hpp"

#include <sstream>                      // wczytywanie liczby do łańcucha
#include <iomanip>                      // funkcja setprecision()
#include <fstream>                      // ładowanie danych z pliku
#include <dlfcn.h>                      // ładowanie biblioteki .so

DataManager::DataManager(RefPtr<Builder> const& builder) {
    char *error;

    // Pobierz bazę danych
    dataBase = RefPtr<ListStore>::cast_static(builder->get_object("dataBase"));
    if (!builder)
        throw LoadWidgetError("dataBase");
}

DataManager::~DataManager() { }

void DataManager::setArthmetic(Arthmetic mode) {
    this->arthm = mode;
    if (mode == Arthmetic::HALF_INTERV)                     // jeśli przełączamy się na a. przedziałową I
        updateIntervalsFromPoints();
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
    long double value;
    interval valueInterval;

    if (this->arthm == Arthmetic::HALF_INTERV) {
        // Konwersja do przedziału
        valueInterval = text.raw();
        // Czy podany przez użytkownika węzeł jest unikalny?
        if (col == ColumnEdit::NODE && !isNodeUnique(valueInterval))
            throw DuplicateNode(text.raw());
    } else {
        // Próba konwersji wpisanego tekstu na liczbę
        value = stold(text.raw());
        // Czy węzeł jest unikalny?
        if (col == ColumnEdit::NODE && !isNodeUnique(value))
            throw DuplicateNode(text.raw());                    // jednak nie...
    }

    // Uzyskanie iteratora ze ścieżki path
    TreeModel::iterator iter = dataBase->get_iter(TreePath(path));
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Wszystko w porządku - zmień wartość w wierszu
        switch (col) {
            case ColumnEdit::NODE:
                if (this->arthm == Arthmetic::FLOAT_POINT)  data[row[modelColumns.id]].node  = value;
                else                                        data[row[modelColumns.id]].nodeI = valueInterval;
                row[modelColumns.node] = text;
                break;
            case ColumnEdit::VALUE:
                if (this->arthm == Arthmetic::FLOAT_POINT)  data[row[modelColumns.id]].value  = value;
                else                                        data[row[modelColumns.id]].valueI = valueInterval;
                row[modelColumns.value] = text;
                break;
            case ColumnEdit::NODE_LEFT:
                data[row[modelColumns.id]].nodeI.a  = value;
                row[modelColumns.nodeLeftEnd]       = text;
                break;
            case ColumnEdit::NODE_RIGHT:
                data[row[modelColumns.id]].nodeI.b  = value;
                row[modelColumns.nodeRightEnd]      = text;
                break;
            case ColumnEdit::VALUE_LEFT:
                data[row[modelColumns.id]].valueI.a = value;
                row[modelColumns.valueLeftEnd]      = text;
                break;
            case ColumnEdit::VALUE_RIGHT:
                data[row[modelColumns.id]].valueI.b = value;
                row[modelColumns.valueRightEnd]     = text;
                break;
        }
    }
}

void DataManager::addRecords(Info::AddNodes const& info) {
    int newKey = 1;
    if (data.empty() == false)
        newKey = data.rbegin()->first + 1;

    switch (arthm) {
        case Arthmetic::FULL_INTERV: {
            for (int i = 0, added = 0; i < info.nNodes; ++i) {
                Info::DataRecord dr;
                dr.nodeI.a  = info.startValue + i*info.step;
                dr.nodeI.b  = dr.nodeI.a + info.intervalWidth;
                dr.valueI.a = 0.00L;
                dr.valueI.b = 0.01L;

                if (!isNodeUnique(dr.nodeI))
                    continue;

                int id = newKey + added;
                addRecordToBase(id, dr);
                data[id] = dr;
                ++added;
            }
            break;
        }
        default: {
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
            S << showpos << fixed << setprecision(25) << "x = " << data[id].node << "\t\tf(x) = " << data[id].value;
            break;
        }
        default:
            S << showpos << "x = " << data[id].nodeI.write() << "\nf(x) = " << data[id].valueI.write();
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
                    r = lagrange<long double>(data, interpolPoint, status);
                    break;
                case Algorithm::NEVILLE:
                    r = neville<long double> (data, interpolPoint, status);
                    break;
            }
            if      (status == 1)   throw EmptyData();
            else if (status == 2)   throw DuplicateNode("<brak danych>");

            factors = calcFactors<long double>(data, status);
            result.value = r;
            break;
        }
        default: {
            interval r;

            switch (this->algorithm) {
                case Algorithm::LAGRANGE:
                    r = lagrange<interval>(data, interval(interpolPoint), status);
                    break;
                case Algorithm::NEVILLE:
                    r = neville<interval> (data, interval(interpolPoint), status);
                    break;
            }
            if (status == 1)
                throw EmptyData();
            else if (status == 2)
                throw DuplicateNode("<brak danych>");

            factorsI = calcFactors<interval>(data, status);
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
        default:
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
                    S << (factors[i] > 0 ? "+ " : "- ")
                      <<  abs(factors[i]) << "x" << "<sup>" << n-i-1 << "</sup> "
                      << ((i+1)%5 ? "" : "\n");
            break;
        default:
            for (int i = 0, n = factorsI.size(); i < n; ++i)
                S << factorsI[i].write() << "x" << "<sup>" << n-i-1 << "</sup> "
                  << (i == n-1 ? "" : "+\n");
            break;
    }
    return S.str();
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

void DataManager::updateIntervalsFromPoints() {
    TreeModel::Children children = dataBase->children();
    for (auto &row : children) {
        int id = row[modelColumns.id];
        data[id].nodeI  = interval(data[id].node);
        data[id].valueI = interval(data[id].value);

        row[modelColumns.nodeLeftEnd]   = to_string(data[id].nodeI.a);
        row[modelColumns.nodeRightEnd]  = to_string(data[id].nodeI.b);
        row[modelColumns.valueLeftEnd]  = to_string(data[id].valueI.a);
        row[modelColumns.valueRightEnd] = to_string(data[id].valueI.b);
    }
}
