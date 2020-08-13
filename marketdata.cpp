//
// Created by Vivian Kang on 6/7/20.
//
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "marketdata.h"
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include "database.h"
#include "Stock.h"
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

int PopulateStockTable(const Json::Value& root, string symbol, vector<Market>& StockArray, sqlite3* db)
{
    string date;
    float open, high, low, close, adjusted_close;
    double ret;
    int volume;
    Market myStock(symbol);
    //int count = 0;
    float prevclose=0;
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
        if(prevclose==0) ret=0;
        else ret=close/prevclose-1;
        Trade aTrade(date, open, high, low, close, adjusted_close, volume,ret);
        //mylock.lock();
        myStock.addTrade(aTrade);
        prevclose=close;
        //count++;
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

int CreateMarketTable(string name,sqlite3* db){
    std::string stockDB_drop_table = "DROP TABLE IF EXISTS "+name+";";
    if (DropTable(stockDB_drop_table.c_str(), db) == -1)
        return -1;
    string stockDB_create_table = "CREATE TABLE " + name
            + "(symbol CHAR(20) NOT NULL,"\
            "date CHAR(20) NOT NULL,"\
            "open REAL NOT NULL,"\
            "high REAL NOT NULL,"\
            "low REAL NOT NULL,"\
            "close REAL NOT NULL,"\
            "adjusted_close REAL NOT NULL,"\
            "volume UNSIGNED BIG INT NOT NULL,"\
            "return REAL DEFAULT 0,"\
            "PRIMARY KEY(symbol,date)"\
            "FOREIGN KEY(symbol) references SP500(symbol)"\
            "ON DELETE CASCADE ON UPDATE CASCADE);";
    if (CreateTable(stockDB_create_table.c_str(), db) == -1)
        return -1;
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
    //cout<<"Retrieving S&P500 constituents from database..."<<endl;
    string sqlselect="SELECT symbol from SP500;";
    rc=sqlite3_exec(db,sqlselect.c_str(),GetSymbolCallBack,ptrSymbolList,&error);
    if (rc){
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    return 0;
}

static int GetRetCallBack(void *list, int count, char** results, char **columns){
    Stock* ptr= static_cast<Stock*>(list);
    //Trade aTrade(results[1],strtod(results[2],NULL),strtod(results[3],NULL),strtod(results[4],NULL),strtod(results[5],NULL),strtod(results[6],NULL),strtod(results[7],NULL),strtod(results[8],NULL));
    //ptr->addTrade(aTrade);
    ptr->adddates(results[0]);
    ptr->addClose(strtod(results[1],NULL));
    //ptr->addRet(strtod(results[2],NULL));
    return 0;
}

int RetrieveMarketDataFromDB(Stock& S, string tablename,string startdate,string enddate,sqlite3* db){
    int rc = 0;
    char* error = NULL;
    Stock* ptr=&S;
    //string sqlselect="select * from " +tablename+" where symbol=\'"+S.GetSymbol()+"\';";
    string sqlselect="SELECT date,adjusted_close FROM  "+tablename+" where symbol=\'"+S.GetSymbol()+"\' and date<=\'"+enddate+"\' and date>=\'"+startdate+"\';";
    rc=sqlite3_exec(db,sqlselect.c_str(),GetRetCallBack,ptr,&error);
    if (rc){
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    return 0;
}

int MultiThreadMarketRetrieve(vector<string>::iterator st,vector<string>::iterator ed,vector<Market>& StockArray,sqlite3* stockDB){
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
    string stock_start_date = "2010-01-02";
    string stock_end_date = "2020-07-27";
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

int write_data(void *ptr, int size, int nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

void *myrealloc(void *ptr, size_t size)
{	if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}

int write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{	size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

string getTimeinSeconds(string Time)
{
    std::tm t = {0};
    std::istringstream ssTime(Time);
    char time[100];
    memset(time, 0, 100);
    if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
    {
        cout << std::put_time(&t, "%c %Z") << "\n"
             << std::mktime(&t) << "\n";
        sprintf (time, "%lld", mktime(&t));
        return string(time);
    }
    else
    {
        cout << "Parse failed\n";
        return "";
    }
}

int RetrieveDataFromYahoo(Market& S)
{
    string symbol=S.GetSymbol();
    string startTime = getTimeinSeconds("2010-01-01T16:00:00");
    string endTime = getTimeinSeconds("2020-07-27T16:00:00");

    struct MemoryStruct data;
    data.memory = NULL;
    data.size = 0;

    // file pointer to create file that store the data
    FILE *fp;

    // name of files
    const char outfilename[FILENAME_MAX] = "Output.txt";
    const char resultfilename[FILENAME_MAX] = "Results.txt";

    // declaration of an object CURL
    CURL *handle;

    CURLcode result;

    // set up the program environment that libcurl needs
    curl_global_init(CURL_GLOBAL_ALL);

    // curl_easy_init() returns a CURL easy handle
    handle = curl_easy_init();

    // if everything's all right with the easy handle...
    if (handle)
    {

            string sCookies, sCrumb;
            if (sCookies.length() == 0 || sCrumb.length() == 0)
            {
                fp = fopen(outfilename, "w");
                //curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history?p=AMZN");
                curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
                curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
                curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

                curl_easy_setopt(handle, CURLOPT_ENCODING, "");
                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
                result = curl_easy_perform(handle);
                fclose(fp);

                if (result != CURLE_OK)
                {
                    // if errors have occurred, tell us what is wrong with result
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                    return 1;
                }

                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);

                // perform, then store the expected code in result
                result = curl_easy_perform(handle);

                if (result != CURLE_OK)
                {
                    // if errors have occured, tell us what is wrong with result
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                    return 1;
                }

                char cKey[] = "CrumbStore\":{\"crumb\":\"";
                char *ptr1 = strstr(data.memory, cKey);
                char *ptr2 = ptr1 + strlen(cKey);
                char *ptr3 = strstr(ptr2, "\"}");
                if ( ptr3 != NULL )
                    *ptr3 = NULL;

                sCrumb = ptr2;

                fp = fopen("cookies.txt", "r");
                char cCookies[100];
                if (fp) {
                    while (fscanf(fp, "%s", cCookies) != EOF);
                    fclose(fp);
                }
                else
                    cerr << "cookies.txt does not exists" << endl;

                sCookies = cCookies;
                free(data.memory);
                data.memory = NULL;
                data.size= 0;
            }

            //if (itr == symbolList.end())
            //    break;

            string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
            //string symbol = *itr;
            string urlB = "?period1=";
            string urlC = "&period2=";
            string urlD = "&interval=1d&events=history&crumb=";
            string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
            const char * cURL = url.c_str();
            const char * cookies = sCookies.c_str();
            curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
            curl_easy_setopt(handle, CURLOPT_URL, cURL);
            fp = fopen(resultfilename, "ab");
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
            result = curl_easy_perform(handle);
            fclose(fp);

            // Check for errors */
            if (result != CURLE_OK)
            {
                // if errors have occurred, tell us what is wrong with 'result'
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);

            if (result != CURLE_OK)
            {
                // if errors have occurred, tell us what is wrong with result
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }


            stringstream sData;
            sData.str(data.memory);
            string sValue, sDate;
            vector<double> data(6);
            string line;
            getline(sData, line);
            while ( getline(sData, line) ) {
                //cout << line << endl;
                sDate = line.substr(0, line.find_first_of(','));
                //line.erase(line.find_last_of(','));
                for(int i=5;i>=0;i--) {
                    sValue = line.substr(line.find_last_of(',') + 1);
                    line.erase(line.find_last_of(','));
                    data[i] = strtod(sValue.c_str(), NULL);
                }
                if (data[0]!=0)
                {
                    Trade aTrade(sDate,data[0],data[1],data[2],data[3],data[4],data[5],0);
                    S.addTrade(aTrade);
                }
            }

    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }

    free(data.memory);
    data.size= 0;

    // cleanup since you've used curl_easy_init
    curl_easy_cleanup(handle);

    // release resources acquired by curl_global_init()
    curl_global_cleanup();

    return 0;
}

