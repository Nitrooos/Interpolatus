#include "App.hpp"

#include "Exceptions.hpp"

#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>

App::App(int &argc, char *argv[])
    : gtkApp(Gtk::Application::create(argc, argv, "org.gtkmm.examples.base")),
      mainWindow(0) {
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
        throw AppSetupError("FileError: " + ex.what());
    }
    catch(const Glib::MarkupError& ex) {
        throw AppSetupError("MarkupError: " + ex.what());
    }
    catch(const Gtk::BuilderError& ex) {
        throw AppSetupError("BuilderError: " + ex.what());
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
    catch (std::exception const& e) {
        std::cerr << "Running app failed:\n" << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "Unexpected exception has occured!\n";       // Kernel Panic
    }
    return 1;
}
