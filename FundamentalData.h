//
// Created by Vivian Kang on 6/22/20.
//

#ifndef PORTFOLIOGA_FUNDAMENTALDATA_H
#define PORTFOLIOGA_FUNDAMENTALDATA_H
#include <string>
#include "json/json.h"
#include <sqlite3.h>
using namespace std;
int PopulateFundamentalTable(const Json::Value& root, string symbol, sqlite3* db);
int FundamentalRetrieve(vector<string>& stocklist,sqlite3* stockDB);
#endif //PORTFOLIOGA_FUNDAMENTALDATA_H
