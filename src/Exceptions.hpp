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
#include <string>

using namespace std;

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

    struct LoadSharedLibError : public AppInitError {
        LoadSharedLibError(string libName, string error)
            : AppInitError("Nie udało się wczytać biblioteki dynamicznej " + libName + " z powodu\n\t" + error) { }
    };

    struct LoadSharedFunError : public AppInitError {
        LoadSharedFunError(string libName, string funName, string error)
            : AppInitError("Nie udało się wczytać funkcji " + funName + " z biblioteki dynamicznej " + libName +
                           " z powodu\n\t" + error) { }
    };

struct UserError : public AppError {
    UserError(string strError) : AppError("Błąd użytkownika:\n\t" + strError) { }
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
