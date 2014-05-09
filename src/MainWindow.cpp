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
#include "DataManager.hpp"
#include "FileDialog.hpp"
#include "ModelColumns.hpp"
#include "Glade.hpp"
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
    treeView             = dynamic_cast<TreeView *>    (Glade::loadWidget("dataTable", builder));
    addNodesButton       = dynamic_cast<Button *>      (Glade::loadWidget("addNodesButton", builder));
    removeSelNodesButton = dynamic_cast<Button *>      (Glade::loadWidget("removeSelNodes", builder));
    removeAllNodesButton = dynamic_cast<Button *>      (Glade::loadWidget("removeAllNodes", builder));
    fileChooseButton     = dynamic_cast<Button *>      (Glade::loadWidget("fileChooseButton", builder));
    fileSaveButton       = dynamic_cast<Button *>      (Glade::loadWidget("fileSaveButton", builder));
    interpolButton       = dynamic_cast<Button *>      (Glade::loadWidget("interpolButton", builder));
    halfIntervalRadio    = dynamic_cast<RadioButton *> (Glade::loadWidget("halfIntervalRadio", builder));
    floatRadio           = dynamic_cast<RadioButton *> (Glade::loadWidget("floatRadio", builder));
    fullIntervalRadio    = dynamic_cast<RadioButton *> (Glade::loadWidget("fullIntervalRadio", builder));
    lagrangeRadio        = dynamic_cast<RadioButton *> (Glade::loadWidget("lagrangeRadio", builder));
    nevilleRadio         = dynamic_cast<RadioButton *> (Glade::loadWidget("nevilleRadio", builder));
    statusBar            = dynamic_cast<Label *>       (Glade::loadWidget("statusBar", builder));
    interpolPoint        = dynamic_cast<Entry *>       (Glade::loadWidget("interpolPoint", builder));
    resultEntry          = dynamic_cast<Entry *>       (Glade::loadWidget("resultEntry", builder));

    // Uruchom dataManager'a
    dataManager = new DataManager(builder, treeView);

    // Pobierz dodatkowe okienka - okno dodania nowych węzłów
    Glade::loadFile("../ui/AddNodesWindow.glade", builder);
    builder->get_widget_derived("addNodesWindow", addNodesWindow);      // Okno dodawania węzłów interpolacji
    addNodesWindow->setDataManager(this->dataManager);

    // Dialog wyboru pliku do załadowania
    Glade::loadFile("../ui/FileChooserDialog.glade", builder);
    builder->get_widget_derived("fileChooserDialog", fileDialog);

    // Przygotuj się do przechwytywania sygnału zaznaczenia wiersza
    refTreeSelection = treeView->get_selection();
    refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);

    // Jeśli wszystko poszło OK
    set_default_size(1200, 500);

    CellRendererText *rend = dynamic_cast<CellRendererText *> (treeView->get_column_cell_renderer(0));
    rend->signal_edited().connect(
        sigc::bind<ColumnEdit>(sigc::mem_fun(*this, &MainWindow::onRecordEdit), ColumnEdit::NODE));

    rend = dynamic_cast<CellRendererText *> (treeView->get_column_cell_renderer(1));
    rend->signal_edited().connect(
        sigc::bind<ColumnEdit>(sigc::mem_fun(*this, &MainWindow::onRecordEdit), ColumnEdit::VALUE));

    this->signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::onQuitClick));

    addNodesButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onAddNodesButtonClick));
    removeSelNodesButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveSelNodesButtonClick));
    removeAllNodesButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveAllNodesButtonClick));
    interpolButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onInterpolButtonClick));

    fileChooseButton->signal_clicked().connect(
        sigc::bind<Button *>(sigc::mem_fun(*this, &MainWindow::onFileButtonClick), fileChooseButton));
    fileSaveButton->signal_clicked().connect(
        sigc::bind<Button *>(sigc::mem_fun(*this, &MainWindow::onFileButtonClick), fileSaveButton));
    floatRadio->signal_clicked().connect(
        sigc::bind<RadioButton *>(sigc::mem_fun(*this, &MainWindow::onArthmRadioClick), floatRadio));
    halfIntervalRadio->signal_clicked().connect(
        sigc::bind<RadioButton *>(sigc::mem_fun(*this, &MainWindow::onArthmRadioClick), halfIntervalRadio));
    fullIntervalRadio->signal_clicked().connect(
        sigc::bind<RadioButton *>(sigc::mem_fun(*this, &MainWindow::onArthmRadioClick), fullIntervalRadio));
    lagrangeRadio->signal_clicked().connect(
        sigc::bind<RadioButton *>(sigc::mem_fun(*this, &MainWindow::onAlgorithmRadioClick), lagrangeRadio));
    nevilleRadio->signal_clicked().connect(
        sigc::bind<RadioButton *>(sigc::mem_fun(*this, &MainWindow::onAlgorithmRadioClick), nevilleRadio));

    refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::onRowSelect));

    // Pokaż załadowane okno
    show();
}

MainWindow::~MainWindow() {
    delete resultEntry;
    delete interpolPoint;
    delete statusBar;
    delete treeView;
    delete floatRadio;

    delete lagrangeRadio;
    delete nevilleRadio;
    delete halfIntervalRadio;
    delete fullIntervalRadio;

    delete removeAllNodesButton;
    delete removeSelNodesButton;
    delete fileSaveButton;
    delete fileChooseButton;
    delete addNodesButton;
    delete interpolButton;

    delete dataManager;
    delete fileDialog;
    delete addNodesWindow;
}

void MainWindow::onRecordEdit(const ustring &pathString, const ustring &newText, ColumnEdit column) {
    try {
        // Próbuj zmienić tekst w bazie
        dataManager->changeRecord(pathString, newText, column);
        // Uaktualnij status bar
        onRowSelect();
    } catch (invalid_argument const& e) {
        statusBar->set_label("Niepoprawna wartość - brak możliwości konwersji do long double!");
    } catch (out_of_range const& e) {
        statusBar->set_label("Wpisano wartość spoza zakresu liczby typu long double!");
    } catch (DuplicateNode const& e) {
        statusBar->set_label("Podany węzeł już istnieje w siatce!");
    }
}

void MainWindow::onRowSelect() {
    auto selected = refTreeSelection->get_selected_rows();

    switch (selected.size()) {
        case 0:
            removeSelNodesButton->set_sensitive(false);
            statusBar->set_label("");
            break;
        case 1:
            removeSelNodesButton->set_sensitive(true);
            statusBar->set_label(dataManager->getRecordInfo(*selected.begin()));    // Uaktualnienie status bar
            break;
        default:
            removeSelNodesButton->set_sensitive(true);
            statusBar->set_label("");
            break;
    }
}

void MainWindow::onAddNodesButtonClick() {
    addNodesWindow->run();
}

void MainWindow::onRemoveSelNodesButtonClick() {
    auto selected = refTreeSelection->get_selected_rows();
    dataManager->removeRecords(selected);
}

void MainWindow::onRemoveAllNodesButtonClick() {
    refTreeSelection->unselect_all();
    dataManager->removeAllRecords();

    statusBar->set_label("");
}

void MainWindow::onFileButtonClick(Button *btn) {
    if (btn == this->fileChooseButton)
        fileDialog->setMode(true);                  // Dialog do wczytywania pliku
    else
        fileDialog->setMode(false);                 // Zapisywanie do pliku

    fileDialog->show();                             // Pokaż dialog
    int result = fileDialog->run();                 // Uruchom dialog i przechwyć odpowiedź
    fileDialog->hide();                             // Ukryj dialog

    if (btn == this->fileChooseButton && result == RESPONSE_OK) {
        dataManager->loadFile(fileDialog->get_filename());
        return;
    }
    if (btn == this->fileSaveButton && result == RESPONSE_OK)
        dataManager->saveFile(fileDialog->get_filename());
}

void MainWindow::onInterpolButtonClick() {
    try {
        dataManager->setInterpolPoint(stold(interpolPoint->get_text()));
        dataManager->interpolation();
        resultEntry->set_text(dataManager->getResult());
    } catch (...) {
        cerr << "Coś złapano!\n";
    }
}

void MainWindow::onArthmRadioClick(RadioButton *rb) {
    if (!rb->get_active())
        return;

    if (rb == this->floatRadio)
        dataManager->setArthmetic(Arthmetic::FLOAT_POINT);
    else if (rb == this->halfIntervalRadio)
        dataManager->setArthmetic(Arthmetic::HALF_INTERV);
    else
        dataManager->setArthmetic(Arthmetic::FULL_INTERV);

    onRowSelect();
}

void MainWindow::onAlgorithmRadioClick(RadioButton *rb) {
    if (!rb->get_active())
        return;

    if (rb == this->lagrangeRadio)
        dataManager->setAlgorithm(Algorithm::LAGRANGE);
    else
        dataManager->setAlgorithm(Algorithm::NEVILLE);
}

bool MainWindow::onQuitClick(GdkEventAny *event) {
    hide();
    return true;
}
