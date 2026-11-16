#include <iostream>
#include <unordered_map>
#include <fstream>
#include "Pipe.h"
#include "Station.h"
#include "Utils.h"
#include <chrono>
#include <format>

using namespace std;

void command(int action, unordered_map <int, Pipe>& pipes, unordered_map <int, Station>& css) {
	switch (action) {
	case 1: {
		inputObjects(pipes);
		break;
	}
	case 2: {
		inputObjects(css);
		break;
	}
	case 3: {
		if (pipes.empty() && css.empty()) {
			cout << "Warning: The pipe and CS have not been created." << endl;
		}
		else {
			outputOdjects(pipes, "Data output for the pipe:");
			outputOdjects(css, "Data output for the station:");
		}
		break;
	}
	case 4: {
		if (pipes.empty() && css.empty()) {
			cout << "Warning: The pipe and CS have not been created." << endl;
		}
		else {
			string file_name;
			cout << "Specify the file name: ";
			getline(cin, file_name);
			savedDataToFile(file_name, pipes, css);
		}
		break;
	}
	case 5: {
		string file_name;
		cout << "Specify the file name: ";
		getline(cin, file_name);
		loadDataFromFile(file_name, pipes, css);
		break;
	}
	case 6: {
		if (!pipes.empty()) {
			searchPipes(pipes);
		}
		else {
			cout << "Warning: The pipe has not been created." << endl;
		}
		break;
	}
	case 7: {
		if (!css.empty()) {
			searchStations(css);
		}
		else {
			cout << "Warning: the CS has not been created." << endl;
		}
		break;
	}
	}
}

int main() {
	string time = std::format("{:%d_%m_%Y %H_%M_%OS}", std::chrono::system_clock::now());
	ofstream logfile("log_" + time + ".txt");
	redirect_output_wrapper cerr_out(cerr);
	if (logfile) {
		cerr_out.redirect(logfile);
	}
	string menu = "Menu\n1.Add a pipe\n2.Add a compressor station\n3.View all objects\n4.Save\n5.Upload\n6.Batch processing of pipes\n7.Batch processing of compressor stations\n0.Exit\n";
	unordered_map <int, Pipe> pipes = {};
	unordered_map <int, Station> css = {};
	while (true) {
		int action;
		readValidNumber(action, menu, 0, 7);
		if (action == 0) {
			return 0;
		}
		else {
			command(action, pipes, css);
		}
	}
	return 0;
}