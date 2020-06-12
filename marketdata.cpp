//
// Created by Vivian Kang on 6/7/20.
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "marketdata.h"
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include "database.h"
#include <mutex>
using namespace std;

std::mutex mylock;
//writing call back function for storing fetched values in memory
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int RetrieveMarketData(string url_request, Json::Value& root)
{
    std::string readBuffer;



    //creating session handle
    CURL* myHandle;

    CURLcode result;
    myHandle = curl_easy_init();

    curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());
    curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201");
    curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1);

    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
    result = curl_easy_perform(myHandle);

    // check for errors
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
    }
    else {
        //cout << readBuffer << endl;
        //json parsing
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();
        string errors;

        bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
        if (not parsingSuccessful)
        {
            // Report failures and their locations in the document.
            cout << "Failed to parse JSON" << std::endl << readBuffer << errors << endl;
            return -1;
        }
        //std::cout << "\nSucess parsing json\n" << root << endl;

    }

    //End a libcurl easy handle.This function must be the last function to call for an easy session
    curl_easy_cleanup(myHandle);
    return 0;
}

int PopulateStockTable(const Json::Value& root, string symbol, vector<Stock>& StockArray, sqlite3* db)
{
    string date;
    float open, high, low, close, adjusted_close;
    int volume;
    Stock myStock(symbol);
    int count = 0;
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        //cout << *itr << endl;
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;

            if (inner.key().asString() == "adjusted_close")
                adjusted_close = inner->asFloat();
            else if (inner.key().asString() == "close")
                close = inner->asFloat();
            else if (inner.key() == "date")
                date = inner->asString();
            else if (inner.key().asString() == "high")
                high = inner->asFloat();
            else if (inner.key().asString() == "low")
                low = inner->asFloat();
            else if (inner.key() == "open")
                open = inner->asFloat();
            else if (inner.key().asString() == "volume")
                volume = inner->asInt64();
            else
            {
                cout << "Invalid json field" << endl;
                return -1;
            }
        }
        Trade aTrade(date, open, high, low, close, adjusted_close, volume);
        //mylock.lock();
        myStock.addTrade(aTrade);
        count++;
        //mylock.unlock();
        // Execute SQL
        /*char stockDB_insert_table[512];
        sprintf(stockDB_insert_table, "INSERT INTO MarketData (symbol, date, open, high, low, close, adjusted_close, volume) VALUES( \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)",  symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
        if (InsertTable(stockDB_insert_table, db) == -1)
           return -1;*/
    }
    mylock.lock();
    StockArray.push_back(myStock);
    mylock.unlock();
    return 0;
}

int PopulateSP500Table(const Json::Value& root, sqlite3* db)
{
    int count = 0;
    string name, symbol, sector;
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        //cout << *itr << endl;
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;

            if (inner.key().asString() == "Name")
                name = inner->asString();
            else if (inner.key().asString() == "Sector")
                sector = inner->asString();
            else if (inner.key() == "Symbol")
                symbol = inner->asString();
            else
            {
                cout << "Invalid json field" << endl;
                system("pause");
                return -1;
            }
        }
        count++;

        // Execute SQL
        char sp500_insert_table[512];
        sprintf(sp500_insert_table, "INSERT INTO SP500 (id, symbol, name, sector) VALUES(%d, \"%s\", \"%s\", \"%s\")", count, symbol.c_str(), name.c_str(), sector.c_str());
        if (InsertTable(sp500_insert_table, db) == -1)
            return -1;
    }
    return 0;
}

static int GetSymbolCallBack(void *list, int count, char** data, char **columns){
    vector<string>* ptr= static_cast<vector<string>*>(list);
    ptr->push_back(data[0]);
    return 0;
}

int GetSymbols(sqlite3* db,vector<string>& Symbolist){
    int rc = 0;
    char* error = NULL;
    vector<string>* ptrSymbolList=&Symbolist;
    cout<<"Retrieving S&P500 constituents from database..."<<endl;
    string sqlselect="SELECT symbol from SP500;";
    rc=sqlite3_exec(db,sqlselect.c_str(),GetSymbolCallBack,ptrSymbolList,&error);
    if (rc){
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    return 0;
}

int MultiThreadRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Stock>& StockArray,sqlite3* stockDB){
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
    string stock_start_date = "2010-01-02";
    string stock_end_date = "2019-12-31";
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
        string stockDB_data_request = stock_url_common + stockDB_symbol + ".US?" +
                                      "from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d&fmt=json";

        Json::Value stockDB_root;   // will contains the root value after parsing.
        if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
            return -1;
        if (PopulateStockTable(stockDB_root, *itr,StockArray, stockDB) == -1)
            return -1;
    }
    return 0;
}
