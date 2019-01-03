//
// Created by jakub on 01/01/19.
//

#include <iostream>
#include <pqxx/pqxx>
#include <iomanip>
#include "Module.h"
#include "Db.h"
#include "Utils.h"

void Module::insert_db() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("INSERT INTO module(name, ects)"
                                "VALUES("+txn.quote(this->name)+","+txn.quote(this->ects)+") RETURNING id");
    txn.commit();
    this->id = res[0][0].as<int>();
}

void Module::update_db() {
    pqxx::work txn(Db::connection);
    txn.exec("UPDATE module SET name = " + txn.quote(this->name) +
             " ,ects = " + txn.quote(this->ects) +
             " WHERE id = " + txn.quote(this->id));
    txn.commit();
}

void Module::remove_db() {
    pqxx::work txn(Db::connection);
    txn.exec("DELETE FROM module WHERE id = " + txn.quote(this->id));
    txn.commit();
}

void Module::print() {
    cout
        << "==================" << endl
        << "Modul: " << this->name << endl
        << "Identyfikator: " << this->id << endl
        << "ECTS: " << this->ects << endl
        << "==================" << endl;
}

int Module::get_id() {
    return this->id;
}

Module Module::get_by_id(const int &id) {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("select name, ects"
                                " from module where id = " + txn.quote(id));
    txn.abort();
    if (res.empty()) {
        cout << "Modul o podanym identyfikatorze nie istnieje w bazie." << endl;
        throw "no such module";
    }

    Module m;
    m.id = id;
    m.name = res[0][0].as<string>();
    m.ects = res[0][1].as<int>();
    return m;
}

Module Module::read_properties_from_input(Module m = Module()) {
    Utils::read_validate_length(m.name, "Podaj nazwe modulu: ", "Bledna nazwa modulu.", 40);

    bool err = false;
    do {
        m.ects = Utils::get_num_from_user<int>("Podaj ilosc ECTS: ");
        if (err)
            cout << "Ilosc ECTS nie moze byc niedodatnia" << endl;
        err = m.ects < 0;
    } while (err);
    return m;
}

Module Module::create_new_module_from_input() {
    Module m = Module::read_properties_from_input();
    m.insert_db();
    cout << "Utworzono modul." << endl;
    m.print();
    return m;
}

void Module::print_all_modules() {
    pqxx::work txn(Db::connection);
    pqxx::result res = txn.exec("SELECT id, ects, name FROM module ORDER BY id");

    cout << left << setfill(' ') << setw(13) << "Id";
    cout << left << setfill(' ') << setw(10) << "ECTS";
    cout << left << setfill(' ') << setw(20) << "Nazwa";

    cout << endl;
    for (auto row = res.begin(); row != res.end(); row++) {
        cout << left << setfill(' ') << setw(13) << row[0].c_str();
        cout << left << setfill(' ') << setw(10) << row[1].c_str();
        cout << left << setfill(' ') << setw(20) << row[2].c_str();
        cout << endl;
    }
}

void Module::module_menu() {
    int id = Utils::get_num_from_user<int>("Podaj identyfikator modulu: ");
    Module m;

    try {
        m = Module::get_by_id(id);
    } catch (...) {
        return;
    }

    m.print();
    int choice;
    bool menu_loop = true;
    while (menu_loop) {
        cout << "============== MENU MODULU ==============" << endl;
        cout << "\t0) Wyjdz z systemu" << endl;
        cout << "\t1) Wypisz dane modulu" << endl;
        cout << "\t2) Modyfikuj modul" << endl;
        cout << "\t3) Usun modul" << endl;
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
                m.print();
                break;
            }
            case 2: {
                m = read_properties_from_input(m);
                m.update_db();
                break;
            }
            case 3: {
                m.remove_db();
                cout << "Modul zostal usuniety." << endl;
                menu_loop = false;
                break;
            }
        }
    }
    cout << "Powrot do poprzedniego menu." << endl;
}