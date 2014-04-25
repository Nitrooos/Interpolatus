/* 
 * File:   exceptions.hpp
 * Author: bartas
 *
 * Created on March 15, 2014, 6:42 PM
 */

#ifndef EXCEPTIONS_HPP
#define	EXCEPTIONS_HPP

#include <exception>
#include <string>

using namespace std;

struct AppInitError : public exception {
    public:
        AppInitError(string error)
            : error("Rzucono wyjątek klasy AppInitError:\n" + error) { }
        ~AppInitError() throw() { }
        const char *what() const throw() { return error.c_str(); }
    private:
        string error;
};

struct LoadWidgetError : public exception {
    public:
        LoadWidgetError(string widget)
            : widget("Rzucono wyjątek klasy LoadWidgetError:\nFailed to load widget: " + widget) { }
        ~LoadWidgetError() throw() { }
        const char *what() const throw() { return widget.c_str(); }
    private:
        string widget;
};

struct DuplicateNode : public exception {
    DuplicateNode() { }
    ~DuplicateNode() throw() { }
    const char *what() const throw() { return "Rzucono wyjątek klasy DuplicateNode: Podany węzeł już istnieje"; }
};

#endif	/* EXCEPTIONS_HPP */

