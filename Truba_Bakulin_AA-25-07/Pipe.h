#pragma once
#include <string>
#include <iostream>

class Pipe
{
private:
	static int NextId;
	int id = {};
	std::string name = {};
	double length = {}; // km
	int diameter = {}; // mm
	bool underRepair = {};
public:
	Pipe();
	int getID() const;
	std::string getName() const;
	bool getUnderRepair() const;
	void editUnderRepair();
	friend std::istream& operator >> (std::istream& in, Pipe& p);
	friend std::ifstream& operator >> (std::ifstream& inF, Pipe& p);
	friend std::ofstream& operator << (std::ofstream& outF, const Pipe& p);
	friend std::ostream& operator << (std::ostream& out, const Pipe& p);
};