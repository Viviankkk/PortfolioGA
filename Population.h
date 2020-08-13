//
// Created by Vivian Kang on 7/7/20.
//

#ifndef PORTFOLIOGA_POPULATION_H
#define PORTFOLIOGA_POPULATION_H

//class Portforlio;

class Population{
private:
    vector<Portfolio> portfolios;
    double total_fitness;
public:
    void Sort();
    void Shuffle() {
        random_shuffle ( portfolios.begin(), portfolios.end() );
    }
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
    double GetBestFitness(){
        Sort();
        return portfolios.back().fitness;
    }

};
Portfolio GeneticAlgorithm(vector<Stock>& stocks,char sign);
Portfolio GeneticAlgorithm(vector<Stock>& stocks,vector<double>& Maxfitness,char sign);

#endif //PORTFOLIOGA_POPULATION_H
