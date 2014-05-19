#include "Algorithms.hpp"

template<typename T>
int validateData(mymap const& data) {
    if (data.empty())
            return 1;
    else {
        for (auto it = data.begin(); it != data.end(); ++it)
            for (auto it2 = next(it); it2 != data.end(); ++it2)
                if (getNode<T>(it) == getNode<T>(it2))
                    return 2;
    }
    return 0;
}

template<typename T>
T lagrange(mymap const &data, T xx, int &st) {
    if ( (st = validateData<T>(data)) )
        return T(0);

    T fx = 0.0L, p;
    for (mymap::const_iterator i = data.begin(); i != data.end(); ++i) {
        p = 1.0L;
        for (mymap::const_iterator k = data.begin(); k != data.end(); ++k)
            if (i->first != k->first)
                p *= (xx - getNode<T>(k))/(getNode<T>(i) - getNode<T>(k));
        fx += getValue<T>(i)*p;
    }
    return fx;
}

template<typename T>
T neville (mymap const &data, T xx, int &st) {
    if ( (st = validateData<T>(data)) )
        return T(0);

    map<int, Info::DataRecord> t(data);
    for (mymap::const_iterator k = t.begin(); k != t.end(); ++k) {
        for (mymap::iterator i = prev(t.end()); i->first != k->first; --i) {
            getValue<T>(i) = ((xx - getNode<T>(prev(i, k->first)))*getValue<T>(i) - (xx - getNode<T>(i))*getValue<T>(prev(i)))/
                              (getNode<T>(i) - getNode<T>(prev(i, k->first)));
        }
    }
    return getValue<T>(prev(t.end()));
}

template<typename T>
vector<T> calcFactors(mymap const &data, int &st) {
    vector<T> res(data.size(), 0.0L);

    if ( (st = validateData<T>(data)) )
        return res;

    for (auto it = data.begin(); it != data.end(); ++it) {

        T f           = getValue<T>(it),
          denominator = 1;

        // computes the dominator from the formula (obliczanie mianownika dla danego i we wzorze Lagrange'a)
        for (auto kit = data.begin(); kit != data.end(); ++kit)
            if (kit != it)
                denominator *= getNode<T>(it) - getNode<T>(kit);

        // starting the algorithm that fills the table
        int n = data.size();

        vector<vector<T>> tab(n);         // w każdej iteracji wektor powstaje na nowo
        for (int i = 0; i < n; ++i)
            tab[i].resize(n);                       // macierz kwadratowa NxN

        // filling the first line with 1 (wypełnianie pierwszej kolumny - współczynniki przy x^n)
        for (int i = 0; i < n; ++i)
            tab[0][i] = 1;

        auto kit = data.begin();
        // filling the first row with 0 except from the first cell (wypełnianie pierwszego wiersza)
        for (int i = 1; i < n; ++i)
            tab[i][0] = 0;

        // filling the second with with it->first (kolumna 1 wiersz 0)
        if (kit != it)
            tab[1][0] = getNode<T>(kit);
        ++kit;

        // filling the rest
        for (int i = 1; i < n; ++i) {
            for (int j = 1; j < n; ++j) {
                tab[j][i] += tab[j][i-1];
                if (kit != it)
                    tab[j][i] += tab[j-1][i-1]*getNode<T>(kit);
            }
            ++kit;
        }

        // naprzemiennie wsp należy dodawać i odejmować
        for (int i = 0; i < n; ++i)
            if (i%2 == 0)
                res[i] += f/denominator*tab[i][n-1];
            else
                res[i] -= f/denominator*tab[i][n-1];
    }
    return res;
}

template long double lagrange<long double>(mymap const &data, long double xx, int &st);
template interval    lagrange<interval>   (mymap const &data, interval xx, int &st);

template long double neville<long double> (mymap const &data, long double xx, int &st);
template interval    neville<interval>    (mymap const &data, interval xx, int &st);

template vector<long double> calcFactors<long double> (mymap const &data, int &st);
template vector<interval>    calcFactors<interval>    (mymap const &data, int &st);

/*int validateData(map<int, Info::DataRecord> const& data, Arthmetic arthm) {
    if (data.empty())
            return 1;
    else {
        switch (arthm) {
            case Arthmetic::FLOAT_POINT:
                for (auto it = data.begin(); it != data.end(); ++it)
                    for (auto it2 = next(it); it2 != data.end(); ++it2)
                        if (it->second.node == it2->second.node)
                            return 2;
                break;
            default:
                for (auto it = data.begin(); it != data.end(); ++it)
                        for (auto it2 = next(it); it2 != data.end(); ++it2)
                            if (it->second.nodeI.a == it2->second.nodeI.a &&
                                it->second.nodeI.b == it2->second.nodeI.b)
                                return 2;
                break;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------
//
//  The function Lagrange calculates the value of a polynomial given by
//  Lagrange's interpolation formula.
//  Data:
//    n  - number of interpolation nodes minus 1 (the degree of polynomial
//         is at most n,
//    x  - an array containing the values of interpolation nodes,
//    f  - an array containing the values of function,
//    xx - the point at which the value of the Lagrange interpolating
//         polynomial should be calculated.
//  Result:
//    Lagrange(n,x,f,xx,st) - the value of the Lagrange interpolating
//                            polynomial at xx.
//  Other parameters:
//    st - a variable which within the function Lagrange is assigned the
//         value of:
//           1, if n<0,
//           2, if there exist x[i] and x[j] (i<>j; i,j=0,1,...,n) such
//              that x[i]=x[j],
//           0, otherwise.
//         Note: If st=1 or st=2, then Lagrange(n,x,f,xx,st) is not
//               calculated.
//  Unlocal identifier:
//    vector - a type identifier of extended array [q0..qn], where q0<=0 and
//             qn>=n.
//
//---------------------------------------------------------------------------

long double lagrange(map<int, Info::DataRecord> const& data, long double xx, int &st) {
    if ( (st = validateData(data, Arthmetic::FLOAT_POINT)) )
        return 0.0;

    long double fx = 0.0, p;
    for (auto &i : data) {
        p = 1.0;
        for (auto &k : data)
            if (i.first != k.first)
                p *= (xx - k.second.node)/(i.second.node - k.second.node);
        fx += i.second.value*p;
    }
    return fx;
}

interval lagrangeI(map<int, Info::DataRecord> const& data, interval const& xx, int &st) {
    if ( (st = validateData(data, Arthmetic::FULL_INTERV)) )
        return interval(0);

    interval fx = 0, p;
    for (auto &i : data) {
        p = 1;
        for (auto &k : data)
            if (i.first != k.first)
                p *= (xx - k.second.nodeI)/(i.second.nodeI - k.second.nodeI);
        fx += i.second.valueI*p;
    }
    return fx;
}

//---------------------------------------------------------------------------
//
//  The function Neville calculates the value of interpolating polynomial by
//  Neville's algorithm.
//  Data:
//    n  - number of interpolation nodes minus 1 (the degree of polynomial
//         is at most n),
//    x  - an array containing the values of interpolation nodes,
//    xx - the point at which the value of interpolating polynomial should
//         be calculated,
//    f  - an array containing the values of function (changed on exit).
//  Result:
//    Neville(n,x,xx,f,st) - the value of interpolating polynomial at xx.
//  Other parameters:
//    st - a variable which within the function Neville is assigned the
//         value of:
//           1, if n<0,
//           2, if there exist x[i] and x[j] (i<>j, i,j=0,1,...,n) such that
//              x[i]=x[j],
//           0, otherwise.
//         Note: If st=1 or st=2, then Neville(n,x,xx,f,st) is not
//               calculated.
//  Unlocal identifier:
//    vector - a type identifier of extended array [q0..qn], where q0<=0 and
//             qn>=n.
//
//---------------------------------------------------------------------------

long double neville(map<int, Info::DataRecord> const& data, long double xx, int &st) {
    if ( (st = validateData(data, Arthmetic::FLOAT_POINT)) )
        return 0.0;

    map<int, Info::DataRecord> t(data);
    for (auto k = t.begin(); k != t.end(); ++k) {
        for (auto i = t.rbegin(); i->first != k->first; ++i) {
            i->second.value = ((xx - next(i, k->first)->second.node)*i->second.value - (xx - i->second.node)*next(i)->second.value)/
                               (i->second.node - next(i, k->first)->second.node);
        }
    }
    return t.rbegin()->second.value;
}

interval nevilleI(map<int, Info::DataRecord> const& data, interval const& xx, int &st) {
    if ( (st = validateData(data, Arthmetic::FULL_INTERV)) )
        return interval(0);

    map<int, Info::DataRecord> t(data);
    for (auto k = t.begin(); k != t.end(); ++k) {
        for (auto i = t.rbegin(); i->first != k->first; ++i) {
            i->second.valueI = ((xx - next(i, k->first)->second.nodeI)*i->second.valueI - (xx - i->second.nodeI)*next(i)->second.valueI)/
                               (i->second.nodeI - next(i, k->first)->second.nodeI);
        }
    }
    return t.rbegin()->second.valueI;
}

//---------------------------------------------------------------------------
//
//  Zajebisty opis funkcji factors!!!
//
//---------------------------------------------------------------------------

vector<long double> calcFactors(map<int, Info::DataRecord> const& data, int &st) {
    vector<long double> res(data.size(), 0.0);

    if ( (st = validateData(data, Arthmetic::FLOAT_POINT)) )
        return res;

    for (auto it = data.begin(); it != data.end(); ++it) {

        long double f = it->second.value;
        long double denominator = 1.0;

        // computes the dominator from the formula (obliczanie mianownika dla danego i we wzorze Lagrange'a)
        for (auto kit = data.begin(); kit != data.end(); ++kit)
            if (kit != it)
                denominator *= it->second.node - kit->second.node;

        // starting the algorithm that fills the table
        int n = data.size();

        vector<vector<long double>> tab(n);         // w każdej iteracji wektor powstaje na nowo
        for (int i = 0; i < n; ++i)
            tab[i].resize(n);                       // macierz kwadratowa NxN

        // filling the first line with 1 (wypełnianie pierwszej kolumny - współczynniki przy x^n)
        for (int i = 0; i < n; ++i)
            tab[0][i] = 1;

        auto kit = data.begin();
        // filling the first row with 0 except from the first cell (wypełnianie pierwszego wiersza)
        for (int i = 1; i < n; ++i)
            tab[i][0] = 0;

        // filling the second with with it->first (kolumna 1 wiersz 0)
        if (kit != it)
            tab[1][0] = kit->second.node;
        ++kit;

        // filling the rest
        for (int i = 1; i < n; ++i) {
            for (int j = 1; j < n; ++j) {
                tab[j][i] += tab[j][i-1];
                if (kit != it)
                    tab[j][i] += tab[j-1][i-1]*kit->second.node;
            }
            ++kit;
        }

        // naprzemiennie wsp należy dodawać i odejmować
        for (int i = 0; i < n; ++i)
            if (i%2 == 0)
                res[i] += f/denominator*tab[i][n-1];
            else
                res[i] -= f/denominator*tab[i][n-1];
    }
    return res;
}*/

