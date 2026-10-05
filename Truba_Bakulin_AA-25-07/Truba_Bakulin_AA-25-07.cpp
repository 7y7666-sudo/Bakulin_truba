#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct Pipe {
    string mark;
    double length;
    int diameter;
    bool inRepair;
};

struct CS {
    string name;
    int totalWorkshops;
    int workingWorkshops;
    char stationClass;
};

int inputInt() {
    int value;
    string extra;

    while (true) {
        cin >> ws;

        if (cin.peek() == '+') {
            getline(cin, extra);
            cout << "Error! Please enter a valid integer: ";
            continue;
        }

        if (cin >> value) {
            getline(cin, extra);

            if (extra.find_first_not_of(" \t\r") == string::npos) {
                return value;
            }
        }
        else {
            cin.clear();
            getline(cin, extra);
        }

        cout << "Error! Please enter a valid integer: ";
    }
}

double inputDouble() {
    double value;
    string extra;

    while (true) {
        cin >> ws;

        if (cin.peek() == '+') {
            getline(cin, extra);
            cout << "Error! Please enter a valid number: ";
            continue;
        }

        if (cin >> value) {
            getline(cin, extra);

            if (extra.find_first_not_of(" \t\r") == string::npos) {
                return value;
            }
        }
        else {
            cin.clear();
            getline(cin, extra);
        }

        cout << "Error! Please enter a valid number: ";
    }
}

int inputBinary() {
    int value;

    while (true) {
        value = inputInt();

        if (value == 0 || value == 1) {
            return value;
        }

        cout << "Error! Enter 1 or 0: ";
    }
}

bool isValidPipe(const Pipe& p) {
    return !p.mark.empty() &&
        p.length > 0 &&
        p.diameter > 0;
}

bool isValidCS(const CS& s) {
    return !s.name.empty() &&
        s.totalWorkshops >= 0 &&
        s.workingWorkshops >= 0 &&
        s.workingWorkshops <= s.totalWorkshops &&
        s.stationClass >= 0;
}

void readPipe(Pipe& p) {
    cout << "Enter km mark (name): ";
    cin >> p.mark;

    cout << "Enter length (km): ";
    p.length = inputDouble();

    while (p.length <= 0) {
        cout << "Error! Length must be greater than 0. Try again: ";
        p.length = inputDouble();
    }

    cout << "Enter diameter (mm): ";
    p.diameter = inputInt();

    while (p.diameter <= 0) {
        cout << "Error! Diameter must be greater than 0. Try again: ";
        p.diameter = inputInt();
    }

    cout << "In repair? (1 - yes, 0 - no): ";
    p.inRepair = inputBinary();
}

void printPipe(const Pipe& p) {
    cout << "Pipe Details" << endl;
    cout << "Mark: " << p.mark << endl;
    cout << "Length: " << p.length << " km" << endl;
    cout << "Diameter: " << p.diameter << " mm" << endl;
    cout << "Status: " << (p.inRepair ? "In Repair" : "Operational") << endl;
}

void editPipeRepair(Pipe& p) {
    cout << "Current status: " << (p.inRepair ? "In Repair" : "Operational") << endl;
    cout << "Set status to 'In Repair'? (1 - yes, 0 - no): ";
    p.inRepair = inputBinary();
}

void readCS(CS& s) {
    cout << "Enter CS name: ";
    cin >> s.name;

    cout << "Enter total workshops: ";
    s.totalWorkshops = inputInt();

    while (s.totalWorkshops < 0) {
        cout << "Error! Total workshops cannot be less than 0. Try again: ";
        s.totalWorkshops = inputInt();
    }

    cout << "Enter working workshops: ";
    s.workingWorkshops = inputInt();

    while (s.workingWorkshops < 0 ||
        s.workingWorkshops > s.totalWorkshops) {
        cout << "Error! Working workshops cannot be more than total or less than 0. Try again: ";
        s.workingWorkshops = inputInt();
    }

    cout << "Enter station class: ";
    cin >> s.stationClass;
}

void printCS(const CS& s) {
    cout << "Station Details" << endl;
    cout << "Name: " << s.name << endl;
    cout << "Total workshops: " << s.totalWorkshops << endl;
    cout << "Working workshops: " << s.workingWorkshops << endl;
    cout << "Class: " << s.stationClass << endl;
}

void editCSWorkshop(CS& s) {
    cout << "1. Start workshop" << endl;
    cout << "0. Stop workshop" << endl;
    int choice = inputInt();

    if (choice == 1) {
        if (s.workingWorkshops < s.totalWorkshops) {
            s.workingWorkshops++;
            cout << "Workshop started." << endl;
        }
        else {
            cout << "Error: All workshops are already working!" << endl;
        }
    }
    else if (choice == 0) {
        if (s.workingWorkshops > 0) {
            s.workingWorkshops--;
            cout << "Workshop stopped." << endl;
        }
        else {
            cout << "Error: No working workshops available!" << endl;
        }
    }
    else {
        cout << "Invalid choice." << endl;
    }
}

bool loadAll(Pipe& p, CS& s, bool& pipeLoaded, bool& csLoaded) {
    ifstream in("data.txt");

    pipeLoaded = false;
    csLoaded = false;

    if (!in.is_open()) {
        cout << "File not found or empty." << endl;
        return false;
    }

    string type;

    while (in >> type) {
        if (type == "PIPE") {
            Pipe loadedPipe;
            int repairStatus;

            if (!(in >> loadedPipe.mark
                >> loadedPipe.length
                >> loadedPipe.diameter
                >> repairStatus)) {
                cout << "Error! File contains incomplete Pipe data." << endl;
                return false;
            }

            if (!isValidPipe(loadedPipe) ||
                (repairStatus != 0 && repairStatus != 1)) {
                cout << "Error! File contains invalid Pipe data." << endl;
                return false;
            }

            loadedPipe.inRepair = repairStatus;
            p = loadedPipe;
            pipeLoaded = true;
        }
        else if (type == "CS") {
            CS loadedCS;

            if (!(in >> loadedCS.name
                >> loadedCS.totalWorkshops
                >> loadedCS.workingWorkshops
                >> loadedCS.stationClass)) {
                cout << "Error! File contains incomplete CS data." << endl;
                return false;
            }

            if (!isValidCS(loadedCS)) {
                cout << "Error! File contains invalid CS data." << endl;
                return false;
            }

            s = loadedCS;
            csLoaded = true;
        }
        else {
            cout << "Error! Unknown data in file." << endl;
            return false;
        }
    }

    return pipeLoaded || csLoaded;
}

void saveAll(const Pipe& p, const CS& s, bool pipeCreated, bool csCreated) {
    Pipe savedPipe;
    CS savedCS;
    bool pipeSaved = false;
    bool csSaved = false;

    ifstream in("data.txt");

    if (in.is_open()) {
        string type;

        while (in >> type) {
            if (type == "PIPE") {
                Pipe loadedPipe;
                int repairStatus;

                if (!(in >> loadedPipe.mark
                    >> loadedPipe.length
                    >> loadedPipe.diameter
                    >> repairStatus)) {
                    cout << "Error! Existing file contains invalid Pipe data." << endl;
                    return;
                }

                if (!isValidPipe(loadedPipe) ||
                    (repairStatus != 0 && repairStatus != 1)) {
                    cout << "Error! Existing file contains invalid Pipe data." << endl;
                    return;
                }

                loadedPipe.inRepair = repairStatus;
                savedPipe = loadedPipe;
                pipeSaved = true;
            }
            else if (type == "CS") {
                CS loadedCS;

                if (!(in >> loadedCS.name
                    >> loadedCS.totalWorkshops
                    >> loadedCS.workingWorkshops
                    >> loadedCS.stationClass)) {
                    cout << "Error! Existing file contains invalid CS data." << endl;
                    return;
                }

                if (!isValidCS(loadedCS)) {
                    cout << "Error! Existing file contains invalid CS data." << endl;
                    return;
                }

                savedCS = loadedCS;
                csSaved = true;
            }
            else {
                cout << "Error! Existing file contains unknown data." << endl;
                return;
            }
        }
    }

    if (pipeCreated) {
        savedPipe = p;
        pipeSaved = true;
    }

    if (csCreated) {
        savedCS = s;
        csSaved = true;
    }

    if (!pipeSaved && !csSaved) {
        cout << "Error! There are no objects to save." << endl;
        return;
    }

    ofstream out("data.txt");

    if (!out.is_open()) {
        cout << "Error opening file for saving!" << endl;
        return;
    }

    if (pipeSaved) {
        out << "PIPE" << endl;
        out << savedPipe.mark << " "
            << savedPipe.length << " "
            << savedPipe.diameter << " "
            << savedPipe.inRepair << endl;
    }

    if (csSaved) {
        out << "CS" << endl;
        out << savedCS.name << " "
            << savedCS.totalWorkshops << " "
            << savedCS.workingWorkshops << " "
            << savedCS.stationClass << endl;
    }

    if (out) {
        cout << "Data successfully saved to data.txt" << endl;
    }
    else {
        cout << "Error while writing data to file!" << endl;
    }
}

int main() {
    Pipe myPipe;
    CS myCS;

    bool pipeCreated = false;
    bool csCreated = false;

    int choice = -1;

    while (choice != 0) {
        cout << "\n   MAIN MENU " << endl;
        cout << "1. Add Pipe" << endl;
        cout << "2. Add CS" << endl;
        cout << "3. View all objects" << endl;
        cout << "4. Edit Pipe" << endl;
        cout << "5. Edit CS" << endl;
        cout << "6. Save" << endl;
        cout << "7. Load" << endl;
        cout << "0. Exit" << endl;
        cout << "Your choice: ";

        choice = inputInt();

        switch (choice) {
        case 1: {
              readPipe(myPipe);
              pipeCreated = true;
              break;
        }
        case 2: {
            readCS(myCS);
            csCreated = true;
            break;
        }
        case 3: {
            if (pipeCreated) {
                printPipe(myPipe);
            }
            else {
                cout << "Pipe has not been created yet." << endl;
            }

            if (csCreated) {
                printCS(myCS);
            }
            else {
                cout << "CS has not been created yet." << endl;
            }
            break;
        }
        case 4: {
            if (pipeCreated) {
                editPipeRepair(myPipe);
            }
            else {
                cout << "Error! Pipe has not been created yet." << endl;
            }
            break;
        }

        case 5:{
            if (csCreated) {
                editCSWorkshop(myCS);
            }
            else {
                cout << "Error! CS has not been created yet." << endl;
            }
            break;
        }
        case 6: {
            saveAll(myPipe, myCS, pipeCreated, csCreated);
            break;
        }
        case 7: {
            bool pipeLoaded;
            bool csLoaded;

            if (loadAll(myPipe, myCS, pipeLoaded, csLoaded)) {
                if (pipeLoaded) {
                    pipeCreated = true;
                }

                if (csLoaded) {
                    csCreated = true;
                }

                cout << "Data successfully loaded from data.txt" << endl;
            
            }
            break;
        }

        case 0: {
            cout << "Exiting program..." << endl;
            break;
        }
        default:
            cout << "Invalid menu option, try again." << endl;
        }
    }
    return 0;
}
