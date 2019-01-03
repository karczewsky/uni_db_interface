//
// Created by jakub on 28/12/18.
//

#ifndef DZIEKANAT_UTILS_H
#define DZIEKANAT_UTILS_H

#include <string>
#include <iostream>

using namespace std;

class Utils {
public:
    // true if regex matched
    static bool regexMatch(const string &search, const string &reg_expression);
    // true if read succesfully
    static void readValidateLength(string &destination, const string &stdPrompt, const string &errPrompt,
            const int &maxLen);
    static void readValidateLengthRegex(string &destination, const string &std_prompt, const string &err_prompt,
            const int &max_len, const string &reg_expression);
    static bool isValidMark(const float &i);
    template <class T>
    static T getNumFromUser(const string &stdPrompt, const string &errPrompt = "Podaj poprawny numer.") {
        T num;
        bool err = false;
        do {
            if (err) {
                cout << errPrompt << endl;
            }
            cout << stdPrompt;
            err = !(cin >> num);
            if (cin.fail()) {
                cin.clear();
                cin.ignore(INT32_MAX, '\n');
            }
        } while (err);
        return num;
    };
};

#endif //DZIEKANAT_UTILS_H
