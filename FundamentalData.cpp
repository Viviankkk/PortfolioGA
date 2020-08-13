//
// Created by Vivian Kang on 6/22/20.
//

#include <string>
#include <map>
#include <iostream>
#include "json/json.h"
#include "database.h"
#include "marketdata.h"
#include "utility.h"
#include "curl/curl.h"
#include <mutex>
#include <sqlite3.h>
using namespace std;

std::mutex mylockf;
int PopulateFundamentalTable(const Json::Value& root, string symbol,vector<Fundamental>& FundamentalArray)
{

    float PERatio=0,DivYield=0,Beta=0,High52Weeks=0,Low52Week=0,MA50Days=0,MA200Days=0,ROA=0,EPSEstimateNext=0,EPSActual=0;
    int64_t Cap=0;
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        //PERatio=(*itr)["PERatio"].asFloat();
        string temp=itr.key().asString();
        if(itr.key().asString()=="Highlights"){
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;

            if (inner.key().asString() == "PERatio")
                PERatio = inner->asFloat();

            else if (inner.key().asString() == "DividendYield")
                DivYield = inner->asFloat();

            else if(inner.key().asString() == "MarketCapitalization")
                Cap = inner->asInt64();
            else if(inner.key().asString()=="ReturnOnAssetsTTM")
                ROA= inner->asFloat();

        }
        }
        else if(itr.key().asString()=="Technicals"){
            for(Json::Value::const_iterator inner=(*itr).begin();inner!=(*itr).end();inner++){
                if (inner.key().asString() == "Beta")
                    Beta = inner->asFloat();
                else if (inner.key() == "52WeekHigh")
                    High52Weeks = inner->asFloat();
                else if (inner.key().asString() == "52WeekLow")
                    Low52Week = inner->asFloat();
                else if (inner.key().asString() == "50DayMA")
                    MA50Days = inner->asFloat();
                else if (inner.key() == "200DayMA")
                    MA200Days = inner->asFloat();
            }
        }
        else if(itr.key().asString()=="Earnings"){
            for(Json::Value::const_iterator inner=(*itr).begin();inner!=(*itr).end();inner++){
                if (inner.key().asString() == "History"){
                    for(Json::Value::const_iterator innerinner=(*inner).begin();innerinner!=(*inner).end();innerinner++){
                        if (innerinner.key().asString().substr(0,7) == "2020-03"||
                        innerinner.key().asString().substr(0,7) =="2020-04"||innerinner.key().asString().substr(0,7) == "2020-05")

                            EPSEstimateNext=(*innerinner)["epsEstimate"].asFloat();

                        if (innerinner.key().asString().substr(0,7) == "2019-12"||
                            innerinner.key().asString().substr(0,7) =="2020-01"||innerinner.key().asString().substr(0,7) == "2020-02")
                            EPSActual=(*innerinner)["epsActual"].asFloat();
                    }
                }

            }
        }

        //Fundamental F(PERatio,DivYield,Beta,High52Weeks,Low52Week,MA50Days,MA200Days);

    }
    float ratio=0;
    if (EPSActual!=0) ratio=EPSEstimateNext/EPSActual;
    Fundamental F(symbol,PERatio,DivYield,Beta,High52Weeks,Low52Week,MA50Days,MA200Days,Cap,ROA,ratio);
    mylockf.lock();
    FundamentalArray.push_back(F);
    mylockf.unlock();
    return 0;
}

int MultiThreadFundamentalRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Fundamental>& F){
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    string stock_url_common = "https://eodhistoricaldata.com/api/fundamentals/";//"https://eodhistoricaldata.com/api/fundamentals/AAPL.US?api_token=5ba84ea974ab42.45160048"
    string api_token = "5ba84ea974ab42.45160048";
    for(auto itr=st;itr!=ed;itr++)
    {
        string stockDB_symbol=*itr;
        cout<<"Retrieving "<<stockDB_symbol<<endl;
        //Deal with special cases

        //Conflict with dot operator
        std::size_t found = stockDB_symbol.find('.');
        if (found!=std::string::npos) {
            stockDB_symbol = stockDB_symbol.replace(found, 1, "");
        }
        string stockDB_data_request = stock_url_common + stockDB_symbol + ".US?api_token=" + api_token ;

        Json::Value stockDB_root;   // will contains the root value after parsing.
        if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
            return -1;

        if (PopulateFundamentalTable(stockDB_root, *itr, F) == -1)
            return -1;
    }
    return 0;
}

int RetrieveFundamentalDataFromDB(Stock& S, sqlite3* db)
{
    int rc = 0;
    char* error = NULL;
    double minROA,maxROA,minEPS,maxEPS;
    GetMaxMin(maxROA,minROA,"ReturnOnAssets",db);
    GetMaxMin(maxEPS,minEPS,"EPSEstimate",db);


    string sql_select="SELECT * FROM FundamentalData where symbol=\'"+S.GetSymbol()+"\';";
    char** results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    int row=1;
    int pos=row*columns;
    S.AddFundamental(strtod(results[pos+1],NULL),strtod(results[pos+2],NULL),strtod(results[pos+3],NULL),strtod(results[pos+4],NULL),strtod(results[pos+5],NULL),strtod(results[pos+6],NULL),strtod(results[pos+7],NULL),strtoll(results[pos+8],NULL,0),Normalize(maxROA,minROA,strtod(results[pos+9],NULL)),Normalize(maxEPS,minEPS,strtod(results[pos+10],NULL)));


    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);

    return 0;
}