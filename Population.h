//
// Created by Vivian Kang on 7/7/20.
//

#ifndef PORTFOLIOGA_POPULATION_H
#define PORTFOLIOGA_POPULATION_H

class Portforlio;


class Population{
private:
    vector<Portfolio> portfolios;
    double total_fitness;
public:
    void Sort();
    Portfolio Roulette();
    void CalFitness(){
        total_fitness=0;
        for(auto itr=portfolios.begin();itr!=portfolios.end();itr++){
            total_fitness+=itr->fitness;
        }
    }
    void AddPortfolio(Portfolio& p){portfolios.push_back(p);}
    void NextGeneration(vector<Portfolio>& offsprings){
        Sort();
        portfolios.erase(portfolios.begin(),portfolios.begin()+offsprings.size());
        portfolios.insert(portfolios.end(),offsprings.begin(),offsprings.end());
    }
    Portfolio GetBest(){
        Sort();
        auto ptr=portfolios.end();
        return *(ptr-1);
    }
    /*void CopyPortfolio(vector<Portfolio> elitists){
        portfolios.insert(portfolios.end(),elitists.begin(),elitists.end());
    }
    vector<Portfolio> GetElitists(int num){
        Sort();
        return vector<Portfolio>(portfolios.begin(),portfolios.begin()+num+1);
    }*/

};
Portfolio GeneticAlgorithm(vector<Stock>& stocks);
double CalPnL(Portfolio& P,string st,string ed,sqlite3* stockDB);
#endif //PORTFOLIOGA_POPULATION_H
