//
// Created by jakub on 28/12/18.
//

#ifndef DZIEKANAT_STUDENT_H
#define DZIEKANAT_STUDENT_H

#include "Person.h"
#include "Persistable.h"
#include "Group.h"

class Student : Person, Persistable {
private:
    int album_no;
    Group group;
    void insert_db() override;
    void update_db() override;
    void remove_db() override;
    static Student read_properties_from_input(Student s);
public:
    void print();
    void print_grades();
    int get_album_no();
    static void student_menu();
    static void print_all_students();
    static Student get_by_album(const int &album_no);
    static Student create_new_student_from_input();
};


#endif //DZIEKANAT_STUDENT_H
