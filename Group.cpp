//
// Created by jakub on 30/12/18.
//

#include "Group.h"
#include "Db.h"
#include "Utils.h"
#include <pqxx/pqxx>
#include <iostream>
#include <iomanip>

void Group::insert_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("INSERT INTO course_group(name)"
                                "VALUES("+txn.quote(this->name)+") RETURNING id");
    txn.commit();
    this->id = res[0][0].as<int>();
}

void Group::update_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("UPDATE course_group SET name="+txn.quote(this->name)+
            " WHERE id="+txn.quote(this->id));
    txn.commit();
}

void Group::print() {
    cout
            << "==================" << endl
            << "Id: " << this->id << endl
            << "Nazwa: " << this->name << endl
            << "==================" << endl;
}

Group Group::read_properties_from_input(Group g = Group()) {
    Utils::readValidateLength(g.name, "Podaj nazwe grupy: ", "Nieprawidlowa nazwa grupy.", 10);
    return g;
}

void Group::create_new_group_from_input() {
    Group g = Group::read_properties_from_input();
    try {
        g.insert_db();
    } catch (pqxx::unique_violation&) {
        cout << "Podana grupa juz istnieje!" << endl;
        return;
    }
    cout << "Utworzono grupe: " << endl;
    g.print();
}

Group Group::get_by_name(const string &name) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select id, name from course_group where lower(name) like lower('%" + name + "%')");
    txn.abort();
    if(res.empty()) {
        cout << "Podana grupa nie istnieje!" << endl;
        throw "Doesn't exist";
    } else if(res.size() > 1) {
        cout << "Istnieje wiele grup o pasujacej nazwie!" << endl;
        throw "Too many received";
    }

    Group g;
    g.id = res[0][0].as<int>();
    g.name = res[0][1].as<string>();
    return g;
}

Group Group::get_by_id(const int &id) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select name from course_group where id = " + txn.quote(id));
    txn.abort();
    if (res.empty()) {
        cout << "Grupa o podanym numerze id nie istnieje w bazie" << endl;
        throw "No such group";
    }
    Group g;
    g.id = id;
    g.name = res[0][0].as<string>();
    return g;
}

void Group::modify_group() {
    (*this) = Group::read_properties_from_input(*this);
    try {
        this->update_db();
    } catch (pqxx::unique_violation&) {
        cout << "Podana nazwa grupy juz wystepuje w bazie. Modyfikacja anulowana." << endl;
        return;
    }
    cout << "Dana zaktualizowane:" << endl;
    this->print();
}

void Group::remove_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("delete from course_group where id="+txn.quote(this->id));
    txn.commit();
}

void Group::generate_links() {
    pqxx::work txn(Db::connection);
    txn.exec("delete from grade "
             "where student_id in ( "
             "  select album_no from student "
             "  where group_id = " + txn.quote(this->id) +
             "  )");
    txn.exec("insert into grade(student_id, teacher_id, module_id) ( "
               "  select s.album_no, gm.teacher_id, gm.module_id "
               "  from group_module gm join student s on s.group_id = gm.group_id "
               "  where gm.group_id = " + txn.quote(this->id) +
               "  )");
    txn.commit();
    cout << "Podpiecia dla grupy wygenerowane." << endl;
}

void Group::print_students() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select s.album_no, s.last_name ||  ' ' || s.first_name from course_group "
                                "join student s on course_group.id = s.group_id "
                                "where s.group_id = "+txn.quote(this->id));
    cout << left << setfill(' ') << setw(15) << "Numer albumu";
    cout << left << setfill(' ') << setw(30) << "Student";
    cout << endl;

    for(auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(15) << row[0].c_str();
        cout << left << setfill(' ') << setw(30) << row[1].c_str();
        cout << endl;
    }
}

void Group::print_group_modules() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select m.name, m.ects, t.last_name || ' ' || t.first_name  from course_group "
                                "join group_module gm on course_group.id = gm.group_id "
                                "join qualified q on gm.teacher_id = q.teacher_id and gm.module_id = q.module_id "
                                "join teacher t on q.teacher_id = t.id "
                                "join module m on q.module_id = m.id "
                                "where course_group.id = "+txn.quote(this->id));
    cout << left << setfill(' ') << setw(40) << "Przedmiot";
    cout << left << setfill(' ') << setw(7) << "ECTS";
    cout << left << setfill(' ') << setw(30) << "Prowadzacy";
    cout << endl;

    for(auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(40) << row[0].c_str();
        cout << left << setfill(' ') << setw(7) << row[1].c_str();
        cout << left << setfill(' ') << setw(30) << row[2].c_str();
        cout << endl;
    }
}


void Group::add_group_module() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select m.id, t.id, t.last_name || ' ' || t.first_name, m.name "
                                "from module m "
                                "join qualified q on m.id = q.module_id "
                                "join teacher t on q.teacher_id = t.id");

    cout << left << setfill(' ') << setw(14) << "Id modulu";
    cout << left << setfill(' ') << setw(17) << "Id prowadzacego";
    cout << left << setfill(' ') << setw(30) << "Prowadzacy";
    cout << left << setfill(' ') << setw(40) << "Modul";

    cout << endl;
    for(auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(14) << row[0].c_str();
        cout << left << setfill(' ') << setw(17) << row[1].c_str();
        cout << left << setfill(' ') << setw(30) << row[2].c_str();
        cout << left << setfill(' ') << setw(40) << row[3].c_str();
        cout << endl;
    }
    cout << endl;

    int teacher_id = Utils::getNumFromUser<int>("Podaj id prowadzacego: ");
    int module_id = Utils::getNumFromUser<int>("Podaj id przedmiotu: ");

    res = txn.exec("SELECT teacher_id, module_id FROM qualified "
                       "WHERE teacher_id=" + txn.quote(teacher_id) +
                       " AND module_id=" + txn.quote(module_id));

    if (res.empty()) {
        cout << "Niepoprawny prowadzacy/przedmiot" << endl;
        return;
    }

    try {
        res = txn.exec("insert into group_module(group_id, teacher_id, module_id) "
                       "VALUES(" + txn.quote(this->id) + "," + txn.quote(teacher_id) + "," + txn.quote(module_id) +
                       ")");
        txn.commit();
    } catch (pqxx::unique_violation&) {
        cout << "Podany przedmiot zostal juz dodany" << endl;
        return;
    }

    cout << "Przedmiot dodany" << endl;
}

void Group::remove_group_module() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select teacher_id, module_id, "
                                "t.last_name || ' ' || t.first_name, m.name from group_module gm "
                                "join teacher t on t.id = gm.teacher_id "
                                "join module m on m.id = gm.module_id;");

    cout << left << setfill(' ') << setw(14) << "Id modulu";
    cout << left << setfill(' ') << setw(17) << "Id prowadzacego";
    cout << left << setfill(' ') << setw(30) << "Prowadzacy";
    cout << left << setfill(' ') << setw(40) << "Modul";

    cout << endl;
    for(auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(14) << row[0].c_str();
        cout << left << setfill(' ') << setw(17) << row[1].c_str();
        cout << left << setfill(' ') << setw(30) << row[2].c_str();
        cout << left << setfill(' ') << setw(40) << row[3].c_str();
        cout << endl;
    }
    cout << endl;

    int teacher_id = Utils::getNumFromUser<int>("Podaj id prowadzacego: ");
    int module_id = Utils::getNumFromUser<int>("Podaj id przedmiotu: ");
    res = txn.exec("SELECT * FROM group_module "
                   "WHERE teacher_id=" + txn.quote(teacher_id) +
                   " AND module_id=" + txn.quote(module_id)+" AND group_id="+txn.quote(this->id));
    if (res.empty()) {
        cout << "Niepoprawny prowadzacy/modul" << endl;
        return;
    }

    txn.exec("DELETE FROM group_module "
             "WHERE teacher_id="+ txn.quote(teacher_id) +
             " AND module_id=" + txn.quote(module_id) +
             " AND group_id="+txn.quote(this->id));
    txn.commit();
    cout << "Przedmiot usuniety" << endl;
}


void Group::print_all_groups() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("SELECT id, name FROM course_group");
    cout << left << setfill(' ') << setw(5) << "Id";
    cout << left << setfill(' ') << setw(12) << "Nazwa";
    cout << endl;

    for(auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(5) << row[0].c_str();
        cout << left << setfill(' ') << setw(12) << row[1].c_str();
        cout << endl;
    }
}

int Group::getId() {
    return this->id;
}

string Group::getName() {
    return this->name;
}

void Group::group_menu() {
    string name;
    cout << "Podaj nazwe grupy, ktora chcesz zarzadzac: ";
    cin >> name;
    cin.ignore(INT32_MAX, '\n');
    Group g;
    try {
        g = Group::get_by_name(name);
    } catch (...) {
        cout << "Powrot do poprzedniego menu." << endl;
        return;
    }
    g.print();
    int choice;
    bool menu_loop = true;
    while (menu_loop) {
        cout << "============== MENU GRUPY ==============" << endl;
        cout << "\t0) Wyjdz z systemu" << endl;
        cout << "\t1) Modyfikuj grupe" << endl;
        cout << "\t2) Usun grupe" << endl;
        cout << "\t3) Wypisz studentow" << endl;
        cout << "\t4) Wypisz przedmioty grupy" << endl;
        cout << "\t5) Dodaj przedmiot do zajec grupy" << endl;
        cout << "\t6) Usun przedmiot grupie" << endl;
        cout << "\t7) Generuj podpiecia" << endl;
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
                g.modify_group();
                break;
            }
            case 2: {
                g.remove_db();
                cout << "Grupa zostala usunieta, powrot do poprzedniego menu." << endl;
                menu_loop = false;
                break;
            }
            case 3: {
                g.print_students();
                break;
            }
            case 4: {
                g.print_group_modules();
                break;
            }
            case 5: {
                g.add_group_module();
                break;
            }
            case 6: {
                g.remove_group_module();
                break;
            }
            case 7: {
                g.generate_links();
                break;
            }
        }
    }
    cout << "Powrot do poprzedniego menu" << endl;
}