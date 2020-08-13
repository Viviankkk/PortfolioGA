//
// Created by Vivian Kang on 6/24/20.
#include "Portfolio.h"
#include "Population.h"
#include "Stock.h"
#include "Utility.h"
#include "Tests.h"
#include <string>
#include <math.h>
#include <stdlib.h>

using namespace std;
Portfolio::Portfolio(int length,vector<Stock> &stocks,char sign_) {
    vector<int> indexlst; sign=sign_;
    for (int i = 0; i < length; i++) {
        int index = (int) (RANDOM_NUM * SP500_NUM);
        std::vector<int>::iterator it = find(indexlst.begin(), indexlst.end(), index);
        if (it == indexlst.end()) {id.push_back(index);constituents.push_back(stocks[index].GetSymbol());}
    }
    Assignfitness(stocks);
}
Portfolio::Portfolio(vector<int> &id_,char sign_) {
    id=id_;sign=sign_;
    vector<string>().swap(constituents);
    fitness=ret=risk=0;
}

void Portfolio::AssignConstituents(vector<Stock> &stocks) {
    vector<string>().swap(constituents);
    for(auto itr=id.begin();itr!=id.end();itr++) constituents.push_back(stocks[*itr].GetSymbol());
}
void Portfolio::AssignWeight(vector<Stock> &stocks) {
    vector<double> temp;
    for(int i=0;i<id.size();i++){ temp.push_back(stocks[id[i]].Cap);}
    weights=temp/sum(temp);
}

void Portfolio::calret(vector<Stock>& stocks) {
    ret=0;
    for(int i=0;i<id.size();i++){ ret = ret + mean(stocks[id[i]].ret)*weights[i];}
    ret=pow(1+ret,252/PERIOD)-1;
}

double Portfolio::calrisk(vector<Stock>& stocks){
    vector<vector<double>> matrix(id.size());
    int length=stocks[id[0]].ret.size();
    for(int i=0;i<id.size();i++){
        matrix[i].resize(length);
        matrix[i]=stocks[id[i]].ret;
    }
    vector<vector<double>> covmatrix=covariance_matrix(matrix);
    double sum=0;
    for(int i=0;i<id.size();i++){
        sum+=covmatrix[i][i]*pow(weights[i],2);
        for(int j=i+1;j<id.size();j++){
            sum+=2*covmatrix[i][j]*weights[i]*weights[j];
        }
    }
    sum=sqrt(sum);
    //double_t temp2=252/length;
    return sum*sqrt(252/PERIOD);//annualized

}

void Portfolio::CumulativeRet(string st,string ed,sqlite3* stockDB,TestMetrics &TM){
    vector<Stock> stocks;
    for(auto itr=constituents.begin();itr!=constituents.end();itr++){
        Stock mystock(*itr);
        RetrieveMarketDataFromDB(mystock,"MarketData",st,ed,stockDB);
        stocks.push_back(mystock);
    }
    vector<double> portfolioret(stocks[0].close.size(),0);
    for(int i=0;i<stocks.size();i++){
        vector<double> c(stocks[0].close);
        for(int j=0;j<c.size();j++){
            c[j]=stocks[i].close[j]/stocks[i].close[0];
        }
        portfolioret=portfolioret+c*weights[i];
    }
    portfolioret=portfolioret*TM.cret.back();
    Stock SPY("SPY");
    RetrieveMarketDataFromDB(SPY,"SPY",st,ed,stockDB);
    double initial=TM.crefret.back();
    for(int i=1;i<SPY.close.size();i++)
        TM.crefret.push_back(SPY.close[i]/SPY.close[0]*initial);

    for(auto itr=portfolioret.begin()+1;itr!=portfolioret.end();itr++) {
        TM.cret.push_back(*itr);
    }
}

void Portfolio::Mutate(vector<Stock> &stocks)
{
    for (auto itr=id.begin();itr!=id.end();itr++)
    {
        if (RANDOM_NUM < MUTATION_RATE)
        {
            id.erase(itr);
            bool repstock=true;
            while (repstock) {
                int newid= (RANDOM_NUM * SP500_NUM);
                std::vector<int>::iterator it = find(id.begin(), id.end(), newid);
                if(it == id.end()) {
                    id.insert(itr,newid);
                    repstock=false;
                }
            }
        }
    }
    Assignfitness(stocks);
}
void Portfolio::SubRep(){
    int len=id.size();
    sort( id.begin(), id.end() );
    id.erase( unique( id.begin(),id.end() ), id.end() );
    for(int i=id.size();i<len;){
        int index = (int) (RANDOM_NUM * SP500_NUM);
        std::vector<int>::iterator it = find(id.begin(), id.end(), index);
        if (it == id.end()) {id.push_back(index);i++;}
    }
    random_shuffle ( id.begin(), id.end() );
}
void Crossover(Portfolio& Parent,Portfolio& AnotherParent,Portfolio& Child1,Portfolio& Child2)
{

            int crpoint = RANDOM_NUM * PORTFOLIO_SIZE;
            vector<int> a = Parent.GetID();
            vector<int> b = AnotherParent.GetID();
            vector<int> c1(a.begin(), a.begin() + crpoint);
            vector<int> c2(b.begin(), b.begin() + crpoint);
            c1.insert(c1.end(), b.begin() + crpoint, b.end());
            c2.insert(c2.end(), a.begin() + crpoint, a.end());
            Child1.AssignID(c1);
            Child2.AssignID(c2);
            Child1.SubRep();
            Child2.SubRep();
}

void Portfolio::Assignfitness(vector<Stock> &stocks) {
    AssignWeight(stocks);
    switch (sign) {
        case'1':
        {
            calret(stocks);
            risk=calrisk(stocks);
            fitness=ret/risk*7;
        }
        break;
        case '2':
        {
            vector<double> v1;//value indicator, pe-->standardized
            vector<double> p1;//price momentum indicator, current price/high52
            vector<double> q1;//quality indicator, return on assets
            vector<double> e1;//analyst indicator, eps estimate for next quarter/esp actual
            //vector<double> f;
            for(auto itr=id.begin();itr!=id.end();itr++){
                double tempPE=stocks[*itr].PERatio;
                if(tempPE==0) v1.push_back(0);
                else v1.push_back(log(tempPE));
                q1.push_back(stocks[*itr].ROA*10);
                p1.push_back(10/stocks[*itr].close.back()*stocks[*itr].High52Weeks);
                e1.push_back(10*stocks[*itr].EPSEstimate);
            }
            fitness=0;
            for(int i=0;i<id.size();i++){
                fitness+=weights[i]*(v1[i]*p1[i]*q1[i]+v1[i]*v1[i]*e1[i]);
            }
        }
        break;
        case'3':
        {
            calret(stocks);
            risk=calrisk(stocks);
            fitness=ret/risk*7;
            vector<double> v1;//value indicator, pe-->standardized
            vector<double> p1;//price momentum indicator, current price/high52
            vector<double> q1;//quality indicator, return on assets
            vector<double> e1;//analyst indicator, eps estimate for next quarter/esp actual
            for(auto itr=id.begin();itr!=id.end();itr++){
                double tempPE=stocks[*itr].PERatio;
                if(tempPE==0) v1.push_back(0);
                else v1.push_back(log(tempPE));
                q1.push_back(stocks[*itr].ROA*10);
                p1.push_back(10/stocks[*itr].close.back()*stocks[*itr].High52Weeks);
                e1.push_back(10*stocks[*itr].EPSEstimate);//(1+stocks[*itr].EPSEstimate)
            }
            for(int i=0;i<id.size();i++){
                fitness+=weights[i]*(v1[i]*p1[i]*q1[i]+v1[i]*v1[i]*e1[i])/20;
            }
        }
            break;
    }

}


