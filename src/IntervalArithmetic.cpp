/*
 * iArthm.cpp
 *
 *  Created on: 20-11-2012
 *      Author: Tomasz Hoffmann and Andrzej Marciniak
 */

#include "IntervalArithmetic.hpp"

#include "Exceptions.hpp"
#include <iomanip>

interval::interval() : interval(0) { }

interval::interval(int n) : interval(to_string(n)) { }

interval::interval(long double la, long double lb) {
    if (lb > la) {
        a = la;
        b = lb;
    } else
        throw IntervalEndsError();
}

interval::interval(const string &s) {
    read(s);
}

interval::~interval() { }

long double interval::width() {
    fesetround(FE_UPWARD);
    long double w = b - a;
    fesetround(FE_TONEAREST);
    return w;
}

interval interval::operator+(const interval& y) const {
    interval r;
    fesetround(FE_DOWNWARD);
    r.a = this->a + y.a;
    fesetround(FE_UPWARD);
    r.b = this->b + y.b;
    fesetround(FE_TONEAREST);
    return r;
}

interval interval::operator-(const interval& y) const {
    interval r;
    fesetround(FE_DOWNWARD);
    r.a = this->a - y.b;
    fesetround(FE_UPWARD);
    r.b = this->b - y.a;
    fesetround(FE_TONEAREST);
    return r;
}

interval interval::operator*(const interval& y) const {
    interval r;
    long double x1y1, x1y2, x2y1;

    fesetround(FE_DOWNWARD);
    x1y1 = this->a * y.a;
    x1y2 = this->a * y.b;
    x2y1 = this->b * y.a;
    r.a = this->b * y.b;
    if (x2y1 < r.a)
        r.a = x2y1;
    if (x1y2 < r.a)
        r.a = x1y2;
    if (x1y1 < r.a)
        r.a = x1y1;

    fesetround(FE_UPWARD);
    x1y1 = this->a * y.a;
    x1y2 = this->a * y.b;
    x2y1 = this->b * y.a;

    r.b = this->b * y.b;
    if (x2y1 > r.b)
        r.b = x2y1;
    if (x1y2 > r.b)
        r.b = x1y2;
    if (x1y1 > r.b)
        r.b = x1y1;
    fesetround(FE_TONEAREST);
    return r;
}

interval interval::operator/(const interval& y) const {
    interval r;
    long double x1y1, x1y2, x2y1, t;

    if ((y.a <= 0) && (y.b >= 0)) {
        throw runtime_error("Division by an interval containing 0");
    } else {
        fesetround(FE_DOWNWARD);
        x1y1 = this->a / y.a;
        x1y2 = this->a / y.b;
        x2y1 = this->b / y.a;
        r.a  = this->b / y.b;
        t = r.a;
        if (x2y1 < t)
            r.a = x2y1;
        if (x1y2 < t)
            r.a = x1y2;
        if (x1y1 < t)
            r.a = x1y1;

        fesetround(FE_UPWARD);
        x1y1 = this->a / y.a;
        x1y2 = this->a / y.b;
        x2y1 = this->b / y.a;

        r.b = this->b / y.b;
        t = r.b;
        if (x2y1 > t)
            r.b = x2y1;
        if (x1y2 > t)
            r.b = x1y2;
        if (x1y1 > t)
            r.b = x1y1;

    }
    fesetround(FE_TONEAREST);
    return r;
}

interval& interval::operator+=(const interval& y) {
    *this = *this + y;
    return *this;
}

interval& interval::operator-=(const interval& y) {
    *this = *this - y;
    return *this;
}

interval& interval::operator*=(const interval& y) {
    *this = *this * y;
    return *this;
}

interval& interval::operator/=(const interval& y) {
    *this = *this / y;
    return *this;
}

interval interval::read(const string & s) {
    mpfr_t rop;
    mpfr_init2(rop, 80);
    mpfr_set_str(rop, s.c_str(), 10, MPFR_RNDD);

    long double le = mpfr_get_ld(rop, MPFR_RNDD);
    mpfr_set_str(rop, s.c_str(), 10, MPFR_RNDU);
    long double re = mpfr_get_ld(rop, MPFR_RNDU);
    fesetround(FE_TONEAREST);

    this->a = le;
    this->b = re;
}

string interval::write() const {
    ostringstream S;
    S << showpos << setprecision(25) << "[" << a << "; " << b << "]";
    return S.str();
}
