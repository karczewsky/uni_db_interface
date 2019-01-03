//
// Created by jakub on 01/01/19.
//

#ifndef DZIEKANAT_TEACHER_H
#define DZIEKANAT_TEACHER_H

#include "Person.h"
#include "Persistable.h"


class Teacher : Person, Persistable{
private:
    int id;
    void insert_db() override;
    void update_db() override;
    void remove_db() override;
    void add_qualification();
    void remove_qualification();
    void grades_menu();
    static Teacher read_properties_from_input(Teacher t);
public:
    void print();
    void print_qualifications();
    int get_id();
    static void teacher_menu();
    static void print_all_teachers();
    static Teacher get_by_id(const int &id);
    static Teacher create_new_teacher_from_input();
};


#endif //DZIEKANAT_TEACHER_H
