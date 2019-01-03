//
// Created by jakub on 01/01/19.
//

#ifndef DZIEKANAT_MODULE_H
#define DZIEKANAT_MODULE_H

#include <string>
#include "Persistable.h"

using namespace std;

class Module : Persistable {
private:
    int id;
    string name;
    int ects;
    void insert_db() override;
    void update_db() override;
    void remove_db() override;
    static Module read_properties_from_input(Module m);
public:
    int get_id();
    void print();
    static void module_menu();
    static void print_all_modules();
    static Module get_by_id(const int &id);
    static Module create_new_module_from_input();
};


#endif //DZIEKANAT_MODULE_H
