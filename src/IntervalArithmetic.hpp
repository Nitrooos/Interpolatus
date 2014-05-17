/*
 * IntervalArithmetic.h
 *
 *  Created on: 20-11-2012
 *      Author: Tomasz Hoffmann and Andrzej Marciniak
 *
 *  Before you start use module, please install libraries:
 *      - .boost ( http://www.boost.org/ )
 *      - GNU MPFR ( http://www.mpfr.org/ )
 */

#ifndef INTERVALARITHMETIC_H_
#define INTERVALARITHMETIC_H_

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <fenv.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpfr.h>

using namespace std;

//set precision
const int curr_precision = 80;

// store the original rounding mode
const int originalRounding = fegetround();
const char DecimalSeparator = '.';

class interval {
    public:
        interval();
        interval(int n);
        interval(long double la, long double lb);
        interval(const string &s);

        virtual ~interval();

        long double width();

        interval operator+(const interval& y) const;
        interval operator-(const interval& y) const;
        interval operator*(const interval& y) const;
        interval operator/(const interval& y) const;

        interval& operator+=(const interval& y);
        interval& operator-=(const interval& y);
        interval& operator*=(const interval& y);
        interval& operator/=(const interval& y);

        interval read(const string& s);
        string write() const;

        long double a, b;
};

#endif /* INTERVALARITHMETIC_H_ */
