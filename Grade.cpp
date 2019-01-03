//
// Created by jakub on 02/01/19.
//

#include <iostream>
#include <pqxx/pqxx>
#include "Grade.h"
#include "Db.h"

void Grade::insert_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("INSERT INTO grade(mark, student_id, teacher_id, module_id) "
                                "VALUES("+txn.quote(this->mark)+","+txn.quote(this->student.get_album_no())+","+
                                txn.quote(this->teacher.get_id())+","+txn.quote(this->module.get_id())+")");
    txn.commit();
}

void Grade::remove_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("DELETE FROM grade WHERE student_id="+txn.quote(this->student.get_album_no())+
            " AND teacher_id="+txn.quote(this->teacher.get_id())+" AND module_id="+txn.quote(this->module.get_id()));
    txn.commit();
}

void Grade::update_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("UPDATE grade SET mark="+txn.quote(this->mark)+
            " WHERE student_id="+txn.quote(this->student.get_album_no())+
            " AND teacher_id="+txn.quote(this->teacher.get_id())+" AND module_id="+txn.quote(this->module.get_id()));
    txn.commit();
}

Grade Grade::get_by_relations(const int &student_id, const int &teacher_id, const int &module_id) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("SELECT coalesce(mark, 0) FROM grade "
                                "WHERE student_id="+txn.quote(student_id)+" AND teacher_id="+txn.quote(teacher_id)+
                                " AND module_id="+txn.quote(module_id));
    if (res.empty()) {
        cout << "Podana ocena nie istnieje w bazie danych" << endl;
        throw "no such mark";
    }
    txn.abort();

    Grade g = Grade(res[0][0].as<float>());
    g.student = Student::get_by_album(student_id);
    g.teacher = Teacher::get_by_id(teacher_id);
    g.module = Module::get_by_id(module_id);
    return g;
}

Grade::Grade(const float &mark) {
    this->mark = mark;
}

void Grade::set_mark(float const &mark) {
    this->mark = mark;
    this->update_db();
}
