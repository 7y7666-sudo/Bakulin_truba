#include <iostream>
using namespace std;

struct Truba {
    string name;
    float length;
    float diametr;
    bool on_fix = false;
};

struct CS {
    string name;
    int workshop;
    int workshop_on_work;
    char rank;
};

Truba add_truba() {
    Truba t;
    int ok;
start:
    cout << "Enter Truba parameters(name,length,diametr):";
    cin >> t.name >> t.length >> t.diametr;
    cout << t.name << endl << t.length << endl << t.diametr << endl << "Press 1 to continue \nPress 0 to type parametres agian";
    cin >> ok;
    if (ok == 0) { goto start; };
    return t;
};

CS add_CS() {
    CS q;
    int ok;
start:
    cout << "Enter CS parameters(name,workshop,workshop_on_work,rank):";
    cin >> q.name >> q.workshop >> q.workshop_on_work >> q.rank;
    cout << q.name << endl << q.workshop << endl << q.workshop_on_work << endl << q.rank << endl << "Press 1 to continue \nPress 0 to type parametres agian";
    cin >> ok;
    if (ok == 0) { goto start; };
    return q;
};
void all() {
    //cout << q << t;
}
void menu() {
    int x;
    cin >> x;
    switch (x) {
    case 0: { exit(0);break; }

    case 1: { Truba t = add_truba(); cout << t.name << t.length; break; }

    case 2: { CS q = add_CS();cout << q.name; break; }

    case 3: { cout << q.name; break; }
    }
}

int main() {
    cout << "Menu:\n1. Add Truba\n2. Add CS\n3. Show all objects\n4. Edit Truba\n5. Edit CS\n6. Save\n7. Load\n0. Exit\n";
    while (true) {
        menu();
    }
    //T_par();
}