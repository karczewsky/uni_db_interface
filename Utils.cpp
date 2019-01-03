//
// Created by jakub on 28/12/18.
//

#include <iostream>
#include <limits>
#include <regex>
#include "Utils.h"

using namespace std;

// Returns true if pattern was found in string
bool Utils::regex_match(const string &search, const string &reg_expression) {
    regex reg(reg_expression);
    return regex_search(search, reg);
}

void Utils::read_validate_length(string &destination, const string &stdPrompt,
                                 const string &errPrompt, const int &maxLen) {
    bool error = false;
    do {
        if (error)
            cout << errPrompt << endl;
        cout << stdPrompt;
        getline(cin, destination);
        error = destination.length() == 0 || destination.length() > maxLen;
    } while (error);
}

void Utils::read_validate_length_regex(string &destination, const string &std_prompt, const string &err_prompt,
                                       const int &max_len, const string &reg_expression) {
    bool err = false;
    do {
        if (err)
            cout << err_prompt << endl;
        Utils::read_validate_length(destination, std_prompt, err_prompt, max_len);
        err = !Utils::regex_match(destination, reg_expression);
    } while (err);

}

bool Utils::is_valid_mark(const float &i) {
    return i == 2 || i == 3 || i == 3.5 || i == 4 || i == 4.5 || i == 5;
}