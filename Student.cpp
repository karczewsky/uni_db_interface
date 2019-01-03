//
// Created by jakub on 28/12/18.
//

#include <iostream>
#include <pqxx/pqxx>
#include <iomanip>
#include "Student.h"
#include "Group.h"
#include "Utils.h"
#include "Db.h"

void Student::insert_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("INSERT INTO student(first_name, last_name, city, street, postal_code, email, group_id)"
                                "VALUES("+txn.quote(this->first_name)+","+txn.quote(this->last_name)+","+
                                txn.quote(this->city)+","+txn.quote(this->street)+","+txn.quote(this->postal_code)+","+
                                txn.quote(this->email)+","+txn.quote(this->group.getId())+") RETURNING album_no");
    txn.commit();
    this->album_no = res[0][0].as<int>();
}

void Student::update_db() {
    pqxx::work txn(Db::connection);
    txn.exec("UPDATE student SET first_name = " + txn.quote(this->first_name) +
             " ,last_name = " + txn.quote(this->last_name) + " ,city = " + txn.quote(this->city) +
             " ,street = " + txn.quote(this->street) + " ,postal_code = " + txn.quote(this->postal_code) +
             " ,email = "+txn.quote(this->email) + " ,group_id = " + txn.quote(this->group.getId()) +
             " WHERE album_no = " + txn.quote(this->album_no));
    txn.commit();
}

void Student::remove_db() {
    pqxx::work txn(Db::connection);
    txn.exec("DELETE FROM student WHERE album_no = " + txn.quote(album_no));
    txn.commit();
}


int Student::get_album_no() {
    return this->album_no;
}


void Student::print() {
    cout
    << "==================" << endl
    << "Student: " << this->first_name << " " << this->last_name << endl
    << "Grupa: " << this->group.getName() << endl
    << "Numer albumu: " << this->album_no << endl
    << "Adres: " << this->street << ", " << this->city << " " << this->postal_code << endl
    << "E-mail: " << this->email << endl
    << "==================" << endl;
}

void Student::print_grades() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select m.id, t.id, m.name, m.ects, t.last_name || ' ' || t.first_name, coalesce(mark, 0), g.mark "
                                "from grade g "
                                "join teacher t on g.teacher_id = t.id "
                                "join module m on g.module_id = m.id "
                                "where g.student_id = " + txn.quote(this->album_no));

    cout << left << setfill(' ') << setw(14) << "Id modulu";
    cout << left << setfill(' ') << setw(16) << "Id prowadzacego";
    cout << left << setfill(' ') << setw(30) << "Modul";
    cout << left << setfill(' ') << setw(10) << "ECTS";
    cout << left << setfill(' ') << setw(30) << "Prowadzacy";
    cout << left << setfill(' ') << setw(10) << "Ocena";
    cout << endl;

    int deficit_sum = 0;
    for(auto row = res.begin(); row != res.end(); row++) {
        auto ects_module = row[3].as<int>();
        auto grade_module = row[5].as<float>();
        deficit_sum = (grade_module < 3) ? deficit_sum + ects_module : deficit_sum;


        cout << left << setfill(' ') << setw(14) << row[0].c_str();
        cout << left << setfill(' ') << setw(16) << row[1].c_str();
        cout << left << setfill(' ') << setw(30) << row[2].c_str();
        cout << left << setfill(' ') << setw(10) << row[3].c_str();
        cout << left << setfill(' ') << setw(30) << row[4].c_str();

        stringstream ss;
        if (grade_module == 0) {
            ss << "-";
        } else {
            ss << fixed << setprecision(1) << grade_module;
        }
        cout << left << setfill(' ') << setw(10) << ss.str();
        cout << endl;
    }
    cout << endl;
    cout << "Obecny deficyt ECTS: " << deficit_sum << endl;
    if (deficit_sum > 15) {
        cout << "Student zagrozony wydaleniem!" << endl;
    }
}

Student Student::get_by_album(const int &album_no) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select first_name, last_name, city, street, postal_code, email, group_id"
                                " from student where album_no = " + txn.quote(album_no));
    txn.abort();
    if (res.empty()) {
        cout << "Student o podanym numerze albumu nie istnieje w bazie." << endl;
        throw "no such student";
    }

    Student s;
    s.album_no = album_no;
    s.first_name = res[0][0].as<string>();
    s.last_name = res[0][1].as<string>();
    s.city = res[0][2].as<string>();
    s.street = res[0][3].as<string>();
    s.postal_code = res[0][4].as<string>();
    s.email = res[0][5].as<string>();
    s.group = Group::get_by_id(res[0][6].as<int>());
    return s;
}

Student Student::read_properties_from_input(Student s = Student()) {
    string group_s;
    bool loop = true;
    while (loop) {
        try {
            Utils::readValidateLength(group_s, "Podaj nazwe grupy do ktorej nalezy student: ",
                    "Niepoprana nazwa grupy: ", 10);
            s.group = Group::get_by_name(group_s);
            loop = false;
        } catch(...) {
            cout << "Blad przy wyszukiwaniu grupy." << endl;
        }
    }

    Utils::readValidateLength(s.first_name, "Podaj imie: ", "Bledne imie.", 40);
    Utils::readValidateLength(s.last_name, "Podaj nazwisko: ", "Bladne nazwisko.", 50);
    Utils::readValidateLength(s.city, "Podaj miasto: ", "Bledne miasto.", 30);
    Utils::readValidateLength(s.street, "Podaj ulice: ", "Bledna ulica.", 50);
    Utils::readValidateLengthRegex(s.postal_code, "Podaj kod: ", "Bledny kod pocztowy.", 6, R"(\b\d{2}-\d{3}\b)");
    Utils::readValidateLengthRegex(s.email, "Podaj email: ", "Bledny email.", 50,
            R"(\b[A-Za-z0-9._%-]+@[A-Za-z0-9._%-]+\.[A-Za-z]{2,4}\b)");

    return s;
}

Student Student::create_new_student_from_input() {
    Student s = Student::read_properties_from_input();
    s.insert_db();
    cout << "Utworzono studenta." << endl;
    s.print();
    return s;
}

void Student::print_all_students() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select album_no, course_group.name, first_name || ' ' || last_name, email,"
                                " street || ', ' || city || ' ' || postal_code  from student"
                                " join course_group on course_group.id = student.group_id order by album_no;");

    cout << left << setfill(' ') << setw(13) << "Nr albumu";
    cout << left << setfill(' ') << setw(10) << "Grupa";
    cout << left << setfill(' ') << setw(30) << "Student";
    cout << left << setfill(' ') << setw(20) << "Email";
    cout << left << setfill(' ') << setw(40) << "Adres";
    cout << endl;
    for (auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(13) << row[0].c_str();
        cout << left << setfill(' ') << setw(10) << row[1].c_str();
        cout << left << setfill(' ') << setw(30) << row[2].c_str();
        cout << left << setfill(' ') << setw(20) << row[3].c_str();
        cout << left << setfill(' ') << setw(40) << row[4].c_str();
        cout << endl;
    }
}

void Student::student_menu() {
    int album_no = Utils::getNumFromUser<int>("Podaj nr albumu studenta: ");
    Student s;

    try {
        s = Student::get_by_album(album_no);
    } catch (...) {
        return;
    }

    s.print();
    int choice;
    bool menu_loop = true;
    while (menu_loop) {
        cout << "============== MENU GRUPY ==============" << endl;
        cout << "\t0) Wyjdz z systemu" << endl;
        cout << "\t1) Wypisz dane studenta" << endl;
        cout << "\t2) Modyfikuj studenta" << endl;
        cout << "\t3) Wypisz oceny, status studenta" << endl;
        cout << "\t4) Usun studenta" << endl;
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
                s.print();
                break;
            }
            case 2: {
                s = read_properties_from_input(s);
                s.update_db();
                break;
            }
            case 3: {
                s.print_grades();
                break;
            }
            case 4: {
                s.remove_db();
                cout << "Student zostal usuniety." << endl;
                menu_loop = false;
                break;
            }
        }
    }
    cout << "Powrot do poprzedniego menu." << endl;
}
