#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Pipe.h"
#include "Station.h"
#include "Utils.h"


class GTS
{
private:
	std::unordered_map <int, Pipe> pipes = {};
	std::unordered_map <int, Station> css = {};
	std::unordered_map <int, std::pair<int, int>> connections = {};
	int maxPipeID = 1;
	int maxStationID = 1;
	void editPipes(const std::vector<int>& result);
	void editStations(const std::vector<int>& result);
	bool isPipeConnected(int id) const;
	bool isStationConnected(int id) const;
	void deletePipes(const std::vector<int>& result);
	void deleteStations(const std::vector<int>& result);
	int findFreePipeByDiameter(int diameter) const;
	int inputStationId(const std::string& prompt, int excludeID = 0) const;
	int findOrCreatePipe(int diameter);
	int countWorkStations() const;
public:
	void addPipe();
	void addStation();
	void showAll() const;
	void saveToFile() const;
	void loadFromFile();
	void searchPipes();
	void searchStations();
	void connect();
	void disconnect();
	void sort();
};

template<typename Item>
void inputObjects(std::unordered_map<int, Item>& items, int maxObjectID) {
	Item item;
	item.setID(maxObjectID);
	std::cin >> item;
	items.emplace(item.getID(), item);
}

template <typename Item>
void outputObjects(const std::unordered_map<int, Item>& items, const std::string& prompt) {
	if (!items.empty()) {
		std::cout << prompt << std::endl;
		for (const auto& item : items) {
			std::cout << item.second;
			std::cout << std::endl;
		}
	}
}

inline void outputConnections(const std::unordered_map<int, std::pair<int, int>>& connections) {
	if (!connections.empty()) {
		std::cout << "Gas transportation network:" << std::endl;
		for (const auto& [key, item] : connections) {
			std::cout << key << ":";
			std::cout << std::get<0>(item) << "->";
			std::cout << std::get<1>(item) << std::endl;
		}
	}
}

template <typename Item>
void writeContainer(std::ofstream& outFile, const std::unordered_map<int, Item>& items) {
	outFile << items.size() << '\n';
	for (const auto& item : items) {
		outFile << item.second;
	}
}

inline void writeConnections(std::ofstream& outFile, const std::unordered_map<int, std::pair<int, int>>& connections) {
	outFile << connections.size() << '\n';
	for (const auto& [key, item] : connections) {
		outFile << key << " ";
		outFile << std::get<0>(item) << " ";
		outFile << std::get<1>(item) << '\n';
	}
}

template <typename Container1, typename Container2, typename Container3>
void savedDataToFile(const std::string& file_name
	, const Container1& pipes
	, const Container2& css
	, const Container3& connection) {
	std::ofstream outFile(file_name);
	if (!outFile.is_open()) {
		std::cout << "Error: failed to open the file" << std::endl;
		return;
	}
	writeContainer(outFile, pipes);
	writeContainer(outFile, css);
	writeConnections(outFile, connection);
	std::cerr << file_name << std::endl;
	std::cout << "The object(s) has been added to the file" << std::endl;
	outFile.close();
}

template <typename Item>
void loadContainer(std::ifstream& inFile, std::unordered_map<int, Item>& items, int& maxObjectID) {
	int count = 0;
	inFile >> count;
	if (count == 0) {
		return;
	}
	items.clear();
	Item item;
	for (int i = 0; i < count; i++) {
		inFile >> item;
		items.emplace(item.getID(), item);
		maxObjectID = std::max(maxObjectID, item.getID() + 1);
	}
}

inline void loadConnections(std::ifstream& inFile, std::unordered_map<int, std::pair<int, int>>& connections) {
	int count = 0;
	inFile >> count;
	if (count == 0) {
		return;
	}
	connections.clear();
	for (int i = 0; i < count; i++) {
		int PipeID, inputCSID, outputCSID;
		inFile >> PipeID >> inputCSID >> outputCSID;
		connections.emplace(PipeID, std::pair{ inputCSID, outputCSID });
	}
}

template <typename Container1, typename Container2, typename Container3>
void loadDataFromFile(const std::string& file_name, Container1& pipes, Container2& css, Container3& connections
	, int& maxPipeID
	, int& maxStationID) {
	std::ifstream inFile(file_name);
	if (!inFile.is_open()) {
		std::cout << "Error: failed to open the file" << std::endl;
		return;
	}
	else if (inFile.peek() == EOF) {
		std::cout << "Warning: the file is empty" << std::endl;
		return;
	}
	else {
		loadContainer(inFile, pipes, maxPipeID);
		loadContainer(inFile, css, maxStationID);
		loadConnections(inFile, connections);
		std::cerr << file_name << std::endl;
		std::cout << "The data has been successfully read!" << std::endl;
	}
	inFile.close();
}


template <typename Item>
bool checkByName(const Item& item, const std::string& name) {
	return item.getName().find(name) != std::string::npos;
}

inline bool checkByRepair(const Pipe& p, bool underRepair) {
	return p.getUnderRepair() == underRepair;
}

inline bool checkByWorkshops(const Station& cs, double unusedWorkshops) {
	return cs.getUnusedWorkshops() <= unusedWorkshops;
}

template<typename Item, typename Filter, typename T>
std::vector<int> findAndoutputObjectsByFilter(const std::unordered_map<int, Item>& items, Filter filter, const T& parametr) {
	std::vector <int> result;
	for (const auto& item : items) {
		if (filter(item.second, parametr)) {
			result.push_back(item.first);
		}
	}
	for (int id : result) {
		std::cout << items.at(id) << std::endl;
	}
	return result;
}

inline bool isFound(int id, const std::vector<int>& found_result) {
	for (int valid_id : found_result) {
		if (id == valid_id) {
			return true;
		}
	}
	return false;
}

inline bool isDuplicate(int id, const std::vector<int>& selected_ids) {
	for (int existing_id : selected_ids) {
		if (existing_id == id) {
			return true;
		}
	}
	return false;
}

template <typename Item>
std::vector<int> inputSpecificIDs(const std::unordered_map<int, Item>& items, const std::vector<int>& found_result) {
	std::vector<int> selected_ids;
	int id = 0;
	while (true) {
		readValidNumber(id, "Enter the ID (0 to finish entering): ", 0);
		if (id == 0) {
			if (!selected_ids.empty()) {
				break;
			}
			std::cout << "You haven’t selected any ID. The action has been cancelled." << std::endl;
			continue;
		}
		if (!isFound(id, found_result)) {
			std::cout << "Error: ID " << id << " is not included in the search results" << std::endl;
			continue;
		}
		if (isDuplicate(id, selected_ids)) {
			std::cout << "This ID has already been selected.\n";
			continue;
		}
		selected_ids.push_back(id);
		std::cout << "ID " << id << " selected\n";
	}
	return selected_ids;
}

template <typename Item>
void searchByName(const std::unordered_map<int, Item>& items, std::vector<int>& foundObjects) {
	std::string name;
	while (true) {
		readValidName(name, "Enter the object name: ");
		foundObjects = findAndoutputObjectsByFilter(items, checkByName<Item>, name);
		if (!foundObjects.empty()) {
			break;
		}
		std::cout << "Warning: there are no objects with this characteristic." << std::endl;
	}
}

template <typename T, typename Item, typename Filter >
void searchByValue(const std::unordered_map<int, Item>& items, std::vector<int>& foundObjects, const std::string& prompt
	, T a
	, T b
	, Filter function) {
	T searchValue;
	while (true) {
		readValidNumber(searchValue, prompt, a, b);
		foundObjects = findAndoutputObjectsByFilter(items, function, searchValue);
		if (!foundObjects.empty()) {
			break;
		}
		std::cout << "Warning: there are no objects with this characteristic." << std::endl;
	}
}