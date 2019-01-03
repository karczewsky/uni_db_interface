//
// Created by jakub on 02/01/19.
//

#ifndef DZIEKANAT_GRADE_H
#define DZIEKANAT_GRADE_H

#include "Student.h"
#include "Teacher.h"
#include "Module.h"

class Grade : Persistable {
private:
    explicit Grade(const float& mark);
    float mark;
    Student student;
    Teacher teacher;
    Module module;
    void insert_db() override;
    void update_db() override;
    void remove_db() override;
public:
    void set_mark(float const &mark);
    static Grade get_by_relations(const int& student_id, const int& teacher_id, const int& module_id);
};


#endif //DZIEKANAT_GRADE_H
