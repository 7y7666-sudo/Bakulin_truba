#include "Pipe.h"
#include <iostream>
#include "Utils.h"

using namespace std;

int Pipe::NextId = 1;

Pipe::Pipe()
{
	id = NextId++;
}

int Pipe::getID() const
{
	return id;
}

std::string Pipe::getName() const
{
	return name;
}

bool Pipe::getUnderRepair() const
{
	return underRepair;
}

std::istream& operator>>(istream& in, Pipe& p)
{
	readValidName(p.name, "Enter the name of the pipe: ");
	readValidNumber(p.length, "Enter the length of the pipe (in kilometers): ", 1.0);
	readValidNumber(p.diameter, "Enter the pipe diameter (in millimeters): ", 1);
	readValidNumber(p.underRepair, "Enter the pipe status (1 — under repair, 0 — in working order): ", false, true);
	return in;
}

std::ifstream& operator>>(ifstream& inF, Pipe& p) {
	inF >> p.id;
	inF.ignore(100, '\n');
	getline(inF, p.name);
	inF >> p.length;
	inF >> p.diameter;
	inF >> p.underRepair;
	Pipe::NextId = p.id + 1;
	return inF;
}

void Pipe::editUnderRepair()
{
	underRepair = !(underRepair);
}

std::ostream& operator << (ostream& out, const Pipe& p) {
	out << "Pipe ID: " << p.id << endl;
	out << "Pipe name: " << p.name << endl;
	out << "Pipe length: " << p.length << " km" << endl;
	out << "Pipe diameter: " << p.diameter << " mm" << endl;
	out << "Condition of the pipe: " << (p.underRepair ? "Under repair" : "In working order") << endl;
	return out;
}

std::ofstream& operator << (ofstream& outF, const Pipe& p) {
	outF << p.id << endl;
	outF << p.name << endl;
	outF << p.length << endl;
	outF << p.diameter << endl;
	outF << p.underRepair << endl;
	return outF;
}