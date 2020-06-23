//
// Created by Vivian Kang on 6/17/20.
//

#ifndef PORTFOLIOGA_STOCK_H
#define PORTFOLIOGA_STOCK_H
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "sqlite3.h"
#include "database.h"
#include "marketdata.h"
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
    string Getdate() const{return date;}
    //float Getopen() const{return open;}
    //float Gethigh() const{return high;}
    //float Getlow() const{return low;}
    //float Getclose() const{return close;}
    float Getadjclose() const{return adjusted_close;}
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
    bool operator < (const Trade& T) const
    {
        return (date < T.date);
    }//for sort
    friend ostream& operator << (ostream& out, const Trade& t)
    {
        out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
        return out;
    }
};

class Fundamental{
private:
    float PERatio;
    float DivYield;
    float Beta;
    float High52Weeks;
    float Low52Weeks;
    float MA50Days;
    float MA200Days;
public:
    Fundamental(float PE,float Div,float B,float High,float Low,float MA50,float MA200):
    PERatio(PE),DivYield(Div),Beta(B),High52Weeks(High),Low52Weeks(Low),MA50Days(MA50),MA200Days(MA200){}
    Fundamental(const Fundamental& F):PERatio(F.PERatio),DivYield(F.DivYield),Beta(F.Beta),High52Weeks(F.High52Weeks),Low52Weeks(F.Low52Weeks),MA200Days(F.MA200Days){}
    Fundamental(){ PERatio=DivYield=Beta=High52Weeks=Low52Weeks=MA50Days=MA200Days=0;}
    ~Fundamental(){}
    friend ostream& operator<<(ostream &out,const Fundamental& F){
        out<<"P/E Ratio: "<<F.PERatio<< " Dividend Yield: "<<F.DivYield<<" Beta: "<<F.Beta<<" High 52 Weeks: "<<F.High52Weeks<<" Low 52 Weeks: "<<F.Low52Weeks<< " MA 50 days: "<<F.MA50Days<<" MA 200 days: "<<F.MA200Days<<endl;
        return out;
    }


};
//bool cmp (Trade a,Trade b) { return (a.Getdate()<b.Getdate()); }
class Stock
{
private:
    string symbol;
    vector<Trade> trades;
    Fundamental Fdata;
    vector<double> dailyret;
    vector<double> rf;

public:
    Stock(){symbol="";vector<Trade>().swap(trades);}//wipe out trades
    Stock(string symbol_) :symbol(symbol_)
    {
        vector<Trade>().swap(trades);
        vector<double>().swap(dailyret);
        vector<double>().swap(rf);
    }
    Stock(string symbol_,vector<Trade> trades_,Fundamental Fdata_,vector<double> dailyret_,vector<double> rf_):
    symbol(symbol_),trades(trades_),Fdata(Fdata_),dailyret(dailyret_),rf(rf_) {}
    Stock(const Stock& Stock_):symbol(Stock_.symbol),trades(Stock_.trades),Fdata(Stock_.Fdata),dailyret(Stock_.dailyret),rf(Stock_.rf) {}
    ~Stock() {}
    string GetSymbol()const {return symbol;}
    vector<Trade> GetTrades()const {return trades;}
    void addTrade(Trade aTrade) { trades.push_back(aTrade);}
    void addTradeVec(vector<Trade>& Trades) { trades=Trades;}
    void addFundamental(Fundamental& F) { Fdata=F;}
    void addRet(vector<double>& Ret){ dailyret=Ret;}
    void addRf(vector<double>& Rf) { rf=Rf;}
    friend ostream& operator << (ostream& out, const Stock& s)
    {
        out << "Symbol: " << s.symbol << endl;
        for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
            out << *itr;
        out<<s.Fdata;
        for (auto itr=s.dailyret.begin();itr!= s.dailyret.end();itr++)
            out<<*itr<<"  ";
        out<<endl;
        return out;
    }
};


#endif //PORTFOLIOGA_STOCK_H
