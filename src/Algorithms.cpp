#include "Algorithms.hpp"

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

long double Algorithms::lagrange(int n, vector<long double> const &x, vector<long double> &f, long double xx, int &st) {
    int i, k;
    long double fx, p;

    if (n < 0)
        st = 1;
    else {
        st = 0;
        if (n > 0) {
            i = -1;
            do {
                ++i;
                for (k = i + 1; k <= n; ++k)
                    if (x[i] == x[k])
                        st = 2;
            } while ((i != n - 1) && (st != 2));
        }
        if (st == 0) {
            fx = 0;
            for (i = 0; i <= n; ++i) {
                p = 1;
                for (k = 0; k <= n; ++k)
                    if (k != i)
                        p = p*(xx - x[k])/(x[i] - x[k]);
                fx = fx + f[i]*p;
            }
            return fx;
        }
    }
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

long double Algorithms::neville(int n, vector<long double> const &x, vector<long double> &f, long double xx, int &st) {
    int i, k;
    if (n < 0)
        st = 1;
    else {
        st = 0;
        if (n > 0) {
            i = -1;
            do {
                ++i;
                for (k = i + 1; k <= n; ++k)
                    if (x[i] == x[k])
                        st = 2;
            } while ((i != n - 1) && (st != 2));
        }
        if (st == 0) {
            for (k = 1; k <= n; ++k)
                for (i = n; i >= k; --i)
                    f[i] = ((xx - x[i - k])*f[i] - (xx - x[i])*f[i - 1])/(x[i] - x[i - k]);
            return f[n];
        }
    }
}