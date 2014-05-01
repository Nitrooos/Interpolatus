#include "DataManager.hpp"
#include "Glade.hpp"
#include "Exceptions.hpp"

#include <gtkmm.h>
#include <sstream>
#include <iomanip>
#include <fstream>

DataManager::DataRecord::DataRecord()
    : node(0.0), value(0.0), nodeI({0.0, 0.0}), valueI({0.0, 0.0}) { }

DataManager::DataManager(RefPtr<Builder> const& builder) {
    // Pobierz bazę danych
    dataBase = RefPtr<ListStore>::cast_static(builder->get_object("dataBase"));
    if (!builder)
        throw LoadWidgetError("dataBase");
}

void DataManager::loadFile(string name) {
    removeAllRecords();

    int id;
    ifstream F(name);
    for (;;) {
        DataRecord dr;
        F >> id >> dr.node >> dr.value >> dr.nodeI.a >> dr.nodeI.b >> dr.valueI.a >> dr.valueI.b;
        if (F) {
            data[id] = dr;
            addRecordToBase(id, dr);
        } else break;
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

void DataManager::changeArthmetic(Arthmetic mode) {
    this->arthm = mode;
}

void DataManager::changeRecord(ustring const& path, ustring const& text, ColumnEdit col) {
    // Próba konwersji wpisanego tekstu na liczbę
    long double value = stold(text.raw());

    // Czy węzeł jest unikalny?
    if (col == ColumnEdit::NODE && !isNodeUnique(value))
        throw DuplicateNode();          // jednak nie...

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
    int newKey = 0;
    if (data.empty() == false)
        newKey = data.rbegin()->first + 1;

    switch (arthm) {
        case Arthmetic::FLOAT_POINT: {
            for (int i = 0, added = 0; i < info.nNodes; ++i) {
                DataRecord dr;
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
                DataRecord dr;
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
        case Arthmetic::FLOAT_POINT:
            S << "x = " << fixed << setprecision(25) << data[id].node << "\t\tf(x) = " << data[id].value;
            break;
        case Arthmetic::HALF_INTERV:
            S << "Not implemented yet";
            break;
        case Arthmetic::FULL_INTERV:
            S << "x = [" << fixed << setprecision(25) << data[id].nodeI.a << "; " << data[id].nodeI.b << "]\t\t" <<
                 "f(x) = [" << data[id].valueI.a << "; " << data[id].valueI.b << "]";
            break;
    }
    return S.str();
}

Arthmetic DataManager::whatArthmetic() const {
    return arthm;
}

bool DataManager::isNodeUnique(long double node) const {
    for (auto &x : data)
        if (x.second.node == node)
            return false;
    return true;
}

void DataManager::addRecordToBase(int id, DataRecord const& dr) {
    auto row = *(dataBase->append());
    row[modelColumns.id] = id;
    row[modelColumns.node]  = to_string(dr.node);
    row[modelColumns.value] = to_string(dr.value);
    row[modelColumns.nodeLeftEnd]   = to_string(dr.nodeI.a);
    row[modelColumns.nodeRightEnd]  = to_string(dr.nodeI.b);
    row[modelColumns.valueLeftEnd]  = to_string(dr.valueI.a);
    row[modelColumns.valueRightEnd] = to_string(dr.valueI.b);
}
