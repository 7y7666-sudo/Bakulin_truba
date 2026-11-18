#include "Utils.h"
#include "GTS.h"
#include <chrono>
#include <format>

using namespace std;

int main() {
	string time = std::format("{:%d_%m_%Y %H_%M_%OS}", std::chrono::system_clock::now());
	ofstream logfile("Logs\\log_" + time + ".txt");
	redirect_output_wrapper cerr_out(cerr);
	if (logfile) {
		cerr_out.redirect(logfile);
	}
	string menu = "Menu\n1.Add pipe\n2.Add CS\n3.View all objects\n4.Save\n5.Load\n6.Batch processing of pipes\n7.Batch processing of CS\n8.Connect\n9.Disconnect\n10.Sort\n0.Exit\n";
	GTS gts;
	while (true) {
		int action;
		readValidNumber(action, menu, 0, 10);
		if (action == 0) {
			return 0;
		}
		switch (action) {
		case 1: { gts.addPipe(); break; }
		case 2: { gts.addStation(); break; }
		case 3: { gts.showAll(); break; }
		case 4: { gts.saveToFile(); break; }
		case 5: { gts.loadFromFile(); break; }
		case 6: { gts.searchPipes(); break; }
		case 7: { gts.searchStations(); break; }
		case 8: { gts.connect(); break; }
		case 9: { gts.disconnect(); break; }
		case 10: { gts.sort(); break; }
		}
	}
	return 0;
}