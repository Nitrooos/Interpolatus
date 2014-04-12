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
#include "AddNodesWindow.hpp"
#include "ModelColumns.hpp"
#include "Exceptions.hpp"

#include <gtkmm.h>
#include <glibmm.h>

#include <iostream>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject), builder(refBuilder)
{
    // Pobierz podstawowe widgety z pliku Glade
    builder->get_widget("dataTable", treeView);
    if (!builder)
        throw LoadWidgetError("treeView");
    
    builder->get_widget("quitButton", quitButton);
    if (!quitButton)
        throw LoadWidgetError("quitButton");
        
    builder->get_widget("addNodesButton", addNodesButton);
    if (!builder)
        throw LoadWidgetError("addNodesButton");
        
    builder->get_widget("removeAllNodes", removeAllNodesButton);
    if (!builder)
        throw LoadWidgetError("removeAllNodesButton");
        
    builder->get_widget("statusBar", statusBar);
    if (!builder)
        throw LoadWidgetError("statusBar");
        
    dataBase = RefPtr<ListStore>::cast_static(builder->get_object("dataBase"));
    if (!builder)
        throw LoadWidgetError("dataBase");

    // Przygotuj się do przechwytywania sygnału zaznaczenia wiersza
    refTreeSelection = treeView->get_selection();

    // Pobierz dodatkowe okienka
    builder->get_widget_derived("addNodesWindow", addNodesWindow);      // Okno dodawania węzłów interpolacji

    // Jeśli wszystko poszło OK
    set_default_size(640, 480);

    CellRendererText *rend = dynamic_cast<CellRendererText *> (treeView->get_column_cell_renderer(1));
    rend->signal_edited().connect(
        sigc::mem_fun(*this, &MainWindow::onNodeEdited));

    quitButton->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onQuitClick));

    addNodesButton->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onAddNodesButtonClick));

    removeAllNodesButton->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onRemoveAllNodesButtonClick));

    addNodesWindow->getConfirmButton().signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onAddNodesWindowConfirmClick));

    refTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this, &MainWindow::onNodeSelect));

    show();
}

MainWindow::~MainWindow() {
    delete statusBar;
    delete treeView;
    delete quitButton;
    delete removeAllNodesButton;
    delete addNodesButton;
    delete addNodesWindow;

    cout << "MainWindow destructor end\n";
}

void MainWindow::onNodeEdited(const ustring &pathString, const ustring &newText) {
    TreePath path(pathString);

    try {
        // Próba konwersji wpisanego tekstu na liczbę
        long double newValue = stold(newText.raw());

        // Czy węzeł jest unikalny?
        if (!isNodeUnique(newText))
            statusBar->set_label("Podany węzeł już istnieje w bieżącej siatce!\n");

        // Uzyskanie iteratora ze ścieżki path
        TreeModel::iterator iter = dataBase->get_iter(path);
        if (iter) {
            Gtk::TreeModel::Row row = *iter;

            // Wszystko w porządku - zmień wartość w wierszu
            row[modelColumns.node] = newText;

            // Uaktualnij status bar
            onNodeSelect();
        }
    } catch (invalid_argument const& e) {
        statusBar->set_label("Niepoprawna wartość - brak możliwości konwersji do long double!");
    } catch (out_of_range const& e) {
        statusBar->set_label("Wpisano wartość spoza zakresu liczby typu long double!");
    }
}

void MainWindow::onNodeSelect() {
    // Pobierz zaznaczony wiersz
    auto row = *(refTreeSelection->get_selected());
    
    if (!row)                           // Jeśli nie ma zaznaczonego wiersza...
        return;                         // to nie mamy co robić!

    // Jakie wartości zmiennopozycyjne kryją się w poszczególnych komórkach?
    long double node  = stold(row.get_value(modelColumns.node ).raw()),
                value = stold(row.get_value(modelColumns.value).raw());
    ostringstream S;
    S << "x = " << fixed << setprecision(25) << node << "\t\tf(x) = " << value;

    // Uaktualnienie status bar
    statusBar->set_label(ustring(S.str()));
}

void MainWindow::onAddNodesButtonClick() {
    addNodesWindow->show();
    addNodesWindow->present();
}

void MainWindow::onRemoveAllNodesButtonClick() {
    refTreeSelection->unselect_all();
    dataBase->clear();
    nRecords = 0;
}

void MainWindow::onAddNodesWindowConfirmClick() {
    TreeModel::Row row;
    long double startValue = addNodesWindow->getStartValue(), step = addNodesWindow->getStep();
    int addedRows = 0;

    for (int i = 0, n = addNodesWindow->getnNodes(); i < n; ++i) {
        auto value = ustring(to_string(startValue + i*step));

        if (!isNodeUnique(value))
            continue;
        
        row = *(dataBase->append());
        row[modelColumns.id   ] = guint(nRecords + addedRows);
        row[modelColumns.node ] = value;
        row[modelColumns.value] = ustring("0.0");

        ++addedRows;
    }
    nRecords += addedRows;
}

void MainWindow::onQuitClick() {
    hide();
}

bool MainWindow::isNodeUnique(ustring node) const {
    for (auto &x : dataBase->children())
        if (x[modelColumns.node] == node)
            return false;
    return true;
}
