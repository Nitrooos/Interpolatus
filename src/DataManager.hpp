/*
 * DataManager.hpp
 *
 * Copyright 2014 Bartosz Kostaniak <bartas@debian>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 *
 */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <string>
#include <map>

using namespace std;
using namespace Gtk;
using namespace Glib;

#include "Types.hpp"
#include "ModelColumns.hpp"

struct AddNodesInfo;

class DataManager {
    public:
        DataManager(RefPtr<Builder> const& builder, TreeView *treeView);
        ~DataManager();

        void setArthmetic(Arthmetic mode);
        void setAlgorithm(Algorithm algorithm);
        void setInterpolPoint(long double point);

        void loadFile(string name);
        void saveFile(string name);

        /* NIEDOKOŃCZONE */void changeRecord(ustring const& path, ustring const& value, ColumnEdit col);
        /* NIEDOKOŃCZONE */void addRecords(Info::AddNodes const& info);
        void removeRecords(vector<TreePath> const& sel);
        void removeAllRecords();
        /* NIEDOKOŃCZONE */string getRecordInfo(TreePath const& path);
        Arthmetic whatArthmetic() const;
        ustring getResult() const;

        void interpolation();
    private:
        bool isNodeUnique(long double node) const;
        map<long double, long double>::iterator getNodeF(string number);
        map<interval, interval>::iterator       getNodeHI(string number);
        map<interval, interval>::iterator       getNodeFI(string leftEnd, string rightEnd);

        Arthmetic arthm{Arthmetic::FLOAT_POINT};
        Algorithm algorithm{Algorithm::LAGRANGE};
        long double interpolPoint{0.0};

        // Uchwyt na bibliotekę .so
        void *libHandle;
        // Wskaźniki na funkcje udostępniane przed bibliotekę .so
        long double (*lagrange)(map<long double, long double> &, long double, int &),
                    (*neville )(map<long double, long double> &, long double, int &);

        TreeView *treeView{nullptr};
        map<long double, long double> dataFloatPoint;
        map<interval, interval> dataHalfInterval;
        map<interval, interval> dataFullInterval;

        RefPtr<ListStore> floatBase;
        RefPtr<ListStore> halfIntervalBase;
        RefPtr<ListStore> fullIntervalBase;
        ModelColumnsFloat mColFloat;
        ModelColumnsInterval mColInterval;

        long double resultFloat{0.0};
        interval resultHalfInterval{0.0, 0.0},
                 resultFullInterval{0.0, 0.0};
};

#endif /* DATAMANAGER_H */
