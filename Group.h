//
// Created by jakub on 30/12/18.
//

#ifndef DZIEKANAT_GROUP_H
#define DZIEKANAT_GROUP_H

#include <string>
#include "Persistable.h"

using namespace std;

class Group : Persistable {
private:
    int id;
    string name;
    void insert_db() override;
    void update_db() override;
    void remove_db() override;
    void modify_group();
    void add_group_module();
    void remove_group_module();
    void generate_links();
    static Group read_properties_from_input(Group g);
public:
    void print();
    void print_students();
    void print_group_modules();
    static Group get_by_name(const string &name);
    static Group get_by_id(const int &id);
    static void create_new_group_from_input();
    static void group_menu();
    static void print_all_groups();
    int get_id();
    string get_name();
};


#endif //DZIEKANAT_GROUP_H
