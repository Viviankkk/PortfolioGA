//
// Created by Vivian Kang on 6/22/20.
//

#ifndef PORTFOLIOGA_FUNDAMENTALDATA_H
#define PORTFOLIOGA_FUNDAMENTALDATA_H
#include <string>
#include "json/json.h"
#include <sqlite3.h>
using namespace std;
int PopulateFundamentalTable(const Json::Value& root, string symbol,vector<Fundamental>& FundamentalArray);
int MultiThreadFundamentalRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Fundamental>& F);
int RetrieveFundamentalDataFromDB(Stock& S, sqlite3* db);
#endif //PORTFOLIOGA_FUNDAMENTALDATA_H
