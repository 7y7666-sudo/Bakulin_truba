#include <iostream>


struct Truba {
    std::string name;
    float length;
    float diametr;
    bool on_fix = false;
};

struct CS {
    std::string name;
    int workshop;
    int workshop_on_work;
    char rank;
};

void console() {
    Truba t;
    CS q;
    int ok;
start:
    std::cout << "Enter Truba parameters(name,length,diametr):";
    std::cin >> t.name >> t.length >> t.diametr;
    std::cout << t.name << std::endl << t.length << std::endl << t.diametr << std::endl << "Press 1 to continue \nPress 0 to type parametres agian";
    std::cin >> ok;
    if (ok == 0) { goto start; };
start1:
    std::cout << "Enter CS parameters(name,workshop,workshop_on_work,rank):";
    std::cin >> q.name >> q.workshop >> q.workshop_on_work >> q.rank;
    std::cout << q.name << std::endl << q.workshop << std::endl << q.workshop_on_work << std::endl << q.rank << std::endl << "Press 1 to continue \nPress 0 to type parametres agian";
    std::cin >> ok;
    if (ok == 0) { goto start1; };
};

int main() {
    console();
}