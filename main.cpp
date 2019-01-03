#include <iostream>

#include "Student.h"
#include "Group.h"
#include "Teacher.h"
#include "Utils.h"
#include "Module.h"

using namespace std;

int main() {
    cout << "Witaj w systemie bankowym." << endl;
    int choice;
    bool menu_loop = true;
    while (menu_loop) {
        cout << "============== MENU ==============" << endl;
        cout << "\t0) Wyjdz z systemu" << endl;
        cout << "\t ==== GRUPY ====" << endl;
        cout << "\t1) Wypisz wszystkie grupy" << endl;
        cout << "\t2) Zaloz nowa grupe" << endl;
        cout << "\t3) Przejdz do zarzadzania grupami" << endl;
        cout << "\t ==== STUDENCI ====" << endl;
        cout << "\t4) Wypisz wszystkich studentow" << endl;
        cout << "\t5) Dodaj nowego studenta do grupy." << endl;
        cout << "\t6) Przejdz do zarzadzania studentami" << endl;
        cout << "\t ==== PROWADZACY ====" << endl;
        cout << "\t7) Wypisz wszystkich prowadzacych" << endl;
        cout << "\t8) Dodaj nowego prowadzacego" << endl;
        cout << "\t9) Przejdz do zarzadania prowadzacymi" << endl;
        cout << "\t ==== MODULY ====" << endl;
        cout << "\t10) Wypisz wszystkie moduly" << endl;
        cout << "\t11) Dodaj nowy modul" << endl;
        cout << "\t12) Przejdz do zarzadania modulami" << endl;

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
                cout << "Zamykanie programu" << endl;
                menu_loop = false;
                break;
            }
            case 1: {
                Group::print_all_groups();
                break;
            }
            case 2: {
                cout << "Zakladanie nowej grupy rozpoczete." << endl;
                Group::create_new_group_from_input();
                break;
            }
            case 3: {
                Group::group_menu();
                break;
            }
            case 4: {
                Student::print_all_students();
                break;
            }
            case 5: {
                Student::create_new_student_from_input();
                break;
            }
            case 6: {
                Student::student_menu();
                break;
            }
            case 7: {
                Teacher::print_all_teachers();
                break;
            }
            case 8: {
                Teacher::create_new_teacher_from_input();
                break;
            }
            case 9: {
                Teacher::teacher_menu();
                break;
            }
            case 10: {
                Module::print_all_modules();
                break;
            }
            case 11: {
                Module::create_new_module_from_input();
                break;
            }
            case 12: {
                Module::module_menu();
                break;
            }
        }
    }
    return 0;
}