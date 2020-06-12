//
// Created by Vivian Kang on 6/7/20.
//

#ifndef PORTFOLIOGA_MARKETDATA_H
#define PORTFOLIOGA_MARKETDATA_H
#include <string>
#include "json/json.h"
#include "database.h"
#include <sqlite3.h>

using namespace std;
class Trade
{
private:
    string date;
    float open;
    float high;
    float low;
    float close;
    float adjusted_close;
    int64_t volume;
public:
    Trade(){}
    Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
            date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
    {}
    Trade(const Trade& Trade_):
    date(Trade_.date),open(Trade_.open), high(Trade_.high), low(Trade_.low), close(Trade_.close), adjusted_close(Trade_.adjusted_close), volume(Trade_.volume)
    {}

    ~Trade() {}
    //string Getdate() const{return date;}
    //float Getopen() const{return open;}
    //float Gethigh() const{return high;}
    //float Getlow() const{return low;}
    //float Getclose() const{return close;}
    //float Getadjclose() const{return adjusted_close;}
    //int64_t Getvolume() const{return volume;}
    int InsertATrade(string symbol,sqlite3 *db){
        char stockDB_insert_table[512];
        //if (date=="")
        //    return 0;
        sprintf(stockDB_insert_table,
                "INSERT INTO MarketData (symbol, date, open, high, low, close, adjusted_close, volume) VALUES( \"%s\", \"%s\", %f, %f, %f, %f, %f, %lld)",symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
        if (InsertTable(stockDB_insert_table, db) == -1)
            return -1;
        return 0;
    }
    friend ostream& operator << (ostream& out, const Trade& t)
    {
        out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
        return out;
    }
};

class Stock
{
private:
    string symbol;
    vector<Trade> trades;

public:
    Stock(){symbol="";vector<Trade>().swap(trades);}//wipe out trades
    Stock(string symbol_) :symbol(symbol_)
    {vector<Trade>().swap(trades);}
    Stock(const Stock& Stock_):symbol(Stock_.symbol),trades(Stock_.trades)
    {}
    ~Stock() {}
    string GetSymbol()const {return symbol;}
    vector<Trade> GetTrades()const {return trades;}
    void addTrade(Trade aTrade)
    {
        trades.push_back(aTrade);
    }
    friend ostream& operator << (ostream& out, const Stock& s)
    {
        out << "Symbol: " << s.symbol << endl;
        for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
            out << *itr;
        return out;
    }
};

int RetrieveMarketData(string url_request, Json::Value& root);
int PopulateStockTable(const Json::Value& root, string symbol, vector<Stock>& StockArray, sqlite3* db);
int PopulateSP500Table(const Json::Value& root, sqlite3* db);
int GetSymbols(sqlite3* db,vector<string>& Symbolist);
int MultiThreadRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Stock>& StockArray,sqlite3* stockDB);
#endif //PORTFOLIOGA_MARKETDATA_H
