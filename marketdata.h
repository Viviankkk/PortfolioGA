//
// Created by Vivian Kang on 6/7/20.
//

#ifndef PORTFOLIOGA_MARKETDATA_H
#define PORTFOLIOGA_MARKETDATA_H
#include <string>
#include "json/json.h"
#include "database.h"
#include "Stock.h"
#include <sqlite3.h>

using namespace std;

int RetrieveMarketData(string url_request, Json::Value& root);
int PopulateStockTable(const Json::Value& root, string symbol, vector<Stock>& StockArray, sqlite3* db);
int PopulateSP500Table(const Json::Value& root, sqlite3* db);
int GetSymbols(sqlite3* db,vector<string>& Symbolist);
int MultiThreadRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Stock>& StockArray,sqlite3* stockDB);
int GetReturn(Stock& S,sqlite3* db);
#endif //PORTFOLIOGA_MARKETDATA_H
