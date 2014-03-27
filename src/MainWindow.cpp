/*
 * mainwindow.cpp
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

#include "MainWindow.hpp"
#include "Exceptions.hpp"

#include <gtkmm.h>
#include <glibmm.h>

MainWindow::MainWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject), builder(refBuilder), notebook(0), treeView(0) {
    builder->get_widget("notebook1", notebook);
    builder->get_widget("dataTable", treeView);
    builder->get_widget("quitButton", quitButton);
    dataBase = RefPtr<ListStore>::cast_static(refBuilder->get_object("dataBase"));
    
    if (notebook && treeView && dataBase && quitButton) {
        this->set_default_size(640, 480);

        //TreeModel::Row row = *(dataBase->append());
        //row.set_value(0, ustring("gtk-about"));
        //row.set_value(0, ustring("5.5"));
        //row.set_value(1, ustring("34.798"));
        //row.set_value(2, ustring("Row generated at run-time!"));

        CellRendererText *rend = dynamic_cast<CellRendererText *> (treeView->get_column_cell_renderer(0));
        rend->signal_edited().connect(
            sigc::mem_fun(*this, &MainWindow::on_node_edited)
        );

        quitButton->signal_clicked().connect(
            sigc::mem_fun(*this, &MainWindow::on_button_quit)
        );

        this->show();
    } else {
        throw AppSetupError("Cannot get all needed widgets from .glade file!");
    }
}

MainWindow::~MainWindow() {
    delete notebook;
    delete treeView;
}

void MainWindow::on_node_edited(const ustring &pathString, const ustring &newText) {
    Gtk::TreePath path(pathString);

    long double newValue = strtold(newText.c_str(), nullptr);
    if (newValue > 10.0f) {
        //Get the row from the path:
        Gtk::TreeModel::iterator iter = dataBase->get_iter(path);
        if (iter) {
            Gtk::TreeModel::Row row = *iter;

            //Put the new value in the model:
            row.set_value(0, newText);
        }
    }

    //Convert the inputed text to an integer, as needed by our model column:
    //~ char *pchEnd = 0;
    //~ int new_value = strtol(new_text.c_str(), &pchEnd, 10);

    //~ if (new_value > 10) {
        //~ //Prevent entry of numbers higher than 10.
//~ 
        //~ //Tell the user:
        //~ Gtk::MessageDialog dialog(*this,
                //~ "The number must be less than 10. Please try again.",
                //~ false, Gtk::MESSAGE_ERROR);
        //~ dialog.run();
//~ 
        //~ //Start editing again, with the bad text, so that the user can correct it.
        //~ //A real application should probably allow the user to revert to the
        //~ //previous text.
//~ 
        //~ //Set the text to be used in the start_editing signal handler:
        //~ m_invalid_text_for_retry = new_text;
        //~ m_validate_retry = true;
//~ 
        //~ //Start editing again:
        //~ m_TreeView.set_cursor(path, m_treeviewcolumn_validated,
                //~ m_cellrenderer_validated, true /* start_editing */);
    //~ }
    //~ else
    //~ {
}

void MainWindow::on_button_quit() {
    hide();
}
