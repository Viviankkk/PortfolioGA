//
// Created by Vivian Kang on 8/11/20.
//

#include <iostream>
#include <sqlite3.h>
#include "marketdata.h"
#include "Stock.h"
#include "FundamentalData.h"
#include "Portfolio.h"
#include "Population.h"
#include "Tests.h"
#include <vector>
#include <string>
#include <cmath>
using namespace  std;

int  Backtest(TestMetrics& BK,sqlite3* stockDB){

    //Retrieve Reference data
    Stock Rf("^TNX");
    if(RetrieveMarketDataFromDB(Rf,"TNX",BK.date_st,BK.date_ed,stockDB)==-1) return -1;
    //Calculate PnL
    double len=BK.cret.size();
    BK.annualizedRef=pow(BK.crefret.back()/BK.crefret.front(),252/len)-1;
    BK.annualizedPnL=pow(BK.cret.back()/BK.cret.front(),252/len)-1;
    //Calculate Volatility
    vector<double> ret(len-1);
    for(int i=0;i<len-1;i++) ret[i]=BK.cret[i+1]/BK.cret[i]-1;
    double avg=mean(ret);
    double sum=0;
    for(int i=0;i<len-1;i++) sum+=(ret[i]-avg)*(ret[i]-avg);
    BK.annualizedvol=sqrt(sum/ret.size()*252);
    //Calculate Sharpe Ratio
    BK.SR=(BK.annualizedPnL-Rf.GetClose().back()/100)/BK.annualizedvol;
    //Calculate Max Drawdown
    vector<double>::iterator peakpos=max_element(BK.cret.begin(), BK.cret.end());
    double trough=*min_element(peakpos,BK.cret.end());
    double peak=*peakpos;
    BK.MD=(peak-trough)/peak;
    return 0;
}

std::ostream& operator<<(std::ostream& out, const TestMetrics& TM){
    out<<"Testing Period: "<<TM.date_st<<" to "<<TM.date_ed<<endl;
    out<<"Annualized PnL:" <<TM.annualizedPnL*100<<"%"<<endl
    <<"Period PnL: "<<(TM.cret.back()-1)*100<<"%"<<endl
    <<"Annualized Volatility: "<<TM.annualizedvol<<endl
    <<"Sharpe Ratio: "<<TM.SR<<endl
    <<"Max Drawdown: "<<TM.MD<<endl
    <<"Market Performance(Annualized): "<<TM.annualizedRef*100<<"%"<<endl
    <<"Market Performance(Period): "<<(TM.crefret.back()-1)*100<<"%"<<endl;
    return out;
}
int BuyandHold (TestMetrics& BK,string startdate,string enddate,sqlite3* stockDB,char sign){

    Stock SPY("SPY");
    if (RetrieveMarketDataFromDB(SPY, "SPY", startdate, enddate, stockDB) == -1) return -1;
    int length = SPY.GetDates().size();

    //Get List for Stocks pool
    vector<string> stocklist;
    if (GetSymbols(stockDB, stocklist) == -1) return -1;

    //Buy and Hold Trategy
    string backtest_st = BK.date_st;
    string backtest_ed = BK.date_ed;
    BK.crefret.push_back(1);
    BK.cret.push_back(1);

    vector<Stock> stocks;
    for (auto itr = stocklist.begin(); itr != stocklist.end(); itr++) {
        Stock mystock(*itr);
        if (RetrieveMarketDataFromDB(mystock, "MarketData", startdate, enddate, stockDB) == -1)
            return -1;
        if (RetrieveFundamentalDataFromDB(mystock, stockDB) == -1) return -1;
        if (mystock.GetDates().size() != length) { continue; }
        mystock.CalRet(PERIOD);
        stocks.push_back(mystock);
        //stockmap[*itr]=mystock;
    }
    Portfolio Hold = GeneticAlgorithm(stocks, sign);
    Hold.CumulativeRet(backtest_st, backtest_ed, stockDB, BK);
    Backtest(BK, stockDB);
    cout << Hold << endl;
    cout << BK <<endl;
    return 0;
}
int MonthlyRebalancing (TestMetrics& BK,string startdate,string enddate,sqlite3* stockDB,char sign){
    //Get List for Stocks pool
    vector<string> stocklist;
    if (GetSymbols(stockDB, stocklist) == -1) return -1;

    //Rebalancing
    string backtest_st = BK.date_st;
    string backtest_ed;
    BK.crefret.push_back(1);
    BK.cret.push_back(1);
    for(int i=0;i<6;i++) {
        backtest_ed=DateAhead(backtest_st,PERIOD/5*7);
        //Get SPY
        Stock SPY("SPY");
        if (RetrieveMarketDataFromDB(SPY, "SPY", startdate, enddate, stockDB) == -1) return -1;
        int length = SPY.GetDates().size();
        //Get consituents from database
        vector<Stock> stocks;
        for (auto itr = stocklist.begin(); itr != stocklist.end(); itr++) {
            Stock mystock(*itr);
            if (RetrieveMarketDataFromDB(mystock, "MarketData", startdate, enddate, stockDB) == -1)
                return -1;
            if (RetrieveFundamentalDataFromDB(mystock, stockDB) == -1) return -1;
            if (mystock.GetDates().size() != length) { continue; }
            mystock.CalRet(PERIOD);
            stocks.push_back(mystock);
        }
        Portfolio Hold = GeneticAlgorithm(stocks,sign);
        Hold.CumulativeRet(backtest_st,backtest_ed,stockDB,BK);
        cout << Hold<<endl;
        startdate = DateAhead(startdate, PERIOD/5*7 );
        enddate = DateAhead(enddate, PERIOD/5*7 );
        backtest_st = backtest_ed;
    }
    Backtest(BK, stockDB);
    cout << BK <<endl;
    return 0;
}

void AVG(TestMetrics& MEAN,TestMetrics& BK,int i){
    MEAN.annualizedPnL=(MEAN.annualizedPnL*i+BK.annualizedPnL)/(i+1);
    MEAN.annualizedvol=(MEAN.annualizedvol*i+BK.annualizedvol)/(i+1);
    MEAN.SR=(MEAN.SR*i+BK.SR)/(i+1);
    MEAN.MD=(MEAN.MD*i+BK.MD)/(i+1);
}

void StatTest(ostream& myfile,char sign,sqlite3* stockDB,string startdate,string enddate,string bkst,string bked ,int times,bool Backtest){
    myfile<<"Stats for Fitness Function "<<sign<<": "<<endl;
    TestMetrics MaxPnL;
    MaxPnL.annualizedPnL = 0;
    TestMetrics MinPnL;
    MinPnL.annualizedPnL = 10;
    TestMetrics MinVol;
    MinVol.annualizedvol = 10;
    TestMetrics MaxVol;
    MaxVol.annualizedvol = 0;
    TestMetrics MaxSR;
    MaxSR.SR = 0;
    TestMetrics MaxMD;
    MaxMD.MD = 0;
    TestMetrics Mean;
    Mean.annualizedvol = Mean.annualizedPnL = Mean.SR = Mean.MD = 0;
    for (int i = 0; i < times; i++) {
        TestMetrics BK;
        BK.date_st = bkst;
        BK.date_ed = bked;
        if(Backtest){
        if (sign=='2') BuyandHold(BK, startdate, enddate, stockDB, '2');
        else MonthlyRebalancing(BK,startdate,enddate,stockDB,sign);}
        else BuyandHold(BK, startdate, enddate, stockDB, sign);
        if (BK.annualizedPnL > MaxPnL.annualizedPnL) MaxPnL = BK;
        if (BK.annualizedPnL < MinPnL.annualizedPnL) MinPnL = BK;
        if (BK.annualizedvol < MinVol.annualizedvol) MinVol = BK;
        if (BK.annualizedvol > MaxVol.annualizedvol) MaxVol = BK;
        if (BK.SR > MaxSR.SR) MaxSR = BK;
        if (BK.MD > MaxMD.MD) MaxMD = BK;
        AVG(Mean, BK, i);
        //Mean.annualizedRef=BK.annualizedRef;
    }
    myfile<<"\tMax\t\tMin"<<endl;
    myfile<<"PnL: "<<MaxPnL.annualizedPnL<<"\t"<<MinPnL.annualizedPnL<<endl;
    myfile<<"Vol: "<<MaxVol.annualizedvol<<"\t"<<MinVol.annualizedvol<<endl;
    myfile<<"SR:  "<<MaxSR.SR<<endl;
    myfile<<"MDD: "<<MaxMD.MD<<endl<<endl;
    myfile<<"Mean Performance:"<<endl;
    myfile<<"Annualized PnL:" <<Mean.annualizedPnL*100<<"%"<<endl
          <<"Annualized Volatility: "<<Mean.annualizedvol<<endl
          <<"Sharpe Ratio: "<<Mean.SR<<endl
          <<"Max Drawdown: "<<Mean.MD<<endl<<endl<<endl;
}