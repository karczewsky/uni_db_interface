//
// Created by jakub on 01/01/19.
//

#include <iostream>
#include <pqxx/pqxx>
#include <iomanip>
#include "Teacher.h"
#include "Db.h"
#include "Utils.h"
#include "Module.h"
#include "Grade.h"

void Teacher::insert_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("INSERT INTO teacher(first_name, last_name, city, street, postal_code, email)"
                                "VALUES("+txn.quote(this->first_name)+","+txn.quote(this->last_name)+","+
                                txn.quote(this->city)+","+txn.quote(this->street)+","+txn.quote(this->postal_code)+","+
                                txn.quote(this->email)+") RETURNING id");
    txn.commit();
    this->id = res[0][0].as<int>();
}

void Teacher::update_db() {
    pqxx::work txn(Db::connection);
    txn.exec("UPDATE teacher SET first_name = " + txn.quote(this->first_name) +
             " ,last_name = " + txn.quote(this->last_name) + " ,city = " + txn.quote(this->city) +
             " ,street = " + txn.quote(this->street) + " ,postal_code = " + txn.quote(this->postal_code) +
             " ,email = "+txn.quote(this->email) +
             " WHERE id = " + txn.quote(this->id));
    txn.commit();
}

void Teacher::remove_db() {
    pqxx::work txn(Db::connection);
    txn.exec("DELETE FROM teacher WHERE id = " +txn.quote(id));
    txn.commit();
}

int Teacher::get_id() {
    return this->id;
}

void Teacher::print() {
    cout
    << "==================" << endl
    << "Prowadzacy : " << this->first_name << " " << this->last_name << endl
    << "Numer identyfikatora: " << this->id << endl
    << "Adres: " << this->street << ", " << this->city << " " << this->postal_code << endl
    << "E-mail: " << this->email << endl
    << "==================" << endl;
}

Teacher Teacher::get_by_id(const int &id) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select first_name, last_name, city, street, postal_code, email"
                                " from teacher where id = " + txn.quote(id));
    txn.abort();
    if (res.empty()) {
        cout << "Prowadzacy o podanym numerze id nie istnieje w bazie." << endl;
        throw "no such teacher";
    }

    Teacher t;
    t.id = id;
    t.first_name = res[0][0].as<string>();
    t.last_name = res[0][1].as<string>();
    t.city = res[0][2].as<string>();
    t.street = res[0][3].as<string>();
    t.postal_code = res[0][4].as<string>();
    t.email = res[0][5].as<string>();
    return t;
}

Teacher Teacher::read_properties_from_input(Teacher t = Teacher()) {
    Utils::readValidateLength(t.first_name, "Podaj imie: ", "Bledne imie.", 40);
    Utils::readValidateLength(t.last_name, "Podaj nazwisko: ", "Bladne nazwisko.", 50);
    Utils::readValidateLength(t.city, "Podaj miasto: ", "Bledne miasto.", 30);
    Utils::readValidateLength(t.street, "Podaj ulice: ", "Bledna ulica.", 50);
    Utils::readValidateLengthRegex(t.postal_code, "Podaj kod: ", "Bledny kod pocztowy.", 6, R"(\b\d{2}-\d{3}\b)");
    Utils::readValidateLengthRegex(t.email, "Podaj email: ", "Bledny email.", 50,
                                   R"(\b[A-Za-z0-9._%-]+@[A-Za-z0-9._%-]+\.[A-Za-z]{2,4}\b)");

    return t;
}

Teacher Teacher::create_new_teacher_from_input() {
    Teacher t = Teacher::read_properties_from_input();
    t.insert_db();
    cout << "Utworzono prowadzacego." << endl;
    t.print();
    return t;
}

void Teacher::print_all_teachers() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select id, first_name || ' ' || last_name, email,"
                                " street || ', ' || city || ' ' || postal_code  from teacher"
                                " order by id;");

    cout << left << setfill(' ') << setw(13) << "Id";
    cout << left << setfill(' ') << setw(30) << "Prowadzacy";
    cout << left << setfill(' ') << setw(20) << "Email";
    cout << left << setfill(' ') << setw(40) << "Adres";
    cout << endl;
    for (auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(13) << row[0].c_str();
        cout << left << setfill(' ') << setw(30) << row[1].c_str();
        cout << left << setfill(' ') << setw(20) << row[2].c_str();
        cout << left << setfill(' ') << setw(40) << row[3].c_str();
        cout << endl;
    }
}

void Teacher::print_qualifications() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select m.id, m.ects, m.name from teacher t "
                                "join qualified q on t.id = q.teacher_id "
                                "join module m on m.id = q.module_id "
                                "where t.id = " + txn.quote(this->id));

    cout << left << setfill(' ') << setw(15) << "Id modulu";
    cout << left << setfill(' ') << setw(15) << "ECTS modulu";
    cout << left << setfill(' ') << setw(20) << "Nazwa modulu";
    cout << endl;
    for (auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(15) << row[0].c_str();
        cout << left << setfill(' ') << setw(15) << row[1].c_str();
        cout << left << setfill(' ') << setw(20) << row[2].c_str();
        cout << endl;
    }
}

void Teacher::add_qualification() {
    int id = Utils::getNumFromUser<int>("Podaj id przedmiotu, ktory moze prowadzic prowadzacy: ");
    Module m;

    try {
        m = Module::get_by_id(id);
    } catch (...) {
        return;
    }

    pqxx::work txn(Db::connection);
    try {
        txn.exec("INSERT INTO qualified(teacher_id, module_id) "
                 "VALUES(" + txn.quote(this->id) + "," + txn.quote(m.get_id()) + ")");
        txn.commit();
    } catch (pqxx::unique_violation&) {
        cout << "Prowadzacy juz posiada kwalifikacje do podanego przedmiotu." << endl;
        return;
    }

    cout << "Dodana kwalifikacja: " << endl;
    m.print();
}

void Teacher::remove_qualification() {
    int id = Utils::getNumFromUser<int>("Podaj id przedmiotu, ktory prowadzacy nie bedzie juz uczyl: ");
    Module m;

    try {
        m = Module::get_by_id(id);
    } catch (...) {
        return;
    }

    pqxx::work txn(Db::connection);
    txn.exec("DELETE FROM qualified "
             "WHERE teacher_id="+txn.quote(this->id)+" AND module_id="+txn.quote(m.get_id()));
    txn.commit();
    cout << "Usunieto kwalifikacje: " << endl;
    m.print();
}

void Teacher::teacher_menu() {
    int id = Utils::getNumFromUser<int>("Podaj identyfikator prowadzacego: ");
    Teacher t;

    try {
        t = Teacher::get_by_id(id);
    } catch (...) {
        return;
    }

    t.print();
    int choice;
    bool menu_loop = true;
    while (menu_loop) {
        cout << "============== MENU PROWADZACEGO ==============" << endl;
        cout << "\t0) Wyjdz z systemu" << endl;
        cout << "\t1) Wypisz dane prowadzacego" << endl;
        cout << "\t2) Modyfikuj prowadzacego" << endl;
        cout << "\t3) Usun prowadzacego" << endl;
        cout << "\t4) Wypisz kwalifikacje prowadzacego" << endl;
        cout << "\t5) Dodaj kwalifikacje prowadzacego" << endl;
        cout << "\t6) Usun kwalifikacje prowadzacego" << endl;
        cout << "\t7) Wystawianie ocen" << endl;
        cout << "Wybierz opcje: ";

        cin >> choice;
        if(cin.fail()) {
            choice = -1;
            cin.clear();
        }
        cin.ignore(INT32_MAX, '\n');

        switch(choice) {
            default: {
                cout << "Bledna opcja!" << endl;
                break;
            }
            case 0: {
                menu_loop = false;
                break;
            }
            case 1: {
                t.print();
                break;
            }
            case 2: {
                t = read_properties_from_input(t);
                t.update_db();
                break;
            }
            case 3: {
                t.remove_db();
                cout << "Prowadzacy zostal usuniety." << endl;
                menu_loop = false;
                break;
            }
            case 4: {
                t.print_qualifications();
                break;
            }
            case 5: {
                t.add_qualification();
                break;
            }
            case 6: {
                t.remove_qualification();
                break;
            }
            case 7: {
                t.grades_menu();
                break;
            }
        }
    }
    cout << "Powrot do poprzedniego menu." << endl;
}

void Teacher::grades_menu() {
    cout << "===== WYSTAWIANIE OCEN =====" << endl;
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select g.module_id, g.student_id, s.last_name || ' ' || s.first_name, m.name, g.mark from grade g "
                                "join student s on g.student_id = s.album_no "
                                "join module m on g.module_id = m.id "
                                "where g.teacher_id = "+txn.quote(this->id));
    txn.abort();

    cout << left << setfill(' ') << setw(15) << "Id modulu";
    cout << left << setfill(' ') << setw(15) << "Nr albumu";
    cout << left << setfill(' ') << setw(25) << "Student";
    cout << left << setfill(' ') << setw(35) << "Przedmiot";
    cout << left << setfill(' ') << setw(15) << "Ocena";
    cout << endl;
    for (auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(15) << row[0].c_str();
        cout << left << setfill(' ') << setw(15) << row[1].c_str();
        cout << left << setfill(' ') << setw(25) << row[2].c_str();
        cout << left << setfill(' ') << setw(35) << row[3].c_str();
        cout << left << setfill(' ') << setw(15) << row[4].c_str();
        cout << endl;
    }

    int module_id = Utils::getNumFromUser<int>("Podaj nr modulu: ");
    int album_no = Utils::getNumFromUser<int>("Podaj nr albumu: ");
    float mark;

    bool err = false;
    do {
        mark = Utils::getNumFromUser<float>("Podaj ocene: ");
        err = !Utils::isValidMark(mark);
        if (err)
            cout << "Zla wartosc oceny." << endl;
    } while (err);

    try {
        Grade grade = Grade::get_by_relations(album_no, this->id, module_id);
        grade.set_mark(mark);
    } catch (...) {
        cout << "Blad przy wystawianiu ocen" << endl;
    }
}
