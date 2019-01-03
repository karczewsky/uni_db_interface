//
// Created by jakub on 28/12/18.
//

#ifndef DZIEKANAT_PERSON_H
#define DZIEKANAT_PERSON_H

#include <string>

using namespace std;

class Person {
protected:
    string first_name;
    string last_name;
    string city;
    string street;
    string postal_code;
    string email;
    Person();
    Person(const string &first_name, const string &last_name, const string &city, const string &street,
           const string &postal_code, const string &email);
};


#endif //DZIEKANAT_PERSON_H
