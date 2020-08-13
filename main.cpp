#include <iostream>
#include <sqlite3.h>
#include <thread>
#include "database.h"
#include "marketdata.h"
#include "Stock.h"
#include "time.h"
#include "FundamentalData.h"
#include "Portfolio.h"
#include "Population.h"
#include "Gnuplot.h"
#include "Tests.h"
#include <chrono>
#include <vector>
#include <string>
#include <fstream>

#define NUM_THREAD 5
#define NUM_OF_STOCKS 496

using namespace std;

int main() {
    srand( time( NULL ) );
    const char* stockDB_name = "SP500.db";
    sqlite3* stockDB = NULL;
    if (OpenDatabase(stockDB_name, stockDB) == -1)
        return -1;
    char selection;
    bool END=false;

    while (!END){
        cout<<"Menu:"<<endl;
        cout<<"A. Retrieve and populate S&P500 constituents."<<endl;
        cout<<"B. Retrieve and populate market data for S&P500 stocks and calculate return."<<endl;
        cout<<"C. Retrieve and populate fundamental data for S&P500 stocks."<<endl;
        cout<<"D. Back test For Exhibition. "<<endl;
        cout<<"E. Probation test For Exhibition."<<endl;
        cout<<"F. Tune Parameters."<<endl;
        cout<<"G. Back test for Stats."<<endl;
        cout<<"H. Probation test for Stats."<<endl;
        cout<<"X. Exit."<<endl;
        cout<<endl<<endl;
        cin>>selection;
        switch(selection)
        {
            case 'A':
            case'a': {
                string sp500_drop_table = "DROP TABLE IF EXISTS SP500;";
                if (DropTable(sp500_drop_table.c_str(), stockDB) == -1)
                    return -1;

                string sp500_create_table = "CREATE TABLE SP500 (id INT NOT NULL, symbol CHAR(20) PRIMARY KEY NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL);";

                if (CreateTable(sp500_create_table.c_str(), stockDB) == -1)
                    return -1;

                //string sp500_data_request = "https://pkgstore.datahub.io/core/s-and-p-500-companies/constituents_json/data/64dd3e9582b936b0352fdd826ecd3c95/constituents_json.json";
                string sp500_data_request="https://pkgstore.datahub.io/core/s-and-p-500-companies/constituents_json/data/8dcccae2a6de88b891fec045e4d6e2c8/constituents_json.json";
                Json::Value sp500_root;   // will contains the root value after parsing.
                if (RetrieveMarketData(sp500_data_request, sp500_root) == -1)
                    return -1;
                if (PopulateSP500Table(sp500_root, stockDB) == -1)
                    return -1;

                string sp500_select_table = "SELECT * FROM SP500;";
                if (DisplayTable(sp500_select_table.c_str(), stockDB) == -1)
                    return -1;
            }
            break;
            case 'B':
            case 'b':
            {
                //Get consituents from database
                vector<string> stocklist;
                if(GetSymbols(stockDB,stocklist)==-1) return -1;
                //Create Table Market Data
                if(CreateMarketTable("MarketData",stockDB)==-1) return -1;
                //Retrieving Data for S&P500 stocks
                cout<<"Retrieving Market Data..."<<endl;
                std::chrono::steady_clock::time_point begin=std::chrono::steady_clock::now();
                vector<Market> StockArray;
                //MultiThreading for Retrieving Data
                /*thread Retrieve[NUM_THREAD];

                int size=int(NUM_OF_STOCKS/NUM_THREAD);
                vector<string>::iterator st=stocklist.begin();
                vector<string>::iterator ed=stocklist.begin();
                for(int i=0;i<NUM_THREAD;i++) {
                    if (i == NUM_THREAD - 1) ed = stocklist.end();
                    else advance(ed, size);
                    Retrieve[i] = thread(MultiThreadMarketRetrieve, st, ed, ref(StockArray), stockDB);
                    st = ed;
                }
                for(int i=0;i<NUM_THREAD;i++) Retrieve[i].join();*/
                vector<string>::iterator st=stocklist.begin();
                vector<string>::iterator ed=stocklist.end();
                MultiThreadMarketRetrieve(st,ed,StockArray,stockDB);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time for Retrieving Market Data (sec) = " <<
                          (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0
                          <<std::endl;
                //Inserting data to database
                cout<<"Inserting data to database..."<<endl;

                begin=std::chrono::steady_clock::now();
                sqlite3_exec(stockDB,"begin;",0,0,0);
                sqlite3_exec(stockDB,"PRAGMA synchronous = OFF; ",0,0,0);
                for(auto itr=StockArray.begin();itr!=StockArray.end();itr++) {
                    vector<Trade> trades=itr->GetTrades();
                    for (auto ptr = trades.begin(); ptr != trades.end(); ptr++) {
                        //cout<<*ptr;
                        if(ptr->InsertATrade(itr->GetSymbol(),"MarketData",stockDB)==-1) return -1;
                    }
                }
                sqlite3_exec(stockDB,"commit;",0,0,0);
                end = std::chrono::steady_clock::now();
                std::cout << "Time for Inserting to Database (sec) = " <<
                          (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0
                          <<std::endl;
                cout<<endl<<endl;
                //Updating Daily Return
                /*cout<<"Upadting Daily Return to MarketData..."<<endl;

                //if(AddColumn("DailyReturn","MarketData","REAL",stockDB)==-1) return -1;
                for(auto itr=stocklist.begin();itr!=stocklist.end();itr++){
                    if(UpdateDailyRet(*itr,"MarketData",stockDB)==-1) return -1;
                }
                */


                //Deal with Index
                cout<<"Retrieving Reference Data..."<<endl;
                vector<string> Name{"TNX","SPY"};
                vector<string> Reflist{"^TNX","SPY"};
                for(int i=0;i<2;i++){
                    if(CreateMarketTable(Name[i],stockDB)==-1) return -1;
                    Market Mdata(Reflist[i]);
                    if(RetrieveDataFromYahoo(Mdata)== 1) return -1;
                    //clock_t t0=clock();
                    sqlite3_exec(stockDB,"begin;",0,0,0);
                    sqlite3_exec(stockDB,"PRAGMA synchronous = OFF; ",0,0,0);
                    vector<Trade> trades=Mdata.GetTrades();
                    for (auto ptr = trades.begin(); ptr != trades.end(); ptr++) {
                        cout<<*ptr;
                        if(ptr->InsertATrade(Mdata.GetSymbol(),Name[i],stockDB)==-1)
                            return -1;
                    }
                    sqlite3_exec(stockDB,"commit;",0,0,0);
                    //if(AddColumn("DailyReturn",Name[i],"REAL",stockDB)==-1) return -1;
                    //if(UpdateDailyRet(Reflist[i],Name[i],stockDB)==-1) return -1;
                    //cout << "Time elapsed for inserting data to database: " << setprecision(4) <<
                    //     (clock() - t0) * 1.0 / CLOCKS_PER_SEC / 60.0 << " min" << endl << endl;
                }
            }
            break;
            case 'c':
            case 'C':
            {
                //Get consituents from database
                vector<string> stocklist;
                if(GetSymbols(stockDB,stocklist)==-1) return -1;
                //Create Table Fundamental Data
                std::string stockDB_drop_table = "DROP TABLE IF EXISTS FundamentalData;";
                if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
                    return -1;

                string stockDB_create_table = "CREATE TABLE FundamentalData"\
                                              "(symbol CHAR(20) NOT NULL,"\
                                              "PERatio REAL NOT NULL,"\
                                              "DividendYield REAL NOT NULL,"\
                                              "Beta REAL NOT NULL,"\
                                              "High52Weeks REAL NOT NULL,"\
                                              "Low52Weeks REAL NOT NULL,"\
                                              "MA50Days REAL NOT NULL,"\
                                              "MA200Days REAL NOT NULL,"\
                                              "Capital UNSIGNED BIG INT NOT NULL,"\
                                              "ReturnOnAssets REAL NOT NULL,"\
                                              "EPSEstimate REAL NOT NULL,"\
                                              "PRIMARY KEY(symbol)"\
                                              "FOREIGN KEY(symbol) references SP500(symbol)"\
                                              "ON DELETE CASCADE ON UPDATE CASCADE"\
                                              ");";

                if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
                    return -1;
                //clock_t t0 = clock();
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                //MultiThreading for Retrieving Data
                thread Retrieve[NUM_THREAD];
                vector<Fundamental> FArray;
                int size=int(NUM_OF_STOCKS/NUM_THREAD);
                vector<string>::iterator st=stocklist.begin();
                vector<string>::iterator ed=stocklist.begin();
                for(int i=0;i<NUM_THREAD;i++){
                    if(i==NUM_THREAD-1) ed=stocklist.end();
                    else advance(ed,size);
                    Retrieve[i]=thread(MultiThreadFundamentalRetrieve,st,ed,ref(FArray));
                    st=ed;
                }
                for(int i=0;i<NUM_THREAD;i++) Retrieve[i].join();
                //MultiThreadFundamentalRetrieve(st,stocklist.end(),FArray);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time for Retrieving Fundamental Data (sec) = " <<
                          (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0
                          <<std::endl;



                begin=std::chrono::steady_clock::now();

                sqlite3_exec(stockDB,"begin;",0,0,0);
                sqlite3_exec(stockDB,"PRAGMA synchronous = OFF; ",0,0,0);
                for(auto itr=FArray.begin();itr!=FArray.end();itr++) {
                    if(itr->InsertFundamental(stockDB)==-1) return -1;
                }
                sqlite3_exec(stockDB,"commit;",0,0,0);

                end = std::chrono::steady_clock::now();
                std::cout << "Time for Inserting to Database (sec) = " <<
                          (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0
                          <<std::endl;
                /*//Displaying
                string stockDB_select_table = "SELECT * FROM FundamentalData;";
                if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1) return -1;
                cout<<endl<<endl;*/
            }
            break;
            case 'd':
            case 'D':
            {
                bool subEND=false;
                while(!subEND) {
                    cout << endl;
                    cout << "Choose Fitness Function:" << endl;
                    cout << "1. Fitness = Return/Risk" << endl;
                    cout << "2. Fitness = V*P*Q + V*V*A" << endl;
                    cout << "3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)" << endl;
                    cout << "0. Return Menu" << endl;
                    char subselection;
                    cin>>subselection;
                    switch(subselection) {
                        case '2':{
                        TestMetrics BK;
                        BK.date_st = "2020-01-01";
                        BK.date_ed = "2020-06-17";
                        string startdate = "2019-06-30";
                        string enddate = "2019-12-31";
                        BuyandHold(BK,startdate,enddate,stockDB,'2');
                        plotCumulative(BK.cret,BK.crefret);
                        }
                        break;
                        case '1':
                        case '3':{
                            TestMetrics BK;
                            //Date For historical data
                            string startdate="2018-01-01";
                            string enddate="2019-12-31";
                            //Date For BackTest data
                            BK.date_st="2020-01-01";
                            BK.date_ed="2020-06-17";
                            MonthlyRebalancing(BK,startdate,enddate,stockDB,subselection);
                            plotCumulative(BK.cret,BK.crefret);
                        }
                        break;
                        case'0':{
                            subEND=true;
                            break;
                        }
                        default:{
                            cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                            cin.clear();cin.sync();
                            break;
                        }
                    }
                }

            }
            break;
            case 'e':
            case 'E':
            {
                bool subEND=false;
                while(!subEND) {
                    cout << endl;
                    cout << "Choose Fitness Function:" << endl;
                    cout << "1. Fitness = Return/Risk" << endl;
                    cout << "2. Fitness = V*P*Q + V*V*A" << endl;
                    cout << "3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)" << endl;
                    cout << "0. Return Menu" << endl;
                    char subselection;
                    cin>>subselection;
                    switch(subselection) {
                        case '1':
                        case '2':
                        case '3':{
                            TestMetrics BK;
                            //Date For historical data
                            string startdate="2018-07-01";
                            string enddate="2020-06-30";
                            //Date For BackTest data
                            BK.date_st="2020-07-01";
                            BK.date_ed="2020-07-20";
                            BuyandHold(BK,startdate,enddate,stockDB,subselection);
                            plotCumulative(BK.cret,BK.crefret);
                        }
                            break;
                        case'0':{
                            subEND=true;
                            break;
                        }
                        default:{
                            cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                            cin.clear();cin.sync();
                            break;
                        }
                    }
                }

            }
                break;
            case 'f':
            case 'F':{
                bool subEND=false;
                while(!subEND) {
                    cout << endl;
                    cout << "Choose Fitness Function:" << endl;
                    cout << "1. Fitness = Return/Risk" << endl;
                    cout << "2. Fitness = V*P*Q + V*V*A" << endl;
                    cout << "3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)" << endl;
                    cout << "0. Return Menu" << endl;
                    char subselection;
                    cin>>subselection;
                    switch(subselection) {
                        case '1':
                        case '2':
                        case '3':{
                            string startdate="2019-06-30";
                            string enddate="2019-12-31";

                            Stock SPY("SPY");
                            if (RetrieveMarketDataFromDB(SPY, "SPY", startdate, enddate, stockDB) == -1) return -1;
                            int length = SPY.GetDates().size();
                            vector<string> stocklist;

                            if (GetSymbols(stockDB, stocklist) == -1) return -1;
                            vector<Stock> stocks;
                            for (auto itr = stocklist.begin(); itr != stocklist.end(); itr++) {
                                Stock mystock(*itr);
                                if (RetrieveMarketDataFromDB(mystock, "MarketData", startdate, enddate, stockDB) == -1)
                                    return -1;
                                if (RetrieveFundamentalDataFromDB(mystock, stockDB) == -1) return -1;
                                if (mystock.GetDates().size() != length) { continue; }
                                mystock.CalRet(PERIOD);
                                stocks.push_back(mystock);

                            }
                            vector<double> MaxFitness(MAX_ALLOWABLE_GENERATIONS,0);
                            for (int i=0;i<20;i++) {
                                vector<double> temp;
                                Portfolio Hold = GeneticAlgorithm(stocks, temp, subselection);
                                for(int j=0;j<MAX_ALLOWABLE_GENERATIONS;j++){
                                    MaxFitness[j]=(MaxFitness[j]*(i)+temp[j])/(i+1);
                                }
                            }

                            plotResults(MaxFitness,MAX_ALLOWABLE_GENERATIONS,"Generations");

                        }
                            break;
                        case'0':{
                            subEND=true;
                            break;
                        }
                        default:{
                            cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                            cin.clear();cin.sync();
                            break;
                        }
                    }
                }
                cin.clear();cin.sync();

            }
                break;
            case 'g':
            case 'G':
            {
                bool subEND=false;
                while(!subEND) {
                    cout << endl;
                    cout << "Choose Fitness Function:" << endl;
                    cout << "1. Fitness = Return/Risk" << endl;
                    cout << "2. Fitness = V*P*Q + V*V*A" << endl;
                    cout << "3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)" << endl;
                    cout << "4. Go through all fitness functions"<<endl;
                    cout << "0. Return Menu" << endl;
                    char subselection;
                    cin>>subselection;
                    ofstream myfile;
                    myfile.open ("Backtest.txt",ios::out | ios::app);
                    switch(subselection) {
                        case '2': {
                            StatTest(myfile,subselection,stockDB,"2019-06-30","2019-12-31","2020-01-01","2020-06-17",30,true);
                        }
                            break;
                        case '1':
                        case '3':{
                            StatTest(myfile,subselection,stockDB,"2018-01-01","2019-12-31","2020-01-01","2020-06-17",30,true);
                        }
                            break;
                        case '4':{
                            StatTest(myfile,'1',stockDB,"2018-01-01","2019-12-31","2020-01-01","2020-06-17",30,true);
                            StatTest(myfile,'2',stockDB,"2019-06-30","2019-12-31","2020-01-01","2020-06-17",30,true);
                            StatTest(myfile,'3',stockDB,"2018-01-01","2019-12-31","2020-01-01","2020-06-17",30,true);
                        }
                            break;
                        case'0':{
                            subEND=true;
                            myfile.close();
                            break;
                        }
                        default:{
                            cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                            cin.clear();cin.sync();
                            break;
                        }
                    }
                }

            }
            break;
            case'H':
            case'h':
            {
                bool subEND=false;
                while(!subEND) {
                    cout << endl;
                    cout << "Choose Fitness Function:" << endl;
                    cout << "1. Fitness = Return/Risk" << endl;
                    cout << "2. Fitness = V*P*Q + V*V*A" << endl;
                    cout << "3. Fitness = Scaled(Return/Risk) + Scaled(V*P*Q + V*V*A)" << endl;
                    cout << "4. Go through all fitness functions"<<endl;
                    cout << "0. Return Menu" << endl;
                    char subselection;
                    cin>>subselection;
                    //Date For historical data
                    string startdate="2018-07-01";
                    string enddate="2020-06-30";
                    //Date For BackTest data
                    string date_st="2020-07-01";
                    string date_ed="2020-07-20";
                    ofstream myfile;
                    myfile.open ("Probationtest.txt",ios::out | ios::app);
                    switch(subselection) {
                        case '2': {
                            StatTest(myfile,subselection,stockDB,"2020-01-30",enddate,date_st,date_ed,30,false);
                        }
                            break;
                        case '1':
                        case '3':{
                            StatTest(myfile,subselection,stockDB,startdate,enddate,date_st,date_ed,30,false);
                        }
                            break;
                        case '4':{
                            StatTest(myfile,'1',stockDB,startdate,enddate,date_st,date_ed,30,false);
                            StatTest(myfile,'2',stockDB,"2020-01-30",enddate,date_st,date_ed,30,false);
                            StatTest(myfile,'3',stockDB,startdate,enddate,date_st,date_ed,30,false);
                        }
                            break;
                        case'0':{
                            subEND=true;
                            myfile.close();
                            break;
                        }
                        default:{
                            cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                            cin.clear();cin.sync();
                            break;
                        }
                    }
                }

            }

            break;
            case 'x':
            case 'X':
            {
                END=true;
                cout<<endl<<endl<<endl<<endl;
                break;
            }
            default:
            {
                cout<<"Invalid Input: Please re-enter your selection."<<endl<<endl<<endl<<endl;
                cin.clear();cin.sync();
                break;
            }

        }
    }
    CloseDatabase(stockDB);
    cout << "Closed Stock database" << endl << endl;
    return 0;
}
