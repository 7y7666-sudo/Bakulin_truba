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
    int stationClass;
};

int inputInt() {
    int value;
    string extra;

    while (true) {
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

bool isValidPipe(Pipe p) {
    return !p.mark.empty() && p.length > 0 && p.diameter > 0;
}

bool isValidCS(CS s) {
    return !s.name.empty() &&
        s.totalWorkshops >= 0 &&
        s.workingWorkshops >= 0 &&
        s.workingWorkshops <= s.totalWorkshops;
}

Pipe readPipe() {
    Pipe p;
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
    return p;
}

void printPipe(Pipe p) {
    cout << "--- Pipe Details ---" << endl;
    cout << "Mark: " << p.mark << endl;
    cout << "Length: " << p.length << " km" << endl;
    cout << "Diameter: " << p.diameter << " mm" << endl;
    cout << "Status: " << (p.inRepair ? "In Repair" : "Operational") << endl;
}

Pipe editPipeRepair(Pipe p) {
    cout << "Current status: " << (p.inRepair ? "In Repair" : "Operational") << endl;
    cout << "Set status to 'In Repair'? (1 - yes, 0 - no): ";
    p.inRepair = inputBinary();
    return p;
}

CS readCS() {
    CS s;
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
    while (s.workingWorkshops > s.totalWorkshops || s.workingWorkshops < 0) {
        cout << "Error! Working workshops cannot be more than total or less than 0. Try again: ";
        s.workingWorkshops = inputInt();
    }

    cout << "Enter station class: ";
    s.stationClass = inputInt();
    return s;
}

void printCS(CS s) {
    cout << "--- Station Details ---" << endl;
    cout << "Name: " << s.name << endl;
    cout << "Total workshops: " << s.totalWorkshops << endl;
    cout << "Working workshops: " << s.workingWorkshops << endl;
    cout << "Class: " << s.stationClass << endl;
}

CS editCSWorkshop(CS s) {
    cout << "1. Start workshop" << endl;
    cout << "0. Stop workshop" << endl;
    int choice = inputInt();

    if (choice == 1) {
        if (s.workingWorkshops < s.totalWorkshops) {
            s.workingWorkshops = s.workingWorkshops + 1;
            cout << "Workshop started." << endl;
        }
        else {
            cout << "Error: All workshops are already working!" << endl;
        }
    }
    else if (choice == 0) {
        if (s.workingWorkshops > 0) {
            s.workingWorkshops = s.workingWorkshops - 1;
            cout << "Workshop stopped." << endl;
        }
        else {
            cout << "Error: No working workshops available!" << endl;
        }
    }
    else {
        cout << "Invalid choice." << endl;
    }
    return s;
}

void saveAll(Pipe p, CS s) {
    if (!isValidPipe(p) || !isValidCS(s)) {
        cout << "Error! Cannot save invalid data. Please enter Pipe and CS data first." << endl;
        return;
    }

    ofstream out("data.txt");
    if (out.is_open()) {
        out << p.mark << " " << p.length << " " << p.diameter << " " << p.inRepair << endl;
        out << s.name << " " << s.totalWorkshops << " " << s.workingWorkshops << " " << s.stationClass << endl;

        if (out.good()) {
            cout << "Data successfully saved to data.txt" << endl;
        }
        else {
            cout << "Error while writing data to file!" << endl;
        }
        out.close();
    }
    else {
        cout << "Error opening file for saving!" << endl;
    }
}

struct DataBundle {
    Pipe p;
    CS s;
    bool success;
};

DataBundle loadAllFixed() {
    DataBundle bundle;
    bundle.p = { "None", 0.0, 0, false };
    bundle.s = { "None", 0, 0, 0 };
    bundle.success = false;

    ifstream in("data.txt");
    if (!in.is_open()) {
        cout << "File not found or cannot be opened." << endl;
        return bundle;
    }

    int repairStatus;
    if (!(in >> bundle.p.mark >> bundle.p.length >> bundle.p.diameter >> repairStatus
        >> bundle.s.name >> bundle.s.totalWorkshops >> bundle.s.workingWorkshops >> bundle.s.stationClass)) {
        cout << "Error! File contains incomplete or invalid data." << endl;
        in.close();
        return bundle;
    }

    if (bundle.p.length <= 0 || bundle.p.diameter <= 0 ||
        (repairStatus != 0 && repairStatus != 1) ||
        !isValidCS(bundle.s)) {
        cout << "Error! File contains invalid data." << endl;
        in.close();
        return bundle;
    }

    bundle.p.inRepair = repairStatus;
    bundle.success = true;
    in.close();

    cout << "Data successfully loaded from data.txt" << endl;
    return bundle;
}

int main() {
    Pipe myPipe = { "None", 0.0, 0, false };
    CS myCS = { "None", 0, 0, 0 };

    bool pipeCreated = false;
    bool csCreated = false;

    int choice = -1;
    while (choice != 0) {
        cout << "\n--- MAIN MENU ---" << endl;
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

        switch (choice)
        {
        case 1: {
            myPipe = readPipe();
            pipeCreated = true;
            break;
        }
        case 2: {
            myCS = readCS();
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
                myPipe = editPipeRepair(myPipe);
            }
            else {
                cout << "Error! Pipe has not been created yet." << endl;
            }
            break;
        }
        case 5: {
            if (csCreated) {
                myCS = editCSWorkshop(myCS);
            }
            else {
                cout << "Error! CS has not been created yet." << endl;
            }
            break;
        }
        case 6: {
            if (pipeCreated && csCreated) {
                saveAll(myPipe, myCS);
            }
            else {
                cout << "Error! Create Pipe and CS before saving." << endl;
            }
            break;
        }
        case 7: {
            DataBundle loaded = loadAllFixed();

            if (loaded.success) {
                myPipe = loaded.p;
                myCS = loaded.s;
                pipeCreated = true;
                csCreated = true;
            }

            break;
        }
        case 0: {
            cout << "Exiting program..." << endl;
            break;
        }
        default: {
            cout << "Invalid menu option, try again." << endl;
            break;
        }
        }
    }

    return 0;
}

