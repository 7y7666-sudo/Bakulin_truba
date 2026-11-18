#include "Station.h"
#include <iostream>
#include <fstream>
#include "Utils.h"

using namespace std;

int Station::getID() const
{
	return id;
}

void Station::setID(int newId)
{
	id = newId;
}

std::string Station::getName() const
{
	return name;
}

double Station::getUnusedWorkshops() const
{
	return 100 * (totalShops - totalWorkingShops) / totalShops;
}

void Station::editTotalWorkingShops(double workshopPercentage)
{
	totalWorkingShops = static_cast<int>(totalShops * workshopPercentage / 100);
}


std::istream& operator>>(istream& in, Station& cs)
{
	readValidName(cs.name, "Enter the station name: ");
	readValidNumber(cs.totalShops, "Enter the number of workshops: ", 1);
	readValidNumber(cs.totalWorkingShops, "Enter the number of working workshops: ", 0, cs.totalShops);
	readValidNumber(cs.classStation, "Enter the station class (a, b, c): ", 'a', 'c');
	return in;
}

std::ifstream& operator>>(ifstream& inF, Station& cs) {
	inF >> cs.id;
	inF.ignore(100, '\n');
	getline(inF, cs.name);
	inF >> cs.totalShops;
	inF >> cs.totalWorkingShops;
	inF >> cs.classStation;
	return inF;
}

std::ostream& operator<<(ostream& out, const Station& cs)
{
	out << "Station ID: " << cs.id << endl;
	out << "Station name: " << cs.name << endl;
	out << "Number of total workshops: " << cs.totalShops << endl;
	out << "Number of workshops: " << cs.totalWorkingShops << endl;
	out << "Station class: " << cs.classStation << endl;
	return out;
}

std::ofstream& operator<<(ofstream& outF, const Station& cs)
{
	outF << cs.id << endl;
	outF << cs.name << endl;
	outF << cs.totalShops << endl;
	outF << cs.totalWorkingShops << endl;
	outF << cs.classStation << endl;
	return outF;
}