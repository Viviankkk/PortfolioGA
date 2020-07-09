//
// Created by Vivian Kang on 6/24/20.
#include "Portfolio.h"
#include "Stock.h"
#include "OperatorOverloading.h"
#include <string>
#include <iostream>
#include <math.h>

using namespace std;
Portfolio::Portfolio(int length,vector<Stock> &stocks) {
    vector<int> indexlst;
    for (int i = 0; i < length; i++) {
        int index = (int) (RANDOM_NUM * SP500_NUM);
        std::vector<int>::iterator it = find(indexlst.begin(), indexlst.end(), index);
        if (it == indexlst.end()) {id.push_back(index);constituents.push_back(stocks[index].GetSymbol());}
    }
    Assignfitness(stocks);
}
Portfolio::Portfolio(vector<int> &id_) {
    id=id_;
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
    return pow(1+sum,0.5217)-1;//annualized

}



/*void Portfolio::calbeta(map<string,Stock>& stockmap){

}*/



/*Portfolio::Portfolio(int length,vector<Stock>& StockArray){
    vector<int> indexlst;
    for(int i=0;i<length;i++){
        int index=(int)(RANDOM_NUM*SP500_NUM);
        std::vector<int>::iterator it=find(indexlst.begin(),indexlst.end(),index);
        if (it == indexlst.end()) constinuents.push_back(StockArray[index]);
    }
    Assignfitness();
    //fitness=0.0;
}

void Portfolio::calret() {
    ret.resize(constinuents[0].dailyret.size());
    for(int i=0;i<constinuents.size();i++){ ret = ret + constinuents[i].dailyret*weights[i];}
}
void Portfolio::calbeta() {
    for(int i=0;i<constinuents.size();i++){
        beta += constinuents[i].Beta*weights[i];
    }
}*/


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

void Crossover(Portfolio& Parent,Portfolio& AnotherParent,Portfolio& Child1,Portfolio& Child2)
{
    //dependent on the crossover rate
    //if (RANDOM_NUM < CROSSOVER_RATE)
    //{
        //create a random crossover point
        //int crossover = (int)(RANDOM_NUM * CHROMO_LENGTH);}
        int crpoint=RANDOM_NUM*PORTFOLIO_SIZE;
        vector<int> a=Parent.GetID();
        vector<int> b=AnotherParent.GetID();
        vector<int> c1(a.begin(),a.begin()+crpoint);
        vector<int> c2(b.begin(),b.begin()+crpoint);
        c1.insert(c1.end(),b.begin()+crpoint,b.end());
        c2.insert(c2.end(),a.begin()+crpoint,a.end());
        Child1.AssignID(c1);
        Child2.AssignID(c2);
}



