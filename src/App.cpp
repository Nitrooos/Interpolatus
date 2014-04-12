#include "App.hpp"

#include "Exceptions.hpp"

#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>

App::App(int &argc, char *argv[])
    : gtkApp(Gtk::Application::create(argc, argv, "org.gtkmm.examples.base")),
      mainWindow(nullptr) {
    setup();
}

App::~App() {
    delete mainWindow;
}

void App::setup() {
    // Load the GtkBuilder file and instantiate its widgets:
    RefPtr<Builder> refBuilder = Builder::create();
    try {
        refBuilder->add_from_file("../ui/treeView.glade");
    }
    catch(const Glib::FileError& ex) {
        throw AppInitError("FileError: " + ex.what());
    }
    catch(const Glib::MarkupError& ex) {
        throw AppInitError("MarkupError: " + ex.what());
    }
    catch(const Gtk::BuilderError& ex) {
        throw AppInitError("BuilderError: " + ex.what());
    }

    // Spróbuj pobrać z pliku potrzebne widgety
    refBuilder->get_widget_derived("mainWindow", mainWindow);
}

void App::run() {    
    gtkApp->run(*mainWindow);
}

int main(int argc, char *argv[]) {
    try {
        App *app = new App(argc, argv);
        app->run();
        delete app;

        return 0;
    }
    catch (LoadWidgetError const& e) {
        MessageDialog dialog{ustring{e.what()}, false, MESSAGE_ERROR, BUTTONS_CLOSE};
        dialog.run();
    }
    catch (AppInitError const& e) {
        cerr << e.what() << "\n";
    }
    catch (...) {
        cerr << "Unexpected exception has occured!\n";       // Kernel Panic
    }
    return 1;
}
