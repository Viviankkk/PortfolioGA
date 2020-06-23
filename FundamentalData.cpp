//
// Created by Vivian Kang on 6/22/20.
//

#include <string>
#include <map>
#include <iostream>
#include "json/json.h"
#include "database.h"
#include "marketdata.h"
#include "curl/curl.h"
#include <sqlite3.h>
using namespace std;


int PopulateFundamentalTable(const Json::Value& root, string symbol, sqlite3* db)
{
    sqlite3_exec(db, "begin;", 0, 0, 0);
    sqlite3_stmt* stmt;
    string stockDB_insert_table = "INSERT INTO FundamentalData VALUES(?, ?, ?, ?, ?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, stockDB_insert_table.c_str(), stockDB_insert_table.length(), &stmt, 0) != SQLITE_OK)
    {
        cerr << "Could not prepare statement." << endl;
        return -1;
    }
    float PERatio=0,DivYield=0,Beta=0,High52Weeks=0,Low52Week=0,MA50Days=0,MA200Days=0;
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

        //Fundamental F(PERatio,DivYield,Beta,High52Weeks,Low52Week,MA50Days,MA200Days);

    }
    sqlite3_reset(stmt);

    if (sqlite3_bind_text(stmt, 1, symbol.c_str(), symbol.length(), SQLITE_STATIC) != SQLITE_OK) {
        cerr << "Could not bind symbol." << endl;
        return -1;
    }

    if (sqlite3_bind_double(stmt, 2, PERatio) != SQLITE_OK) {
        cerr << "Could not bind PERatio." << endl;
        return -1;
    };

    if (sqlite3_bind_double(stmt, 3, DivYield) != SQLITE_OK) {
        cerr << "Could not bind DivYield." << endl;
        return -1;
    }

    if (sqlite3_bind_double(stmt, 4, Beta) != SQLITE_OK) {
        cerr << "Could not bind Beta." << endl;
        return -1;
    }

    if (sqlite3_bind_double(stmt, 5, High52Weeks) != SQLITE_OK) {
        cerr << "Could not bind High52Weeks." << endl;
        return -1;
    }

    if (sqlite3_bind_double(stmt, 6, Low52Week) != SQLITE_OK) {
        cerr << "Could not bind Low52Week." << endl;
        return -1;
    }

    if (sqlite3_bind_double(stmt, 7, MA50Days) != SQLITE_OK) {
        cerr << "Could not bind MA50Days." << endl;
        return 1;
    }

    if (sqlite3_bind_double(stmt, 8, MA200Days) != SQLITE_OK) {
        cerr << "Could not bind adjusted_close." << endl;
        return 1;
    }



    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        cerr << "Could not step (execute) stmt." << endl;
        return -1;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "commit;", 0, 0, 0);
    //char stockDB_insert_table[512];
    //sprintf(stockDB_insert_table, "INSERT INTO FundamentalData (symbol, PERatio, DividendYield, Beta, High52Weeks, Low52Weeks, MA50Days, MA200Days) VALUES( \"%s\", %f, %f, %f, %f, %f, %f, %f)",  symbol.c_str(), PERatio,DivYield,Beta,High52Weeks,Low52Week,MA50Days,MA200Days);
    //if (InsertTable(stockDB_insert_table, db) == -1)
    //    return -1;
    return 0;
}

int FundamentalRetrieve(vector<string>& stocklist,sqlite3* stockDB){
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    string stock_url_common = "https://eodhistoricaldata.com/api/fundamentals/";//"https://eodhistoricaldata.com/api/fundamentals/AAPL.US?api_token=5ba84ea974ab42.45160048"
    string api_token = "5ba84ea974ab42.45160048";
    for(auto itr=stocklist.begin();itr!=stocklist.end();itr++)
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
        if (PopulateFundamentalTable(stockDB_root, *itr, stockDB) == -1)
            return -1;
    }
    return 0;
}
