//
// Created by Vivian Kang on 6/25/20.
//

#include "OperatorOverloading.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

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
vector<double> operator/ (const vector<double>&  a, const double& b) {
    vector<double> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]/b;
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
double sum(vector<double>& vec){
    double sum=0;
    for(int i=0;i<vec.size();i++){
        sum+=vec[i];
    }
    return sum;
}
double mean(vector<double>& vec){
    return sum(vec)/vec.size();
}
double cov(vector<double>& x,vector<double>& y){
    double xmean=mean(x);
    double ymean=mean(y);
    if(x.size()!=y.size()) return -999;
    double total = 0;
    for(int i = 0; i < x.size(); i++)
    {
        total += (x[i] - xmean) * (y[i] - ymean);
    }
    return total / x.size();
}
vector<vector<double>> covariance_matrix(vector<vector<double>>& matrix){
    vector<vector<double>> covmatrix(matrix.size());
    for(int i=0;i<matrix.size();i++){
        covmatrix[i].resize(matrix.size());
        for(int j=0;j<matrix.size();j++){

            if(j>=i){
                covmatrix[i][j]=cov(matrix[i],matrix[j]);
            }
            else covmatrix[i][j]=covmatrix[j][i];
        }
    }
    return covmatrix;
}

string DateAhead(string date,int n){
    tm Anctm = {};
    stringstream ss(date);
    ss >> get_time(&Anctm, "%Y-%m-%d");

    time_t t=mktime(&Anctm)+n*24*60*60;
    tm* af=localtime(&t);
    //char time[100];
    //sprintf(time,"%lld",af);
    string month,day;
    int m=af->tm_mon+1;
    if (m<10) month="0"+to_string(m);
    else month=to_string(m);
    if (af->tm_mday<10) day="0"+to_string(af->tm_mday);
    else day=to_string(af->tm_mday);
    return to_string(af->tm_year+1900)+"-"+month+"-"+day;//asctime(af);
}