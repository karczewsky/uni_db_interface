//
// Created by jakub on 28/12/18.
//

#include <iostream>
#include "Person.h"

Person::Person() {}

Person::Person(const string &first_name, const string &last_name, const string &city, const string &street,
               const string &postal_code, const string &email)
               : first_name(first_name), last_name(last_name), city(city), street(street), postal_code(postal_code),
               email(email) {}