//
// Created by Vivian Kang on 6/25/20.
//

#ifndef PORTFOLIOGA_OPERATOROVERLOADING_H
#define PORTFOLIOGA_OPERATOROVERLOADING_H
#include <vector>
#include <iostream>
#include "sqlite3.h"
using namespace std;

vector<double> operator+ (const vector<double>& a, const vector<double>&  b);

//vector<vector<double>> operator+ (const vector<vector<double>>&  a, const vector<vector<double>>&  b);

vector<double> operator- (const vector<double>&  a,const vector<double>&  b);

//vector<vector<double>> operator- (const vector<vector<double>>&  a, vector<vector<double>>&  b);

//double operator* (const vector<double>&  a, const vector<double>&  b);

vector<double> operator* (const vector<double>&  a, const double& b);
vector<double> operator/ (const vector<double>&  a, const double& b);
vector<double> operator* (vector<vector<double>>&  a, vector<double>&  b);
double mean(vector<double>& vec);
double sum(vector<double>& vec);

vector<vector<double>> covariance_matrix(vector<vector<double>>& matrix);
string DateAhead(string date,int n);
int DateDifference(string st,string ed);
void GetMaxMin(double& max,double& min,string columnname,sqlite3* db);
double Normalize(double max,double min,double value);

#endif //PORTFOLIOGA_OPERATOROVERLOADING_H
