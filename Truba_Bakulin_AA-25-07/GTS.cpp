#include <iostream>
#include <vector>
#include <queue>
#include "GTS.h"
#include "Utils.h"

using namespace std;

void GTS::addPipe()
{
	inputObjects(pipes, maxPipeID++);
}

void GTS::addStation()
{
	inputObjects(css, maxStationID++);
}

void GTS::showAll() const
{
	if (pipes.empty() && css.empty()) {
		cout << "Warning: The pipe and CS have not been created." << endl;
		return;
	}
	outputObjects(pipes, "Data output for the pipe:");
	outputObjects(css, "Data output for the station:");
	outputConnections(connections);
}

void GTS::editPipes(const std::vector<int>& result)
{
	for (const auto& i : result) {
		pipes[i].editUnderRepair();
	}
}

void GTS::editStations(const std::vector<int>& result)
{
	double workshopPercentage = 0;
	readValidNumber(workshopPercentage, "Enter the percentage of working workshops out of the total number of workshops: ", 0.0, 100.0);
	for (const auto& i : result) {
		css[i].editTotalWorkingShops(workshopPercentage);
	}
}

void GTS::saveToFile() const
{
	if (pipes.empty() && css.empty()) {
		cout << "Warning: The pipe and CS have not been created." << endl;
		return;
	}
	string file_name;
	cout << "Specify the file name: ";
	getline(cin, file_name);
	savedDataToFile(file_name, pipes, css, connections);
}

void GTS::loadFromFile()
{
	string file_name;
	cout << "Specify the file name: ";
	getline(cin, file_name);
	loadDataFromFile(file_name, pipes, css, connections, maxPipeID, maxStationID);
}

bool GTS::isPipeConnected(int id) const
{
	for (const auto& [key, value] : connections) {
		if (id == key) {
			return true;
		}
	}
	return false;
}

bool GTS::isStationConnected(int id) const
{
	for (const auto& [key, value] : connections) {
		if (id == value.first || id == value.second) {
			return true;
		}
	}
	return false;
}

void GTS::deletePipes(const std::vector<int>& result)
{
	for (const auto& id : result) {
		if (isPipeConnected(id)) {
			cout << "\nWarning: Pipe with ID " << id << " connected to the gas transmission network" << endl;
			int choice;
			readValidNumber(choice, "Delete the pipe with the ID and the corresponding connection? (1 - yes, 0 - no): ", 0, 1);
			if (choice == 1) {
				connections.erase(id);
				pipes.erase(id);
				cout << "Pipe with ID " << id << " and the corresponding compound has been removed" << endl;
			}
			else {
				continue;
			}
		}
		else {
			pipes.erase(id);
			cout << "Pipe with ID " << id << " has been removed" << endl;
		}
	}
}

void GTS::deleteStations(const std::vector<int>& result)
{
	for (const auto& id : result) {
		if (isStationConnected(id)) {
			int choice;
			cout << "\nWarning: Station with ID " << id << " connected to the gas transmission network" << endl;
			readValidNumber(choice, "Delete the station with the ID and the corresponding connection? (1 - yes, 0 - no): ", 0, 1);
			if (choice == 1) {
				vector<int> toErase = {};
				for (const auto& [key, value] : connections) {
					if (value.first == id || value.second == id) {
						toErase.push_back(key);
					}
				}
				for (const auto& pipeID : toErase) {
					connections.erase(pipeID);
				}
				css.erase(id);
				cout << "Station with ID " << id << " and the corresponding compound has been removed" << endl;
			}
			else {
				continue;
			}
		}
		else {
			css.erase(id);
			cout << "Station with ID " << id << " has been removed" << endl;
		}
	}
}

int GTS::findFreePipeByDiameter(int diameter) const
{
	for (const auto& [key, item] : pipes) {
		if (!isPipeConnected(key) && item.getDiameter() == diameter && !item.getUnderRepair()) {
			return key;
		}
	}
	return 0;
}

int GTS::inputStationId(const std::string& prompt, int excludeID) const
{
	int csID;
	while (true) {
		readValidNumber(csID, prompt, 1);
		if (css.count(csID) == 0) {
			cout << "There is no station with that ID." << endl;
		}
		else if (csID == excludeID) {
			cout << "You cannot connect a station to itself." << endl;
		}
		else {
			break;
		}
	}
	return csID;
}

int GTS::findOrCreatePipe(int diameter)
{
	int pipeID = findFreePipeByDiameter(diameter);
	if (pipeID == 0) {
		cout << "There is no free pipe with that diameter, so we will add a new one:" << endl;
		Pipe newPipe;
		newPipe.setID(maxPipeID++);
		newPipe.inputForConnection(diameter);
		pipes.emplace(newPipe.getID(), newPipe);
		pipeID = maxPipeID - 1;
	}
	return pipeID;
}

void GTS::searchPipes()
{
	if (pipes.empty()) {
		cout << "Warning: The pipe has not been created." << endl;
		return;
	}
	std::vector<int> foundPipes;
	int value = {};

	readValidNumber(value, "Find by: 1-name, 2-attribute under repair: ", 1, 2);
	if (value == 1) {
		searchByName(pipes, foundPipes);
	}
	else {
		searchByValue(pipes, foundPipes, "Enter 1 if the pipe is under repair, 0 if the pipe is in working order: ", false, true, checkByRepair);
	}

	readValidNumber(value, "Clarify: 1 – all found, 2 – specific IDs: ", 1, 2);
	if (value == 2) {
		foundPipes = inputSpecificIDs(pipes, foundPipes);
	}

	readValidNumber(value, "Action: 1 - delete, 2 - edit. ", 1, 2);
	if (value == 1) {
		deletePipes(foundPipes);
	}
	else {
		editPipes(foundPipes);
	}
}

void GTS::searchStations()
{
	if (css.empty()) {
		cout << "Warning: the CS has not been created." << endl;
		return;
	}
	std::vector<int> foundStations;
	int value = {};
	readValidNumber(value, "Find by: 1-name, 2-percentage of unused workshops: ", 1, 2);
	if (value == 1) {
		searchByName(css, foundStations);
	}
	else {
		searchByValue(css, foundStations, "Enter the percentage of unused workshops (<=): ", 0.0, 100.0, checkByWorkshops);
	}

	readValidNumber(value, "Clarify: 1 – all found, 2 – specific IDs: ", 1, 2);
	if (value == 2) {
		foundStations = inputSpecificIDs(css, foundStations);
	}

	readValidNumber(value, "Action: 1 - delete, 2 - edit. ", 1, 2);
	if (value == 1) {
		deleteStations(foundStations);
	}
	else {
		editStations(foundStations);
	}
}

void GTS::connect()
{
	if (css.size() < 2) {
		cout << "Warning: it is not possible to connect objects to the gas transmission network." << endl;
		return;
	}
	int inputCSID = inputStationId("Enter the entrance station ID: ");
	int outputCSID = inputStationId("Enter the exit station ID: ", inputCSID);
	int pipeDiameter = 0;
	readValidNumber(pipeDiameter, "Enter the pipe diameter: ", 1);
	int pipeID = findOrCreatePipe(pipeDiameter);
	connections.emplace(pipeID, pair{ inputCSID, outputCSID });
	cout << "Station " << inputCSID << " connected to the station " << outputCSID << " with pipe " << pipeID << endl;
}

void GTS::disconnect()
{
	if (connections.empty()) {
		cout << "Warning: there are no facilities in the gas transmission network." << endl;
		return;
	}
	while (true) {
		int pipeID;
		readValidNumber(pipeID, "Enter the pipe ID: ", 1);
		if (isPipeConnected(pipeID)) {
			cout << "Gas transmission network " << connections[pipeID].first << "->" << connections[pipeID].second << " disconnected" << endl;
			connections.erase(pipeID);
			break;
		}
		else {
			cout << "Such a connection does not exist." << endl;
		}
	}
}

void GTS::sort()
{
	if (connections.empty()) {
		cout << "Warning: there are no facilities in the gas transmission network." << endl;
		return;
	}

	unordered_map<int, int> degree = {}; // id CS -> кол-во полустепеней захода
	unordered_map<int, vector<int>> adjacent = {}; // id CS -> список полустепеней исхода {pipeID, outCSID}

	for (const auto& [key, item] : connections) {
		adjacent[item.first].push_back(item.second);
		degree[item.first] += 0;
		degree[item.second] += 1;
	}

	queue<int> ready = {};
	for (const auto& [key, item] : degree) {
		if (item == 0) {
			ready.push(key);
		}
		else {
			continue;
		}
	}

	vector<int> order = {};
	while (!ready.empty()) {
		int current = ready.front();
		ready.pop();
		order.push_back(current);

		for (const auto& i : adjacent[current]) {
			--degree[i];
			if (degree[i] == 0) {
				ready.push(i);
			}
		}
	}

	if (order.size() < adjacent.size()) {
		cout << "The gas transmission network contains a cycle, so sorting is not possible." << endl;
		return;
	}
	for (int i : order) {
		cout << i << " ";
	}
	cout << endl;
}