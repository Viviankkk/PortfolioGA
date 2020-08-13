//
// Created by Vivian Kang on 8/7/20.
//

#ifndef PORTFOLIOGA_GNUPLOT_H
#define PORTFOLIOGA_GNUPLOT_H
#include <string>
#include <vector>
using namespace std;
void plotResults(vector<double>& yData, int dataSize,const string name);
void plotCumulative(vector<double>& ptf,vector<double>& ref);
#endif //PORTFOLIOGA_GNUPLOT_H
