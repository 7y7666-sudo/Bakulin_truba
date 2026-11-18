#pragma once
#include <string>

class Pipe
{
private:
	int id = {};
	std::string name = {};
	double length = {}; // km
	int diameter = {}; // mm
	bool underRepair = {}; // true - труба на ремонте, false - труба в рабочем состоянии
public:
	int getID() const;
	void setID(int newId);
	std::string getName() const;
	int getDiameter() const;
	void inputForConnection(int d);
	bool getUnderRepair() const;
	void editUnderRepair();
	friend std::istream& operator >> (std::istream& in, Pipe& p);
	friend std::ifstream& operator >> (std::ifstream& inF, Pipe& p);
	friend std::ofstream& operator << (std::ofstream& outF, const Pipe& p);
	friend std::ostream& operator << (std::ostream& out, const Pipe& p);
};