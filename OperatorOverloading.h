//
// Created by Vivian Kang on 6/25/20.
//

#ifndef PORTFOLIOGA_OPERATOROVERLOADING_H
#define PORTFOLIOGA_OPERATOROVERLOADING_H
#include <vector>
#include <iostream>
using namespace std;

vector<double> operator+ (const vector<double>& a, const vector<double>&  b);

//vector<vector<double>> operator+ (const vector<vector<double>>&  a, const vector<vector<double>>&  b);

vector<double> operator- (const vector<double>&  a,const vector<double>&  b);

//vector<vector<double>> operator- (const vector<vector<double>>&  a, vector<vector<double>>&  b);

//double operator* (const vector<double>&  a, const vector<double>&  b);

vector<double> operator* (const vector<double>&  a, const double& b);



#endif //PORTFOLIOGA_OPERATOROVERLOADING_H
