/*
 * File:   exceptions.hpp
 * Author: bartas
 *
 * Created on March 15, 2014, 6:42 PM
 */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <gtkmm/messagedialog.h>
#include <glibmm/ustring.h>
#include <string>

using namespace std;
using namespace Gtk;
using namespace Glib;

struct AppError : public exception {
    AppError(string strError) : error(strError) { }
    const char *what() const throw() { return error.c_str(); }
    virtual void message() const throw() {
        MessageDialog dialog{ustring{error}, false, MESSAGE_ERROR, BUTTONS_CLOSE};
        dialog.run();
    }
    protected:
        string error;
};

struct AppInitError : public AppError {
    AppInitError(string strError) : AppError("Nie udało się uruchomić aplikacji z powodu:\n\t" + strError) { }
};

    struct LoadGladeFileError : public AppInitError {
        LoadGladeFileError(string filename, string error) : AppInitError("Nie udało się wczytać pliku " + filename +
            " z powodu " + error) { }
    };

    struct LoadWidgetError : public AppInitError {
        LoadWidgetError(string widget)
            : AppInitError("Nie udało się wczytać widgetu: " + widget) { }
    };

struct UserError : public AppError {
    UserError(string strError) : AppError("Błąd użytkownika:\n\t" + strError) { }
};

    struct IntervalEndsError : public UserError {
        IntervalEndsError() : UserError("Prawy koniec przedziału musi być większy od lewego!") { }
    };

    struct DuplicateNode : public UserError {
        DuplicateNode(string node) : UserError("Węzeł " + node + " już istnieje w siatce!") { }
    };

    struct EmptyData : public UserError {
        EmptyData() : UserError("Użytkownik nie podał ani jednego węzła dla siatki!") { }
    };

    struct LoadIDFError : public UserError {
        LoadIDFError(string filename) : UserError("Nieprawidłowy plik danych " + filename) { }
    };

#endif  /* EXCEPTIONS_HPP */
