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
#define SP500_NUM   505
using namespace std;
class Portfolio;
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
    //float Getadjclose() const{return adjusted_close;}
    //int64_t Getvolume() const{return volume;}
    int InsertATrade(string symbol,string Table,sqlite3 *db){
        char stockDB_insert_table[512];
        //if (date=="")
        //    return 0;
        sprintf(stockDB_insert_table,
                "INSERT INTO %s (symbol, date, open, high, low, close, adjusted_close, volume) VALUES( \"%s\", \"%s\", %f, %f, %f, %f, %f, %lld)",Table.c_str(),symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
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
class Reference{
protected:
    string symbol;
public:
    Reference(string s):symbol(s){}
    Reference(){symbol="";}
    Reference(const Reference& S):symbol(S.symbol){}
    ~Reference(){}
};
class Fundamental:public virtual Reference{
protected:
    float PERatio;
    float DivYield;
    float Beta;
    float High52Weeks;
    float Low52Weeks;
    float MA50Days;
    float MA200Days;
public:
    Fundamental(string symbol):Reference(symbol){}
    Fundamental(string symbol,float PE,float Div,float B,float High,float Low,float MA50,float MA200):
    Reference(symbol),PERatio(PE),DivYield(Div),Beta(B),High52Weeks(High),Low52Weeks(Low),MA50Days(MA50),MA200Days(MA200){}
    Fundamental(const Fundamental& F):Reference(F.symbol),PERatio(F.PERatio),DivYield(F.DivYield),Beta(F.Beta),High52Weeks(F.High52Weeks),Low52Weeks(F.Low52Weeks),MA50Days(F.MA50Days),MA200Days(F.MA200Days){}
    Fundamental(){ PERatio=DivYield=Beta=High52Weeks=Low52Weeks=MA50Days=MA200Days=0;}
    ~Fundamental(){}
    void AddFundamental(float PE,float Div,float B,float High,float Low,float MA50,float MA200){
        PERatio=PE;DivYield=Div;Beta=B;High52Weeks=High;Low52Weeks=Low;MA50Days=MA50;MA200Days=MA200;
    }
    int InsertFundamental(sqlite3* db){
        char stockDB_insert_table[512];
        sprintf(stockDB_insert_table, "INSERT INTO FundamentalData (symbol, PERatio, DividendYield, Beta, High52Weeks, Low52Weeks, MA50Days, MA200Days) VALUES( \"%s\", %f, %f, %f, %f, %f, %f, %f)",  symbol.c_str(), PERatio,DivYield,Beta,High52Weeks,Low52Weeks,MA50Days,MA200Days);
        if (InsertTable(stockDB_insert_table, db) == -1)
            return -1;
        return 0;
    }
    friend ostream& operator<<(ostream &out,const Fundamental& F){
        out<<"P/E Ratio: "<<F.PERatio<< " Dividend Yield: "<<F.DivYield<<" Beta: "<<F.Beta<<" High 52 Weeks: "<<F.High52Weeks<<" Low 52 Weeks: "<<F.Low52Weeks<< " MA 50 days: "<<F.MA50Days<<" MA 200 days: "<<F.MA200Days<<endl;
        return out;
    }


};
class Market:public virtual Reference{
protected:
    //string symbol;
    vector<Trade> trades;
    vector<double> dailyret;
public:
    Market(){vector<Trade>().swap(trades);vector<double>().swap(dailyret);}//wipe out trades
    //Market(string symbol_) :symbol(symbol_){ vector<Trade>().swap(trades);vector<double>().swap(dailyret);}
    Market(string symbol):Reference(symbol){vector<Trade>().swap(trades);vector<double>().swap(dailyret);}
    Market(string symbol_,vector<Trade> trades_)://,vector<double> dailyret_
            Reference(symbol_),trades(trades_) {vector<double>().swap(dailyret);}//,dailyret(dailyret_)
    Market(string symbol_,vector<Trade> trades_,vector<double> dailyret_)://
            Reference(symbol_),trades(trades_),dailyret(dailyret_) {vector<double>().swap(dailyret);}//
    Market(const Market& M):Reference(M.symbol),trades(M.trades),dailyret(M.dailyret) {}
    ~Market() {}
    string GetSymbol()const {return symbol;}
    vector<Trade> GetTrades()const {return trades;}
    //void addRet(vector<double>& Ret){ dailyret=Ret;}
    void addRet(double R){dailyret.push_back(R);}
    void addTrade(Trade aTrade) { trades.push_back(aTrade);}
};
class Stock:public Market,public Fundamental{
    friend class Portfolio;
public:
    Stock(){}
    Stock(string symbol_):Reference(symbol_) {vector<Trade>().swap(trades);vector<double>().swap(dailyret);}
    Stock(string symbol_,vector<Trade> trades_,vector<double> dailyret_,float PE,float Div,float B,float High,float Low,float MA50,float MA200):
    Reference(symbol_),Market(symbol_,trades_,dailyret_),Fundamental(symbol_,PE,Div,B,High,Low,MA50,MA200){}
    Stock(const Stock& S):Reference(S.symbol),
    Market(S.symbol,S.trades,S.dailyret),Fundamental(S.symbol,S.PERatio,S.DivYield,S.Beta,S.High52Weeks,S.Low52Weeks,S.MA50Days,S.MA200Days){}
    ~Stock(){}
    //vector<double> GetRet()const{return dailyret;}
    bool operator==(const Stock& other){
        return (symbol==other.symbol);
    }
};
/*class Stock
{
private:
    string symbol;
    vector<Trade> trades;
    Fundamental Fdata;
    vector<double> dailyret;

public:
    Stock(){symbol="";vector<Trade>().swap(trades);}//wipe out trades
    Stock(string symbol_) :symbol(symbol_)
    {
        vector<Trade>().swap(trades);
        vector<double>().swap(dailyret);
        //vector<double>().swap(rf);
    }
    Stock(string symbol_,vector<Trade> trades_,Fundamental Fdata_,vector<double> dailyret_):
    symbol(symbol_),trades(trades_),Fdata(Fdata_),dailyret(dailyret_) {}
    Stock(const Stock& Stock_):symbol(Stock_.symbol),trades(Stock_.trades),Fdata(Stock_.Fdata),dailyret(Stock_.dailyret) {}
    ~Stock() {}
    string GetSymbol()const {return symbol;}
    vector<Trade> GetTrades()const {return trades;}
    void addTrade(Trade aTrade) { trades.push_back(aTrade);}
    void addTradeVec(vector<Trade>& Trades) { trades=Trades;}
    void addFundamental(Fundamental& F) { Fdata=F;}
    void addRet(vector<double>& Ret){ dailyret=Ret;}
    //void addRf(vector<double>& Rf) { rf=Rf;}
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
};*/


#endif //PORTFOLIOGA_STOCK_H
