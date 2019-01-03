//
// Created by jakub on 29/12/18.
//

#ifndef DZIEKANAT_DB_H
#define DZIEKANAT_DB_H

#include <string>
#include <pqxx/pqxx>

using namespace std;

class Db {
public:
    static string connection_string;
    static pqxx::connection connection;
};


#endif //DZIEKANAT_DB_H
