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
#define CHROMO_LENGTH             10
//#define GENE_LENGTH               4
#define MAX_ALLOWABLE_GENERATIONS   100


//returns a float between 0 & 1
#define RANDOM_NUM      ((float)rand()/RAND_MAX)


class Portfolio{//Needs to ensure all constinuents has same length of data
private:
    vector<Stock> constinuents;
    vector<double> weights;
    vector<double> ret;
    vector<double> volatiliy;
    double beta;
    vector<double> treynor;
    vector<double> sharpe;
    vector<double> jensen;
    double fitness;
public:
    Portfolio(int length,vector<Stock>& stocklist);
    Portfolio(const Portfolio& P):constinuents(P.constinuents),fitness(P.fitness) {};
    void Assignfitness(){
        fitness=0.0;//need to amend
    }
    void calvol(int len);
    void calret();
    void calbeta();
    void Mutate(vector<Stock>& stocklist);
    Portfolio Crossover(Portfolio& AnotherParent);
};
#define PORTFOLIOGA_PORTFOLIO_H

#endif //PORTFOLIOGA_PORTFOLIO_H
