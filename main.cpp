#include <iostream>
#include <sqlite3.h>
#include <thread>
#include <iomanip>
#include "database.h"
#include "marketdata.h"
#include "Stock.h"
#include "time.h"
#include "FundamentalData.h"
#define NUM_THREAD 8
#define NUM_OF_STOCKS 505
using namespace std;

int main() {
    const char* stockDB_name = "SP500.db";
    sqlite3* stockDB = NULL;
    if (OpenDatabase(stockDB_name, stockDB) == -1)
        return -1;
    char selection;
    bool END=false;

    while (!END){
        cout<<"Menu:"<<endl;
        cout<<"A. Retrieve and populate S&P500 constituents."<<endl;
        cout<<"B. Retrieve and populate market data for S&P500 stocks."<<endl;
        cout<<"C. Retrieve and populate fundamental data for S&P500 stocks."<<endl;
        cout<<"D. test"<<endl;
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
                std::string stockDB_drop_table = "DROP TABLE IF EXISTS MarketData;";
                if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
                    return -1;

                string stockDB_create_table = "CREATE TABLE MarketData"\
                                              "(symbol CHAR(20) NOT NULL,"\
                                              "date CHAR(20) NOT NULL,"\
                                              "open REAL NOT NULL,"\
                                              "high REAL NOT NULL,"\
                                              "low REAL NOT NULL,"\
                                              "close REAL NOT NULL,"\
                                              "adjusted_close REAL NOT NULL,"\
                                              "volume UNSIGNED BIG INT NOT NULL,"\
                                              "PRIMARY KEY(symbol,date)"
                                              "FOREIGN KEY(symbol) references SP500(symbol)"
                                              "ON DELETE CASCADE ON UPDATE CASCADE);";

                if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
                    return -1;




                clock_t t0 = clock();

                //MultiThreading for Retrieving Data
                thread Retrieve[NUM_THREAD];
                vector<Stock> StockArray;
                int size=int(NUM_OF_STOCKS/NUM_THREAD);
                vector<string>::iterator st=stocklist.begin();
                vector<string>::iterator ed=stocklist.begin();
                for(int i=0;i<NUM_THREAD;i++){
                    if(i==NUM_THREAD-1) ed=stocklist.end();
                    else advance(ed,size);
                    Retrieve[i]=thread(MultiThreadRetrieve,st,ed,ref(StockArray),stockDB);
                    st=ed;
                }
                for(int i=0;i<NUM_THREAD;i++) Retrieve[i].join();
                cout << "Time elapsed for retrieving data: " << setprecision(4) <<
                     (clock() - t0) * 1.0 / CLOCKS_PER_SEC / 60.0 << " min" << endl << endl;

                //Inserting data to database
                t0=clock();
                sqlite3_exec(stockDB,"begin;",0,0,0);
                sqlite3_exec(stockDB,"PRAGMA synchronous = OFF; ",0,0,0);
                for(auto itr=StockArray.begin();itr!=StockArray.end();itr++) {
                    vector<Trade> trades=itr->GetTrades();
                    for (auto ptr = trades.begin(); ptr != trades.end(); ptr++) {
                        //cout<<*ptr;
                        if(ptr->InsertATrade(itr->GetSymbol(),stockDB)==-1) return -1;
                    }
                }
                sqlite3_exec(stockDB,"commit;",0,0,0);
                cout << "Time elapsed for inserting data to database: " << setprecision(4) <<
                     (clock() - t0) * 1.0 / CLOCKS_PER_SEC / 60.0 << " min" << endl << endl;

                //Displaying
                /*string stockDB_select_table = "SELECT * FROM MarketData;";
                        if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
                            return -1;*/
                cout<<endl<<endl;
            }
            break;
            case 'c':
            case 'C':
            {
                //Get consituents from database
                vector<string> stocklist;
                if(GetSymbols(stockDB,stocklist)==-1) return -1;
                //Create Table Market Data
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
                                              "PRIMARY KEY(symbol)"\
                                              //"FOREIGN KEY(symbol) references SP500(symbol)"
                                              //"ON DELETE CASCADE ON UPDATE CASCADE"
                                              ");";

                if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
                    return -1;

                clock_t t0 = clock();
                if(FundamentalRetrieve(stocklist,stockDB)==-1) return -1;
                cout << "Time elapsed for inserting data to database: " << setprecision(4) <<
                     (clock() - t0) * 1.0 / CLOCKS_PER_SEC / 60.0 << " min" << endl << endl;

                //Displaying
                string stockDB_select_table = "SELECT * FROM FundamentalData;";
                        if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
                            return -1;
                cout<<endl<<endl;
            }
            break;
            case 'd':
            case 'D':
            {
                //Get consituents from database
                vector<string> stocklist;
                vector<Stock> stocks;
                if(GetSymbols(stockDB,stocklist)==-1) return -1;
                //Get ret
                for(auto itr=stocklist.begin();itr!=stocklist.end();itr++){
                    Stock mystock(*itr);
                    if(GetReturn(mystock,stockDB)==-1) return -1;
                    stocks.push_back(mystock);
                }
                //Display
                for(auto itr=stocks.begin();itr!=stocks.end();itr++){
                    cout<<*itr<<endl;
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
