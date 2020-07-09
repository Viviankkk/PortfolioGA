//
// Created by Vivian Kang on 7/7/20.
//
#include "Stock.h"
#include "Portfolio.h"
#include "Population.h"
#include "marketdata.h"
Portfolio Population::Roulette()
{
    //generate a random number between 0 & total fitness count
    float Slice = (float)(RANDOM_NUM * total_fitness);

    //go through the chromosones adding up the fitness so far
    float FitnessSoFar = 0.0f;

    for (int i = 0; i < POP_SIZE; i++)
    {
        FitnessSoFar += portfolios[i].fitness;

        //if the fitness so far > random number return the chromo at this point
        if (FitnessSoFar >= Slice)
            return portfolios[i];
    }
    Portfolio Empty;
    return Empty;
}
bool cmp (Portfolio c1,Portfolio c2) { return (c1<c2); }
void Population::Sort() {
    sort(portfolios.begin(),portfolios.end(),cmp);
}

Portfolio GeneticAlgorithm(vector<Stock>& stocks){
    //storage for our population of chromosomes.
    Population Generation;

    //vector<chromo_type> Population(POP_SIZE);
    //chromo_type Population[POP_SIZE];//population has POP_SIZE chromosomes

    //first create a random population, all with zero fitness.
    for (int i = 0; i < POP_SIZE; i++)
    {
        Portfolio P(PORTFOLIO_SIZE,stocks);
        // first generation
        Generation.AddPortfolio(P);
        Generation.CalFitness();
    }

    int GenerationsRequiredToFindASolution = 0;
    //double prevbest=

    //set this flag if a solution has been found
    bool bFound = false;

    //enter the main GA loop
    while (!bFound)
    {
        /*//this is used during roulette wheel sampling
        float TotalFitness = 0.0f;

        //test and update the fitness of every chromosome in the population
        for (int i = 0; i < POP_SIZE; i++)
        {
            Population[i].fitness = AssignFitness(Population[i].bits, Target);
            TotalFitness += Population[i].fitness;
        }

        //check to see if we have found any solutions (fitness will be 999)
        for (int i = 0; i < POP_SIZE; i++)
        {
            if (Population[i].fitness == 999.0f)
            {
                cout << "\nSolution found in " << GenerationsRequiredToFindASolution << " generations!" << endl << endl;;
                cout << "Result: ";
                PrintChromo(Population[i].bits);

                bFound = true;
                break;
            }
        }*/



        //create a new population by selecting two parents at a time and creating offspring
        //by applying crossover and mutation. Do this until the desired number of offspring
        //have been created.

        //define some temporary storage for the new population we are about to create
        vector<Portfolio> temp;
        //from lowest up to POP_SIZE*CROSSOVER_RATE will be substituted


        int cPop = 0;//current population number

        //loop until we have created POP_SIZE new chromosomes
        while (cPop < int(POP_SIZE*CROSSOVER_RATE))
        {
            //create the new population by grabbing members of the old population
            //two at a time via roulette wheel selection.
            Portfolio parent1=Generation.Roulette();
            Portfolio parent2=Generation.Roulette();

            //add crossover based on the crossover rate
            Portfolio offspring1,offspring2;
            Crossover(parent1, parent2,offspring1,offspring2);

            //mutate based on the mutation rate
            offspring1.Mutate(stocks);
            offspring2.Mutate(stocks);
            offspring1.Assignfitness(stocks);
            offspring2.Assignfitness(stocks);
            //Mutate(offspring1);
            //Mutate(offspring2);

            //add these offspring to the new population. (assigning zero as their fitness scores)
            //temp[cPop++] = chromo_type(offspring1, 0.0f);
            //temp[cPop++] = chromo_type(offspring2, 0.0f);
            temp.push_back(offspring1);
            temp.push_back(offspring2);
            cPop+=2;
        }
        //sort(Population.begin(),Population.end(),cmp);
        //copy temp population into main population array
        //Generation.Sort();
        for (int i = 0; i < int(POP_SIZE*CROSSOVER_RATE); i++)
        {
            Generation.NextGeneration(temp);
            Generation.CalFitness();
        }
        ++GenerationsRequiredToFindASolution;
        // exit app if no solution found within the maximum allowable number of generations
        if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)
        {
            //cout << "No solutions found this run!";
            bFound = true;
        }
        //if ()

    }
    Portfolio best=Generation.GetBest();
    best.AssignConstituents(stocks);
    return best;
}

double CalPnL(Portfolio& P,string st,string ed,sqlite3* stockDB){
    double PnL=0;
    vector<string> constituents=P.GetConstituents();
    vector<Stock> stocks;
   for(auto itr=constituents.begin();itr!=constituents.end();itr++){
       Stock mystock(*itr);
       if(RetrieveMarketDataFromDB(mystock,"MarketData",st,ed,stockDB)==-1) return -1;
       mystock.CalculatePnL();
       stocks.push_back(mystock);
   }
   vector<double> weights=P.GetWeight();
   for(int i=0;i<stocks.size();i++){
       PnL+=stocks[i].CalculatePnL()*weights[i];
   }
   return PnL;
}