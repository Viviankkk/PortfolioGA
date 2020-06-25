//
// Created by Vivian Kang on 6/25/20.
//

#include "OperatorOverloading.h"
#include <vector>
#include <iostream>
using namespace std;

vector<double> operator+ (const vector<double>& a, const vector<double>&  b) {
    vector<double> c(a.size(),0);
    if (a.size()!=b.size()) {
        cout<<"Vector sizes do not match!"<<endl;
        return c;
    }
    for(int i=0;i<a.size();i++){
        c[i]=a[i]+b[i];
    }
    return c;
}
/*template <class T>
vector<T> operator+ (const vector<T>& a, const vector<T>&  b) {
    if (a.size()!=b.size()) {
        cout<<"Vector sizes do not match!"<<endl;
        return NULL;
    }
    vector<T> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]+b[i];
    }
    return c;
}*/

/*template <class T>
vector<vector<T>> operator+ (vector<vector<T>>&  a, vector<vector<T>>&  b) {
    if (a.size()!=b.size() || a[0].size()!=b[0].size()) {
        cout<<"Matrix sizes do not match!"<<endl;
        return NULL;
    }
    vector<vector<T>> c(a.size(),(a[0].size()));
    for(int i=0;i<a.size();i++){
        for(int j=0;j<a[0].size();j++){
            c[i][j]=a[i][j]+b[i][j];
        }
    }
    return c;
}*/

template <class T>
vector<T> operator- (const vector<T>&  a, const vector<T>&  b) {
    if (a.size()!=b.size()) {
        cout<<"Vector sizes do not match!"<<endl;
        return NULL;
    }
    vector<T> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]-b[i];
    }
    return c;
}

/*template <class T>
vector<vector<T>> operator- (vector<vector<T>>&  a, vector<vector<T>>&  b) {
    if (a.size()!=b.size() || a[0].size()!=b[0].size()) {
        cout<<"Matrix sizes do not match!"<<endl;
        return NULL;
    }
    vector<vector<T>> c(a.size(),(a[0].size()));
    for(int i=0;i<a.size();i++){
        for(int j=0;j<a[0].size();j++){
            c[i][j]=a[i][j]-b[i][j];
        }
    }
    return c;
}*/

/*template <class T>
T operator* (vector<T>&  a, vector<T>&  b) {
    if (a.size()!=b.size()) {
        cout<<"Vector sizes do not match!"<<endl;
        return NULL;
    }
    T sum;
    for(int i=0;i<a.size();i++){
        sum+=a[i]*b[i];
    }
    return sum;
}*/


vector<double> operator* (const vector<double>&  a, const double& b) {
    vector<double> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]*b;
    }
    return c;
}
/*template <class T>
vector<vector<T>> operator* (vector<vector<T>>&  a, vector<vector<T>>&  b) {
    if (a[0].size()!=b.size()) {
        cout<<"Matrix sizes do not match!"<<endl;
        return NULL;
    }
    vector<vector<T>> c(a.size(),(b[0].size()));
    for(int i=0;i<a.size();i++){
        a[i]*
    }
    return c;
}*/