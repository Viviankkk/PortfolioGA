//
// Created by Vivian Kang on 6/7/20.
//
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include "database.h"
using namespace std;

int OpenDatabase(const char* name, sqlite3*& db)
{
    int rc = 0;
    // Open Database
    cout << "Opening database: " << name << endl;
    rc = sqlite3_open(name, &db);
    if (rc)
    {
        cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return -1;
    }
    cout << "Opened database: " << name << endl;
    return 0;

}

void CloseDatabase(sqlite3* db)
{
    cout << "Closing a database ..." << endl;
    sqlite3_close(db);
    cout << "Closed a database" << endl << endl;
}

int DropTable(const char* sql_drop_table, sqlite3* db)
{
    // Drop the table if exists
    if (sqlite3_exec(db, sql_drop_table, 0, 0, 0) != SQLITE_OK) { // or == -- same effect
        std::cout << "SQLite can't drop sessions table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    return 0;
}

int CreateTable(const char* sql_create_table, sqlite3* db)
{
    int rc = 0;
    char* error = NULL;
    // Create the table
    cout << "Creating a table..." << endl;
    rc = sqlite3_exec(db, sql_create_table, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    cout << "Created a table." << endl;
    return 0;
}

int InsertTable(const char* sql_insert, sqlite3* db)
{
    int rc = 0;
    char* error = NULL;
    // Execute SQL
    //cout << "Inserting a value into a table ..." << endl;
    //cout<<sql_insert<<endl;
    rc = sqlite3_exec(db, sql_insert, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    //cout << "Inserted a value into the table." << endl;
    return 0;
}

int DisplayTable(const char* sql_select, sqlite3* db)
{
    int rc = 0;
    char* error = NULL;

    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char** results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }

    // Display Table
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        for (int colCtr = 0; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;

            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
        }

        // End Line
        cout << endl;

        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}

