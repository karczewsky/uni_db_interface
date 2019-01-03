//
// Created by jakub on 29/12/18.
//

#include "Db.h"

string Db::connection_string = "user=postgres password=postgres host=localhost port=6969 dbname=postgres";
pqxx::connection Db::connection(Db::connection_string);