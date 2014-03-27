/* 
 * File:   exceptions.hpp
 * Author: bartas
 *
 * Created on December 15, 2013, 6:42 PM
 */

#ifndef EXCEPTIONS_HPP
#define	EXCEPTIONS_HPP

#include <exception>
#include <string>

using namespace std;

struct AppSetupError : public exception {
    public:
        AppSetupError(string error) : error(error) { }
        ~AppSetupError() throw() { }
        const char *what() const throw() { return error.c_str(); }
    private:
        string error;
};

#endif	/* EXCEPTIONS_HPP */

