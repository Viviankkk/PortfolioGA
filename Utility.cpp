//
// Created by Vivian Kang on 6/25/20.
//

#include "Utility.h"
#include "sqlite3.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

vector<double> operator+ (const vector<double>& a, const vector<double>&  b) {
    vector<double> c(a.size(),0);
    if (a.size()!=b.size()) {
        cout<<"Vector sizes do not match!"<<endl;
        return c;
    }
    for(int i=0;i<a.size();i++){
        c[i]=a[i]+b[i];
    }
    return c;
}


vector<double> operator* (const vector<double>&  a, const double& b) {
    vector<double> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]*b;
    }
    return c;
}
vector<double> operator/ (const vector<double>&  a, const double& b) {
    vector<double> c(a.size(),0);
    for(int i=0;i<a.size();i++){
        c[i]=a[i]/b;
    }
    return c;
}

vector<double> operator* (vector<vector<double>>&  a, vector<double>&  b) {
    if (a.size()!=b.size()) {
        cout<<"Matrix sizes do not match!"<<endl;
        return b;
    }
    vector<double> c(a[0].size());
    for(int i=0;i<a[0].size();i++){
        c[i]=0;
        for (int j=0;j<a.size();j++){
            c[i]+=a[j][i]*b[j];
        }
    }
    return c;
}

double sum(vector<double>& vec){
    double sum=0;
    for(int i=0;i<vec.size();i++){
        sum+=vec[i];
    }
    return sum;
}
double mean(vector<double>& vec){
    return sum(vec)/vec.size();
}
double cov(vector<double>& x,vector<double>& y){
    double xmean=mean(x);
    double ymean=mean(y);
    if(x.size()!=y.size()) return -999;
    double total = 0;
    for(int i = 0; i < x.size(); i++)
    {
        total += (x[i] - xmean) * (y[i] - ymean);
    }
    return total / x.size();
}
vector<vector<double>> covariance_matrix(vector<vector<double>>& matrix){
    vector<vector<double>> covmatrix(matrix.size());
    for(int i=0;i<matrix.size();i++){
        covmatrix[i].resize(matrix.size());
        for(int j=0;j<matrix.size();j++){

            if(j>=i){
                covmatrix[i][j]=cov(matrix[i],matrix[j]);
            }
            else covmatrix[i][j]=covmatrix[j][i];
        }
    }
    return covmatrix;
}

string DateAhead(string date,int n){
    tm Anctm = {};
    stringstream ss(date);
    ss >> get_time(&Anctm, "%Y-%m-%d");

    time_t t=mktime(&Anctm)+n*24*60*60;
    tm* af=localtime(&t);
    //char time[100];
    //sprintf(time,"%lld",af);
    string month,day;
    int m=af->tm_mon+1;
    if (m<10) month="0"+to_string(m);
    else month=to_string(m);
    if (af->tm_mday<10) day="0"+to_string(af->tm_mday);
    else day=to_string(af->tm_mday);
    return to_string(af->tm_year+1900)+"-"+month+"-"+day;//asctime(af);
}

int DateDifference(string st,string ed){
    tm ST = {}; tm ED={};
    stringstream ss(st);
    ss >> get_time(&ST, "%Y-%m-%d");
    stringstream ss2(ed);
    ss2>>get_time(&ED,"%Y-%m-%d");
    time_t t1=mktime(&ST);
    time_t t2=mktime(&ED);
    double difference = std::difftime(t2, t1) / (60 * 60 * 24);
    return int(difference);
}

void GetMaxMin(double& max,double& min,string columnname,sqlite3* db){
    string sql="Select max("+columnname+"),min("+columnname+") from FundamentalData;";
    int rc = 0;
    char* error = NULL;
    char** results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql.c_str(), &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else{
    max=strtod(results[1+1],NULL);
    min=strtod(results[1+2],NULL);
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);}
}

double Normalize(double max,double min,double value){
    return (value-min)/(max-min);
}