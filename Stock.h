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
#define SP500_NUM   497
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
    double ret;
    int64_t volume;
public:
    Trade(){}
    Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_,double ret_) :
            date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_),ret(ret_)
    {}
    Trade(const Trade& Trade_):
            date(Trade_.date),open(Trade_.open), high(Trade_.high), low(Trade_.low), close(Trade_.close), adjusted_close(Trade_.adjusted_close), volume(Trade_.volume),ret(Trade_.ret)
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
                "INSERT INTO %s (symbol, date, open, high, low, close, adjusted_close, volume, return) VALUES( \"%s\", \"%s\", %f, %f, %f, %f, %f, %lld,%f)",Table.c_str(),symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume,ret);
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
    int64_t Cap;
public:
    Fundamental(string symbol):Reference(symbol){}
    Fundamental(string symbol,float PE,float Div,float B,float High,float Low,float MA50,float MA200,int64_t cap):
    Reference(symbol),PERatio(PE),DivYield(Div),Beta(B),High52Weeks(High),Low52Weeks(Low),MA50Days(MA50),MA200Days(MA200),Cap(cap){}
    Fundamental(const Fundamental& F):Reference(F.symbol),PERatio(F.PERatio),DivYield(F.DivYield),Beta(F.Beta),High52Weeks(F.High52Weeks),Low52Weeks(F.Low52Weeks),MA50Days(F.MA50Days),MA200Days(F.MA200Days),Cap(F.Cap){}
    Fundamental(){ PERatio=DivYield=Beta=High52Weeks=Low52Weeks=MA50Days=MA200Days=0;Cap=0;}
    ~Fundamental(){}
    void AddFundamental(float PE,float Div,float B,float High,float Low,float MA50,float MA200,int64_t cap){
        PERatio=PE;DivYield=Div;Beta=B;High52Weeks=High;Low52Weeks=Low;MA50Days=MA50;MA200Days=MA200;Cap=cap;
    }
    int InsertFundamental(sqlite3* db){
        char stockDB_insert_table[512];
        sprintf(stockDB_insert_table, "INSERT INTO FundamentalData (symbol, PERatio, DividendYield, Beta, High52Weeks, Low52Weeks, MA50Days, MA200Days,Capital) VALUES( \"%s\", %f, %f, %f, %f, %f, %f, %f,%lld)",  symbol.c_str(), PERatio,DivYield,Beta,High52Weeks,Low52Weeks,MA50Days,MA200Days,Cap);
        if (InsertTable(stockDB_insert_table, db) == -1)
            return -1;
        return 0;
    }
    friend ostream& operator<<(ostream &out,const Fundamental& F){
        out<<"P/E Ratio: "<<F.PERatio<< " Dividend Yield: "<<F.DivYield<<" Beta: "<<F.Beta<<" High 52 Weeks: "<<F.High52Weeks<<" Low 52 Weeks: "<<F.Low52Weeks<< " MA 50 days: "<<F.MA50Days<<" MA 200 days: "<<F.MA200Days<<" Capital: "<<F.Cap<<endl;
        return out;
    }


};
class Market:public virtual Reference{
protected:
    //string symbol;
    vector<Trade> trades;
    vector<double> ret;
    vector<double> close;
    vector<string> dates;
public:
    Market(){vector<Trade>().swap(trades);vector<double>().swap(ret);vector<double>().swap(close);vector<string>().swap(dates);}//wipe out trades
    Market(string symbol):Reference(symbol){vector<Trade>().swap(trades);vector<double>().swap(ret);vector<double>().swap(close);vector<string>().swap(dates);}
    Market(string symbol_,vector<Trade> trades_)://,vector<double> dailyret_
            Reference(symbol_),trades(trades_) {vector<double>().swap(ret);vector<double>().swap(close);vector<string>().swap(dates);}//,dailyret(dailyret_)
    Market(string symbol_,vector<Trade> trades_,vector<double> ret_,vector<double> close_,vector<string> date_)://
            Reference(symbol_),trades(trades_),ret(ret_),close(close_),dates(date_) {}//
    Market(string symbol_,vector<Trade> trades_,vector<double> ret_)://
            Reference(symbol_),trades(trades_),ret(ret_) {}//
    Market(const Market& M):Reference(M.symbol),trades(M.trades),ret(M.ret),close(M.close),dates(M.dates) {}
    ~Market() {}
    string GetSymbol()const {return symbol;}
    vector<string> GetDates()const {return dates;}
    vector<Trade> GetTrades()const {return trades;}
    //void addRet(vector<double>& Ret){ dailyret=Ret;}
    void addRet(double R){ret.push_back(R);}
    void addTrade(Trade aTrade) { trades.push_back(aTrade);}
    void addRetVec(vector<double>& ret_) {ret=ret_;}
    void addClose(double adjclose){close.push_back(adjclose);}
    void adddates(string date){dates.push_back(date);}
    void CalRet(int period){
        //int prev=close[0];
        ret.resize(close.size()-period);
        for(int i=period;i<close.size();i++){
          ret[i-period]=close[i]/close[i-period]-1;
        }
    }
};
class Stock:public Market,public Fundamental{
    friend class Portfolio;
public:
    Stock(){}
    Stock(string symbol_):Reference(symbol_) {vector<Trade>().swap(trades);vector<double>().swap(ret);}
    Stock(string symbol_,vector<Trade> trades_,vector<double> ret_,float PE,float Div,float B,float High,float Low,float MA50,float MA200,int64_t cap):
    Reference(symbol_),Market(symbol_,trades_,ret_),Fundamental(symbol_,PE,Div,B,High,Low,MA50,MA200,cap){}
    Stock(const Stock& S):Reference(S.symbol),
    Market(S.symbol,S.trades,S.ret,S.close,S.dates),Fundamental(S.symbol,S.PERatio,S.DivYield,S.Beta,S.High52Weeks,S.Low52Weeks,S.MA50Days,S.MA200Days,S.Cap){}
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
