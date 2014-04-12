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
#include <gtkmm/dialog.h>
#include <glibmm/refptr.h>

#include "ModelColumns.hpp"

class AddNodesWindow;

using namespace Gtk;
using namespace Glib;
using namespace std;

class MainWindow: public Window {
    public:
        MainWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder);
        virtual ~MainWindow();

    private:
        void onQuitClick();
        void onAddNodesButtonClick();
        void onRemoveAllNodesButtonClick();
        void onAddNodesWindowConfirmClick();
        void onNodeEdited(const ustring &path, const ustring &new_text);
        void onNodeSelect();

        bool isNodeUnique(ustring node) const;

        RefPtr<Builder> builder;
        RefPtr<TreeSelection> refTreeSelection;
        RefPtr<ListStore> dataBase;
        ModelColumns   modelColumns;

        AddNodesWindow *addNodesWindow{nullptr};
        Button         *addNodesButton{nullptr};
        Button         *removeAllNodesButton{nullptr};
        Button         *quitButton{nullptr};
        TreeView       *treeView{nullptr};
        Label          *statusBar{nullptr};

        int nRecords{0};
};

#endif /* MAINWINDOW_H */ 
