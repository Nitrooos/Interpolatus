#include "App.hpp"
#include "Glade.hpp"
#include "Exceptions.hpp"

#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>

App::App(int &argc, char *argv[])
    : gtkApp(Application::create(argc, argv, "org.gtkmm.examples.base")) {
    setup();
}

App::~App() {
    delete mainWindow;
}

void App::setup() {
    // Load the GtkBuilder file and instantiate its widgets:
    RefPtr<Builder> refBuilder = Builder::create();
    Glade::loadFile("../ui/Interpolatus.glade", refBuilder);

    // Spróbuj pobrać z pliku potrzebne widgety
    refBuilder->get_widget_derived("mainWindow", mainWindow);
}

void App::run() {    
    gtkApp->run(*mainWindow);
}

int main(int argc, char *argv[]) {
    try {
        App app(argc, argv);
        app.run();

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
