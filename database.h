//
// Created by Vivian Kang on 6/7/20.
//

#ifndef PORTFOLIOGA_DATABASE_H
#define PORTFOLIOGA_DATABASE_H
#include <sqlite3.h>
int OpenDatabase(const char* name, sqlite3*& db);
void CloseDatabase(sqlite3* db);
int DropTable(const char* sql_drop_table, sqlite3* db);
int CreateTable(const char* sql_create_table, sqlite3* db);
int InsertTable(const char* sql_insert, sqlite3* db);
int DisplayTable(const char* sql_select, sqlite3* db);
#endif //PORTFOLIOGA_DATABASE_H
