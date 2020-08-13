//
// Created by Vivian Kang on 8/11/20.
//

#ifndef PORTFOLIOGA_TESTS_H
#define PORTFOLIOGA_TESTS_H
#include <vector>
#include <string>
#include <sqlite3.h>
using namespace std;
struct TestMetrics{
    vector<double> cret;
    vector<double> crefret;
    string date_st;
    string date_ed;
    double annualizedRef;
    double annualizedPnL;
    double annualizedvol;
    double SR;
    double MD;
};
std::ostream& operator<<(std::ostream&, const TestMetrics&);
int  Backtest(TestMetrics& BK,sqlite3* stockDB);
int BuyandHold (TestMetrics& BK,string startdate,string enddate,sqlite3* stockDB,char sign);
int MonthlyRebalancing (TestMetrics& BK,string startdate,string enddate,sqlite3* stockDB,char sign);
void AVG(TestMetrics& MEAN,TestMetrics& BK,int i);
void StatTest(ostream& myfile,char sign,sqlite3* stockDB,string startdate,string enddate,string bkst,string bked,int times,bool );
#endif //PORTFOLIOGA_TESTS_H
