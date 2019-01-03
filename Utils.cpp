//
// Created by jakub on 28/12/18.
//

#include <iostream>
#include <limits>
#include <regex>
#include "Utils.h"

using namespace std;

// Returns true if pattern was found in string
bool Utils::regexMatch(const string &search, const string &reg_expression) {
    regex reg(reg_expression);
    return regex_search(search, reg);
}

void Utils::readValidateLength(string &destination, const string &stdPrompt,
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

void Utils::readValidateLengthRegex(string &destination, const string &std_prompt, const string &err_prompt,
        const int &max_len, const string &reg_expression) {
    bool err = false;
    do {
        if (err)
            cout << err_prompt << endl;
        Utils::readValidateLength(destination, std_prompt, err_prompt, max_len);
        err = !Utils::regexMatch(destination, reg_expression);
    } while (err);

}

bool Utils::isValidMark(const float &i) {
    return i == 2 || i == 3 || i == 3.5 || i == 4 || i == 4.5 || i == 5;
}