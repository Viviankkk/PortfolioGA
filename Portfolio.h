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
#define MAX_ALLOWABLE_GENERATIONS   100


//returns a float between 0 & 1
#define RANDOM_NUM      ((float)rand()/RAND_MAX)


class Portfolio{//Needs to ensure all constinuents has same length of data
private:
    vector<int> id;//id of consitituents
    vector<string> constituents;//for showing
    //vector<Stock> constinuents;
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
    Portfolio(){fitness=ret=risk=0;}
    Portfolio(int length,vector<Stock> &stocks,int period);
    Portfolio(const Portfolio& P):id(P.id),fitness(P.fitness),ret(P.ret),risk(P.risk),constituents(P.constituents),weights(P.weights) {};
    void AssignWeight(vector<Stock>& stocks);
    void Assignfitness(vector<Stock>& stocks,int period){
        AssignWeight(stocks);
        calret(stocks,period);
        risk=calrisk(stocks);
        fitness=ret/risk;
    }
    double calrisk(vector<Stock>& stocks);
    void calret(vector<Stock>& stocks,int period);
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
    void Mutate(vector<Stock>& stocklist);
    //Portfolio Crossover(Portfolio& AnotherParent);
};
#define PORTFOLIOGA_PORTFOLIO_H

#endif //PORTFOLIOGA_PORTFOLIO_H
