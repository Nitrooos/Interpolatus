/*
 * mainwindow.h
 *
 * Copyright 2014 Bartosz Kostaniak <bartas@debian>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/radiobutton.h>
#include <glibmm/refptr.h>
#include "ModelColumns.hpp"

#include "Types.hpp"

using namespace Gtk;
using namespace Glib;
using namespace std;

class AddNodesWindow;
class DataManager;
class LoadFileDialog;

class MainWindow: public Window {
    public:
        MainWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder);
        virtual ~MainWindow();
    private:
        bool onQuitClick(GdkEventAny *event);
        void onAddNodesButtonClick();
        void onRemoveSelNodesButtonClick();
        void onRemoveAllNodesButtonClick();
        void onFileChooseButtonClick();
        void onArthmRadioClick(RadioButton *rb);
        void onAlgorithmRadioClick(RadioButton *rb);
        void onRecordEdit(const ustring &path, const ustring &newText, ColumnEdit column);
        void onRowSelect();

        RefPtr<Builder> builder;
        RefPtr<TreeSelection> refTreeSelection;

        AddNodesWindow *addNodesWindow{nullptr};
        LoadFileDialog *loadFileDialog{nullptr};
        DataManager    *dataManager{nullptr};

        Button         *addNodesButton{nullptr};
        Button         *removeSelNodesButton{nullptr};
        Button         *removeAllNodesButton{nullptr};
        Button         *fileChooseButton{nullptr};

        RadioButton    *floatRadio{nullptr};
        RadioButton    *halfIntervalRadio{nullptr};
        RadioButton    *fullIntervalRadio{nullptr};
        RadioButton    *lagrangeRadio{nullptr};
        RadioButton    *nevilleRadio{nullptr};

        TreeView       *treeView{nullptr};
        Label          *statusBar{nullptr};
        Entry          *interpolPoint{nullptr};
};

#endif /* MAINWINDOW_H */
