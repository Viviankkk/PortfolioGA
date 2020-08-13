//
// Created by Vivian Kang on 6/24/20.
//

#ifndef PORTFOLIOGA_PORTFOLIO_H
#include <vector>
#include "Stock.h"
#include "Utility.h"
#define CROSSOVER_RATE            0.8
#define MUTATION_RATE             0.02
#define POP_SIZE                  50           //must be an even number
#define PORTFOLIO_SIZE             10
#define MAX_ALLOWABLE_GENERATIONS   700


//returns a float between 0 & 1
#define RANDOM_NUM      ((float)rand()/RAND_MAX)

struct TestMetrics;
class Portfolio{//Needs to ensure all constinuents has same length of data
    friend class Population;
private:
    vector<int> id;//id of consitituents
    vector<string> constituents;//for showing
    vector<double> weights;
    double ret;
    double risk;
    double fitness;
    char sign;
public:
    Portfolio(char sign_){vector<int>().swap(id);vector<string>().swap(constituents);fitness=ret=risk=0;sign=sign_;}
    Portfolio(vector<int>& id,char sign_);
    Portfolio(int length,vector<Stock> &stocks,char sign_);
    Portfolio(const Portfolio& P):id(P.id),fitness(P.fitness),ret(P.ret),risk(P.risk),constituents(P.constituents),weights(P.weights),sign(P.sign) {};
    void AssignID(vector<int>& id_){id=id_;}
    void AssignWeight(vector<Stock>& stocks);
    void AssignWeight(vector<double> weights_);
    void AssignConstituents(vector<Stock>& stocks);
    void Assignfitness(vector<Stock>& stocks);
    vector<int> GetID() const{return id;}
    vector<string> GetConstituents() const{return constituents;}
    vector<double> GetWeight() const{return weights;}
    double GetFitness() const{return fitness;}
    int Backtest(string st,string ed,sqlite3* stockDB,double&PnL,double&vol,double&SR,double&MD);
    double calrisk(vector<Stock>& stocks);
    void calret(vector<Stock>& stocks);
    void SubRep();
    friend ostream& operator<<(ostream &out,const Portfolio& F){
        out<<"constituents:"<<endl;
        for(auto itr=F.constituents.begin();itr!=F.constituents.end();itr++){
            out<<*itr<<"  ";
        }
        out<<endl;
        out<<"fitness:"<<F.fitness<<endl;
        return out;
    }

    void CumulativeRet(string st,string ed,sqlite3* stockDB,TestMetrics &TM);
    void Mutate(vector<Stock> &stocks);
    bool operator < (const Portfolio& T) const
    {
        return (fitness < T.fitness);
    }
};
void Crossover(Portfolio& Parent,Portfolio& AnotherParent,Portfolio& Child1,Portfolio& Child2);
#define PORTFOLIOGA_PORTFOLIO_H

#endif //PORTFOLIOGA_PORTFOLIO_H
