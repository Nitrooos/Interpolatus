#include "DataManager.hpp"
#include "Glade.hpp"
#include "Exceptions.hpp"

#include <gtkmm.h>
#include <iostream>
#include <sstream>                      // wczytywanie liczby do łańcucha
#include <iomanip>                      // funkcja setprecision()
#include <fstream>                      // ładowanie danych z pliku
#include <dlfcn.h>                      // ładowanie biblioteki .so

DataManager::DataManager(RefPtr<Builder> const& builder, TreeView *tv)
    : treeView(tv)
{
    char *error;

    libHandle = dlopen("/home/bartosz/Documents/EANproject/debug/libiln.so.1.0", RTLD_LAZY);
    if (!libHandle) {
        // Rzucenie wyjątku dlerror()
        cout << "Nie mogę załadować biblioteki .so!\n";
    }

    lagrange = (decltype(lagrange))dlsym(libHandle, "lagrange");
    if ((error = dlerror()) != NULL) {
        // Rzucenie innym wyjątkiem
        cout << "Nie mogę załadować funkcji lagrange!\n";
    }

    neville = (decltype(neville))dlsym(libHandle, "neville");
    if ((error = dlerror()) != NULL) {
        // Rzucenie wyjątkiem
        cout << "Nie mogę załadować funkcji neville!\n";
    }

    // Pobierz bazy danych
    floatBase = RefPtr<ListStore>::cast_static(builder->get_object("floatBase"));
    if (!builder)
        throw LoadWidgetError("floatBase");

    halfIntervalBase = RefPtr<ListStore>::cast_static(builder->get_object("halfIntervalBase"));
    if (!builder)
        throw LoadWidgetError("halfIntervalBase");

    fullIntervalBase = RefPtr<ListStore>::cast_static(builder->get_object("fullIntervalBase"));
    if (!builder)
        throw LoadWidgetError("fullIntervalBase");
}

DataManager::~DataManager() {
    dlclose(libHandle);                // dynamiczny unloading biblioteki
}

void DataManager::setArthmetic(Arthmetic mode) {
    this->arthm = mode;
    switch (mode) {
        case Arthmetic::FLOAT_POINT:
            this->treeView->set_model(floatBase);
            break;
        case Arthmetic::HALF_INTERV:
            this->treeView->set_model(halfIntervalBase);
            break;
        case Arthmetic::FULL_INTERV:
            this->treeView->set_model(fullIntervalBase);
            break;
    }
}

void DataManager::setAlgorithm(Algorithm algorithm) {
    this->algorithm = algorithm;
}

void DataManager::setInterpolPoint(long double point) {
    this->interpolPoint = point;
}

void DataManager::loadFile(string name) {
    removeAllRecords();

    int flNodes, hiNodes, fiNodes;
    long double node, value;
    interval nodeHI, valueHI, nodeFI, valueFI;

    ifstream F(name);
    F >> flNodes >> hiNodes >> fiNodes;

    for (int i = 0; i < flNodes; ++i) {
        F >> node >> value;
        if (F)
            dataFloatPoint[node] = value;
        else throw NodeFileError();
    }

    for (int i = 0; i < hiNodes; ++i) {
        F >> nodeHI.a >> nodeHI.b >> valueHI.a >> valueHI.b;
        if (F)
            dataHalfInterval[nodeHI] = valueHI;
        else throw NodeFileError();
    }

    for (int i = 0; i < fiNodes; ++i) {
        F >> nodeFI.a >> nodeFI.b >> valueFI.a >> valueFI.b;
        if (F)
            dataFullInterval[nodeFI] = valueFI;
        else throw NodeFileError();
    }

    F.close();
}

void DataManager::saveFile(string name) {
    ofstream G(name);
    G << dataFloatPoint.size() << "\t" << dataHalfInterval.size() << "\t" << dataFullInterval.size() << "\n\n";

    for (auto &d : dataFloatPoint)
        G << d.first << "\t" << d.second << "\n";
    G << "\n";
    for (auto &d : dataHalfInterval)
        G << d.first.a << "\t" << d.first.b << "\t" << d.second.a << "\t" << d.second.b << "\n";
    G << "\n";
    for (auto &d : dataFullInterval)
        G << d.first.a << "\t" << d.first.b << "\t" << d.second.a << "\t" << d.second.b << "\n";
    G << "\n";

    G.close();
}

void DataManager::changeRecord(ustring const& path, ustring const& text, ColumnEdit col) {
    // Próba konwersji wpisanego tekstu na liczbę
    long double newText = stold(text.raw());

    // Czy węzeł jest unikalny?
    if (col == ColumnEdit::NODE && !isNodeUnique(newText))
        throw DuplicateNode();          // jednak nie...

    // Wszystko w porządku - zmień wartość w wierszu
    switch (arthm) {
        // Arytmetyka zmiennopozycyjna
        case Arthmetic::FLOAT_POINT: {
            // Uzyskanie iteratora ze ścieżki path
            TreeModel::iterator iter = floatBase->get_iter(TreePath(path));
            TreeModel::Row row = *iter;
            long double node = stold(row[mColFloat.node]),
                        value = stold(row[mColFloat.value]);

            switch (col) {
                // Edytowany węzeł
                case ColumnEdit::NODE:
                    dataFloatPoint[newText] = value;
                    dataFloatPoint.erase(node);
                    row[mColFloat.node] = text;
                    break;
                // Edytowana wartość w węźle
                case ColumnEdit::VALUE:
                    dataFloatPoint[node] = newText;
                    row[mColFloat.value] = text;
                    break;
            }
        }
        case Arthmetic::HALF_INTERV: {
            break;
        }
        case Arthmetic::FULL_INTERV: {
            break;
        }
    }

}

void DataManager::addRecords(Info::AddNodes const& info) {
    switch (arthm) {
        case Arthmetic::FLOAT_POINT: {
            for (int i = 0; i < info.nNodes; ++i) {
                long double node = info.startValue + i*info.step;
                if (!isNodeUnique(node))
                    continue;

                dataFloatPoint[node] = 0.0;

                auto row = *(floatBase->append());
                row[mColFloat.node]  = to_string(node);
                row[mColFloat.value] = to_string(0.0);
            }
            break;
        }
        case Arthmetic::HALF_INTERV: {
            break;
        }
        case Arthmetic::FULL_INTERV: {
            for (int i = 0; i < info.nNodes; ++i) {
                //if (!isNodeUnique(dr.node))
                    //continue;

                interval node{info.startValue + i*info.step, info.intervalWidth};
                dataFullInterval[node] = interval(0.0, 0.1);

                auto row = *(fullIntervalBase->append());
                row[mColInterval.nodeLeftEnd]   = to_string(node.a);
                row[mColInterval.nodeRightEnd]  = to_string(node.a);
                row[mColInterval.valueLeftEnd]  = to_string(0.0);
                row[mColInterval.valueRightEnd] = to_string(0.1);
            }
            break;
        }
    }
}

void DataManager::removeRecords(vector<TreePath> const& sel) {
    vector<TreeIter> iters;
    switch (arthm) {
        case Arthmetic::FLOAT_POINT:
            for (auto &x : sel)
                iters.push_back(floatBase->get_iter(x));

            for (auto &it : iters) {
                dataFloatPoint.erase(getNodeF((*it)[mColFloat.node])->first);
                floatBase->erase(it);
            }
            break;
        case Arthmetic::HALF_INTERV:
            //~ for (auto &x : sel)
                //~ iters.push_back(halfIntervalBase->get_iter(x));
//~
            //~ for (auto &it : iters) {
                //~ dataHalfInterval.erase(getNode((*it)[mColFloat.node])->first);
                //~ halfIntervalBase->erase(it);
            //~ }
            break;
        case Arthmetic::FULL_INTERV:
            for (auto &x : sel)
                iters.push_back(fullIntervalBase->get_iter(x));

            for (auto &it : iters) {
                dataFullInterval.erase(getNodeFI((*it)[mColInterval.nodeLeftEnd],
                                                 (*it)[mColInterval.nodeRightEnd])->first);
                fullIntervalBase->erase(it);
            }
            break;
    }
}

void DataManager::removeAllRecords() {
    dataFloatPoint.clear();
    dataHalfInterval.clear();
    dataFullInterval.clear();
    floatBase->clear();
    halfIntervalBase->clear();
    fullIntervalBase->clear();
}

string DataManager::getRecordInfo(TreePath const& path) {
    ostringstream S;

    switch (arthm) {
        case Arthmetic::FLOAT_POINT: {
            auto row = (*floatBase->get_iter(path));
            auto iter = getNodeF(row[mColFloat.node]);
            S << showpos << fixed << "x = " << setprecision(25) << iter->first << "\t\tf(x) = " << iter->second;
            break;
        }
        case Arthmetic::HALF_INTERV:
            S << "Not implemented yet";
            break;
        case Arthmetic::FULL_INTERV:
            auto row = (*fullIntervalBase->get_iter(path));
            auto iter = getNodeFI(row[mColInterval.nodeLeftEnd], row[mColInterval.nodeRightEnd]);
            S << showpos << fixed << "x = [" << setprecision(25) << iter->first.a << "; " << iter->first.b << "]\n" <<
                                     "f(x) = [" << iter->second.a << "; " << iter->second.b << "]";
            break;
    }
    return S.str();
}

Arthmetic DataManager::whatArthmetic() const {
    return arthm;
}

ustring DataManager::getResult() const {
    ostringstream S;
    switch (arthm) {
        case Arthmetic::FLOAT_POINT:
            S << showpos << setprecision(25) << resultFloat;
            break;
    }
    return S.str();
}

void DataManager::interpolation() {
    switch (this->arthm) {
        case Arthmetic::FLOAT_POINT: {
            int status;
            long double r = 0.0;

            switch (this->algorithm) {
                case Algorithm::LAGRANGE:
                    r = lagrange(dataFloatPoint, interpolPoint, status);
                    break;
                case Algorithm::NEVILLE:
                    r = neville (dataFloatPoint, interpolPoint, status);
                    break;
            }
            if (status == 0)
                resultFloat = r;
            else {
                // RZUCENIE WYJĄTKIEM
            }
            break;
        }
        case Arthmetic::HALF_INTERV: {
            break;
        }
        case Arthmetic::FULL_INTERV: {
            break;
        }
    }
}

bool DataManager::isNodeUnique(long double node) const {
    if (dataFloatPoint.find(node) == dataFloatPoint.end())
        return true;
    return false;
}

map<long double, long double>::iterator DataManager::getNodeF(string number) {
    return dataFloatPoint.find(stold(number));
}

//map<interval, interval>::iterator const& DataManager::getNodeH(string number) const {
//    return dataHalfInterval.find(stold(number));
//}

map<interval, interval>::iterator DataManager::getNodeFI(string leftEnd, string rightEnd) {
    return dataFullInterval.find(interval(stold(leftEnd), stold(rightEnd)));
}
