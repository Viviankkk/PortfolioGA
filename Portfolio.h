//
// Created by Vivian Kang on 6/24/20.
//

#ifndef PORTFOLIOGA_PORTFOLIO_H
#include <vector>
#include "Stock.h"
#include "OperatorOverloading.h"
#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             0.03
#define POP_SIZE                  50           //must be an even number
#define PORTFOLIO_SIZE             10
//#define GENE_LENGTH               4
#define MAX_ALLOWABLE_GENERATIONS   500


//returns a float between 0 & 1
#define RANDOM_NUM      ((float)rand()/RAND_MAX)


class Portfolio{//Needs to ensure all constinuents has same length of data
    friend class Population;
private:
    vector<int> id;//id of consitituents
    vector<string> constituents;//for showing
    vector<double> weights;
    double ret;
    double risk;
    //vector<double> volatiliy;
    //double beta;
    //vector<double> treynor;
    //vector<double> sharpe;
    //vector<double> jensen;
    double fitness;
public:
    Portfolio(){vector<int>().swap(id);vector<string>().swap(constituents);fitness=ret=risk=0;}
    Portfolio(vector<int>& id);
    Portfolio(int length,vector<Stock> &stocks);
    Portfolio(const Portfolio& P):id(P.id),fitness(P.fitness),ret(P.ret),risk(P.risk),constituents(P.constituents),weights(P.weights) {};
    void AssignID(vector<int>& id_){id=id_;}
    void AssignWeight(vector<Stock>& stocks);
    void AssignConstituents(vector<Stock>& stocks);
    void Assignfitness(vector<Stock>& stocks){
        AssignWeight(stocks);
        calret(stocks);
        risk=calrisk(stocks);
        fitness=ret/risk;
    }
    vector<int> GetID() const{return id;}
    vector<string> GetConstituents() const{return constituents;}
    //double GetReturn() const{return ret;}
    vector<double> GetWeight() const{return weights;}
    double calrisk(vector<Stock>& stocks);
    void calret(vector<Stock>& stocks);
    friend ostream& operator<<(ostream &out,const Portfolio& F){
        out<<"constituents:"<<endl;
        for(auto itr=F.constituents.begin();itr!=F.constituents.end();itr++){
            out<<*itr<<"  ";
        }
        out<<endl;
        //out<<"ret:"<<F.ret<<endl;
        out<<"fitness:"<<F.fitness<<endl;
        return out;
    }
    //void calbeta();
    void Mutate(vector<Stock> &stocks);
    //Portfolio Crossover(Portfolio& AnotherParent);
    bool operator < (const Portfolio& T) const
    {
        return (fitness < T.fitness);
    }
};
void Crossover(Portfolio& Parent,Portfolio& AnotherParent,Portfolio& Child1,Portfolio& Child2);
#define PORTFOLIOGA_PORTFOLIO_H

#endif //PORTFOLIOGA_PORTFOLIO_H
