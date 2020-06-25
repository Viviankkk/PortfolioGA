//
// Created by Vivian Kang on 6/24/20.
#include "Portfolio.h"
#include "Stock.h"
#include "OperatorOverloading.h"
#include <string>
#include <iostream>

using namespace std;
Portfolio::Portfolio(int length,vector<Stock>& StockArray){
    vector<int> indexlst;
    for(int i=0;i<length;i++){
        int index=(int)(RANDOM_NUM*SP500_NUM);
        std::vector<int>::iterator it=find(indexlst.begin(),indexlst.end(),index);
        if (it == indexlst.end()) constinuents.push_back(StockArray[index]);
    }
    Assignfitness();
    //fitness=0.0;
}
/*void Portfolio::calvol(int len) {
    ret.resize(constinuents[0].dailyret.size()-len);
}*/
void Portfolio::calret() {
    ret.resize(constinuents[0].dailyret.size());
    for(int i=0;i<constinuents.size();i++){ ret = ret + constinuents[i].dailyret*weights[i];}
}
void Portfolio::calbeta() {
    for(int i=0;i<constinuents.size();i++){
        beta += constinuents[i].Beta*weights[i];
    }
}

void Portfolio::Mutate(vector<Stock> &StockArray)
{
    for (auto itr=constinuents.begin();itr!=constinuents.end();itr++)
    {
        if (RANDOM_NUM < MUTATION_RATE)
        {
            constinuents.erase(itr);
            bool repstock=true;
            while (repstock) {
                Stock newconstinuent = StockArray[(int) (RANDOM_NUM * SP500_NUM)];
                std::vector<Stock>::iterator it = find(constinuents.begin(), constinuents.end(), newconstinuent);
                if(it == constinuents.end()) {
                    constinuents.insert(itr,newconstinuent);
                    repstock=false;
                }
            }
        }
    }
}
/*Portfolio Portfolio::Crossover(Portfolio& AnotherParent)
{
    //dependent on the crossover rate
    if (RANDOM_NUM < CROSSOVER_RATE)
    {
        //create a random crossover point
        int crossover = (int)(RANDOM_NUM * CHROMO_LENGTH);


    }
}*/


