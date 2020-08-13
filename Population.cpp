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
    Portfolio Empty('1');
    return Empty;
}
bool cmp (Portfolio c1,Portfolio c2) { return (c1<c2); }
void Population::Sort() {
    sort(portfolios.begin(),portfolios.end(),cmp);
}

Portfolio GeneticAlgorithm(vector<Stock>& stocks,char sign){
    //storage for our population of chromosomes.
    Population Generation;


    //first create a random population, all with zero fitness.
    for (int i = 0; i < POP_SIZE; i++)
    {
        Portfolio P(PORTFOLIO_SIZE,stocks,sign);
        // first generation
        Generation.AddPortfolio(P);
        Generation.CalFitness();
    }

    int GenerationsRequiredToFindASolution = 0;

    //set this flag if a solution has been found
    bool bFound = false;

    //enter the main GA loop
    while (!bFound)
    {


        //create a new population by selecting two parents at a time and creating offspring
        //by applying crossover and mutation. Do this until the desired number of offspring
        //have been created.

        //define some temporary storage for the new population we are about to create
        vector<Portfolio> temp;
        //from lowest up to POP_SIZE*CROSSOVER_RATE will be substituted


        int cPop = 0;//current population number
        Generation.Shuffle();
        //loop until we have created POP_SIZE new chromosomes
        while (cPop < int(POP_SIZE*CROSSOVER_RATE))
        {
            //create the new population by grabbing members of the old population
            //two at a time via roulette wheel selection.
            Portfolio parent1=Generation.Roulette();
            Portfolio parent2=Generation.Roulette();

            //add crossover based on the crossover rate
            Portfolio offspring1(sign),offspring2(sign);
            Crossover(parent1, parent2,offspring1,offspring2);

            //mutate based on the mutation rate
            offspring1.Mutate(stocks);
            offspring2.Mutate(stocks);
            offspring1.Assignfitness(stocks);
            offspring2.Assignfitness(stocks);

            //add these offspring to the new population.
            temp.push_back(offspring1);
            temp.push_back(offspring2);
            cPop+=2;
        }

        //copy temp population into main population array
        //Generation.Sort();
        for (int i = 0; i < int(POP_SIZE*CROSSOVER_RATE); i++)
        {
            Generation.NextGeneration(temp);
            Generation.CalFitness();
        }
        ++GenerationsRequiredToFindASolution;
        // exit app
        if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)
        {
            bFound = true;
        }


    }
    Portfolio best=Generation.GetBest();
    best.AssignConstituents(stocks);
    return best;
}
Portfolio GeneticAlgorithm(vector<Stock>& stocks,vector<double>& Maxfitness,char sign){
    //storage for our population of chromosomes.
    vector<double>().swap(Maxfitness);
    Population Generation;

    //first create a random population, all with zero fitness.
    for (int i = 0; i < POP_SIZE; i++)
    {
        Portfolio P(PORTFOLIO_SIZE,stocks,sign);
        // first generation
        Generation.AddPortfolio(P);
        Generation.CalFitness();
    }

    int GenerationsRequiredToFindASolution = 0;

    //set this flag if a solution has been found
    bool bFound = false;

    //enter the main GA loop
    while (!bFound)
    {


        //create a new population by selecting two parents at a time and creating offspring
        //by applying crossover and mutation. Do this until the desired number of offspring
        //have been created.

        //define some temporary storage for the new population we are about to create
        vector<Portfolio> temp;
        //from lowest up to POP_SIZE*CROSSOVER_RATE will be substituted


        int cPop = 0;//current population number
        Generation.Shuffle();
        //loop until we have created POP_SIZE new chromosomes
        while (cPop < int(POP_SIZE*CROSSOVER_RATE))
        {
            //create the new population by grabbing members of the old population
            //two at a time via roulette wheel selection.

            Portfolio parent1=Generation.Roulette();
            Portfolio parent2=Generation.Roulette();

            //add crossover based on the crossover rate
            Portfolio offspring1(sign),offspring2(sign);
            Crossover(parent1, parent2,offspring1,offspring2);

            //mutate based on the mutation rate
            offspring1.Mutate(stocks);
            offspring2.Mutate(stocks);
            offspring1.Assignfitness(stocks);
            offspring2.Assignfitness(stocks);


            //add these offspring to the new population.
            temp.push_back(offspring1);
            temp.push_back(offspring2);
            cPop+=2;
        }

        //copy temp population into main population array
        //
        for (int i = 0; i < int(POP_SIZE*CROSSOVER_RATE); i++)
        {
            Generation.NextGeneration(temp);
            Generation.CalFitness();
        }
        ++GenerationsRequiredToFindASolution;

        Maxfitness.push_back(Generation.GetBestFitness());
        // exit app
        if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)
        {
            bFound = true;
        }


    }
    Portfolio best=Generation.GetBest();
    best.AssignConstituents(stocks);
    return best;
}




