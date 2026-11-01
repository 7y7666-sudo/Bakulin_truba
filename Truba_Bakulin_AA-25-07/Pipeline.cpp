#include "Pipeline.h"
#include "utils.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace {
    template<class Map> std::vector<int> ids(const Map& objects) {
        std::vector<int> result;
        result.reserve(objects.size());
        for (const auto& [id, object] : objects) {
            (void)object;
            result.push_back(id);
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    template<class Map, class Predicate>
    std::vector<int> findIds(const Map& objects, Predicate matches) {
        std::vector<int> result;
        for (const auto& [id, object] : objects)
            if (matches(object)) result.push_back(id);
        std::sort(result.begin(), result.end());
        return result;
    }

    template<class Map>
    void show(const Map& objects, const std::vector<int>& selected, const std::string& title) {
        std::cout << "\n--- " << title << " ---\n";
        if (selected.empty()) std::cout << "No matching objects found.\n";
        for (int id : selected) {
            std::cout << "ID: " << id << '\n';
            objects.at(id).print(std::cout);
        }
        std::cout << "Matches: " << selected.size() << '\n';
    }

    template<class Map> void showAll(const Map& objects, const std::string& title) {
        show(objects, ids(objects), title);
    }

    template<class Map> void eraseOne(Map& objects, const std::string& label) {
        if (objects.empty()) {
            std::cout << "No " << label << " objects to delete.\n";
            std::cerr << label << " deletion requested on empty collection\n";
            return;
        }
        const int id = util::id("Enter " + label + " ID to delete: ");
        if (objects.erase(id)) {
            std::cout << label << " deleted.\n";
            std::cerr << label << " deleted: ID=" << id << '\n';
        }
        else {
            std::cout << "ID not found.\n";
            std::cerr << label << " deletion failed: unknown ID=" << id << '\n';
        }
    }

}

void Pipeline::menu() const {
    std::cout << "\n=== PIPELINE DATABASE ===\n"
        << "1 Add pipe       2 Add CS          3 View all\n"
        << "4 Edit pipe      5 Edit CS         6 Save\n"
        << "7 Load           8 Find pipe name  9 Filter pipe status\n"
        << "10 Find CS name  11 Filter CS percent\n"
        << "12 Delete pipe   13 Delete CS      14 Batch pipes\n"
        << "0 Exit\n"
        << "Save/load: enter the file name when prompted\n";
}

int Pipeline::newId() {
    if (nextId_ > std::numeric_limits<int>::max()) {
        std::cout << "No IDs left.\n";
        std::cerr << "Object creation failed: ID range exhausted\n";
        return 0;
    }
    return static_cast<int>(nextId_++);
}

void Pipeline::run() {
    while (true) {
        menu();
        const int choice = util::number<int>("Choose action: ", 0, 14);
        std::cerr << "Menu action: " << choice << '\n';
        switch (choice) {
        case 1: addPipe(); break;
        case 2: addStation(); break;
        case 3: viewAll(); break;
        case 4: editPipe(); break;
        case 5: editStation(); break;
        case 6: save(); break;
        case 7: load(); break;
        case 8: searchPipesName(); break;
        case 9: searchPipesStatus(); break;
        case 10: searchStationsName(); break;
        case 11: searchStationsPercent(); break;
        case 12: eraseOne(pipes_, "Pipe"); break;
        case 13: eraseOne(stations_, "CS"); break;
        case 14: batchPipes(); break;
        case 0: std::cout << "Exiting.\n"; std::cerr << "Application exited\n"; return;
        }
    }
}

void Pipeline::addPipe() {
    const auto mark = util::text("Pipe mark/name: ");
    const double length = util::positiveDouble("Length in km (>0): ");
    const int diameter = util::number<int>("Diameter in mm (>0): ", 1,
        std::numeric_limits<int>::max());
    const bool inRepair = util::yesNo("In repair? (1 yes, 0 no): ");
    Pipe pipe(mark, length, diameter, inRepair);
    const int id = newId();
    if (!id) return;
    pipes_.emplace(id, pipe);
    std::cout << "Pipe added. ID=" << id << '\n';
    std::cerr << "Pipe added: ID=" << id << ", mark=" << pipe.mark() << '\n';
}

void Pipeline::addStation() {
    const std::string name = util::text("CS name: ");
    const int total = util::number<int>("Total workshops (at least 1): ", 1, std::numeric_limits<int>::max());
    const int working = util::number<int>("Working workshops: ", 0, total);
    char cls = 'A';
    while (true) {
        const auto value = util::trim(util::line("Station class (one non-space character): "));
        if (value.size() == 1 && std::isprint(static_cast<unsigned char>(value[0])) && value[0] != ' ') {
            cls = value[0]; break;
        }
        std::cout << "Enter exactly one printable non-space character.\n";
        std::cerr << "Invalid station class input\n";
    }
    CompressorStation station(name, total, working, cls);
    const int id = newId();
    if (!id) return;
    stations_.emplace(id, station);
    std::cout << "CS added. ID=" << id << '\n';
    std::cerr << "CS added: ID=" << id << ", name=" << name << '\n';
}

void Pipeline::viewAll() const {
    showAll(pipes_, "Pipes");
    showAll(stations_, "Compressor stations");
    std::cerr << "Viewed all objects: pipes=" << pipes_.size()
        << ", CS=" << stations_.size() << '\n';
}

void Pipeline::editPipe() {
    if (pipes_.empty()) { std::cout << "No pipes to edit.\n"; std::cerr << "Pipe edit on empty collection\n"; return; }
    const int id = util::id("Pipe ID: ");
    auto it = pipes_.find(id);
    if (it == pipes_.end()) { std::cout << "ID not found.\n"; std::cerr << "Pipe edit failed: ID=" << id << '\n'; return; }
    const bool status = util::yesNo("Set in repair? (1 yes, 0 no): ");
    it->second.setInRepair(status);
    std::cout << "Pipe updated.\n";
    std::cerr << "Pipe status updated: ID=" << id << ", inRepair=" << status << '\n';
}

void Pipeline::editStation() {
    if (stations_.empty()) { std::cout << "No CS to edit.\n"; std::cerr << "CS edit on empty collection\n"; return; }
    const int id = util::id("CS ID: ");
    auto it = stations_.find(id);
    if (it == stations_.end()) { std::cout << "ID not found.\n"; std::cerr << "CS edit failed: ID=" << id << '\n'; return; }
    const bool started = util::yesNo("Start one workshop? (1 start, 0 stop): ");
    const bool success = started ? it->second.startWorkshop() : it->second.stopWorkshop();
    std::cout << (success ? (started ? "Workshop started.\n" : "Workshop stopped.\n")
        : (started ? "All workshops already work.\n" : "No working workshops.\n"));
    std::cerr << "CS workshop operation " << (started ? "start" : "stop")
        << (success ? " succeeded" : " rejected") << ": ID=" << id << '\n';
}

void Pipeline::searchPipesName() const {
    const auto query = util::text("Text to find in pipe name: ");
    const auto found = findIds(pipes_, [&](const Pipe& p) { return p.mark().find(query) != std::string::npos; });
    show(pipes_, found, "Pipe name contains: " + query);
    std::cerr << "Pipe substring search: query=" << query << ", matches=" << found.size() << '\n';
}

void Pipeline::searchPipesStatus() const {
    const bool status = util::yesNo("Filter in repair? (1 yes, 0 no): ");
    const auto found = findIds(pipes_, [=](const Pipe& p) { return p.inRepair() == status; });
    show(pipes_, found, status ? "Pipes in repair" : "Operational pipes");
    std::cerr << "Pipe repair filter: status=" << status << ", matches=" << found.size() << '\n';
}

void Pipeline::searchStationsName() const {
    const auto query = util::text("Text to find in CS name: ");
    const auto found = findIds(stations_, [&](const CompressorStation& s) { return s.name().find(query) != std::string::npos; });
    show(stations_, found, "CS name contains: " + query);
    std::cerr << "CS substring search: query=" << query << ", matches=" << found.size() << '\n';
}

void Pipeline::searchStationsPercent() const {
    double low = util::number<double>("Minimum unused percent (0-100): ", 0.0, 100.0);
    double high = util::number<double>("Maximum unused percent (0-100): ", 0.0, 100.0);
    while (low > high) {
        std::cout << "Minimum must not exceed maximum.\n";
        std::cerr << "Invalid unused-workshop percentage range\n";
        low = util::number<double>("Minimum (0-100): ", 0.0, 100.0);
        high = util::number<double>("Maximum (0-100): ", 0.0, 100.0);
    }
    const auto found = findIds(stations_, [=](const CompressorStation& s) {
        return s.unusedPercent() >= low && s.unusedPercent() <= high;
        });
    std::ostringstream title; title << "CS unused workshop percentage in [" << low << ", " << high << "]";
    show(stations_, found, title.str());
    std::cerr << "CS percentage filter: min=" << low << ", max=" << high
        << ", matches=" << found.size() << '\n';
}

std::vector<int> Pipeline::choosePipeFilter() const {
    std::cout << "1 All pipes\n2 Name contains substring\n3 Repair status\n";
    const int filter = util::number<int>("Filter: ", 1, 3);
    if (filter == 1) return ids(pipes_);
    if (filter == 2) {
        const auto query = util::text("Name substring: ");
        return findIds(pipes_, [&](const Pipe& p) { return p.mark().find(query) != std::string::npos; });
    }
    const bool status = util::yesNo("In repair? (1 yes, 0 no): ");
    return findIds(pipes_, [=](const Pipe& p) { return p.inRepair() == status; });
}

std::vector<int> Pipeline::choosePipeTargets(const std::vector<int>& candidates) const {
    std::cout << "1 Apply to all found\n2 Choose IDs\n";
    if (util::number<int>("Target set: ", 1, 2) == 1) return candidates;
    const std::unordered_set<int> allowed(candidates.begin(), candidates.end());
    while (true) {
        std::istringstream input(util::line("IDs separated by spaces: "));
        std::unordered_set<int> selected;
        bool valid = true;
        int value = 0;
        while (input >> value) {
            if (!allowed.count(value)) { valid = false; break; }
            selected.insert(value);
        }
        if (!input.eof()) valid = false;
        if (valid && !selected.empty()) {
            std::vector<int> result(selected.begin(), selected.end());
            std::sort(result.begin(), result.end());
            return result;
        }
        std::cout << "Choose one or more IDs from the displayed results.\n";
        std::cerr << "Invalid batch target selection\n";
    }
}

void Pipeline::batchPipes() {
    if (pipes_.empty()) { std::cout << "No pipes to process.\n"; std::cerr << "Batch operation on empty pipe collection\n"; return; }
    const auto candidates = choosePipeFilter();
    show(pipes_, candidates, "Batch candidates");
    if (candidates.empty()) { std::cerr << "Batch operation cancelled: no matching pipes\n"; return; }
    const auto targets = choosePipeTargets(candidates);
    std::cout << "1 Edit repair status\n2 Delete\n";
    const int action = util::number<int>("Batch action: ", 1, 2);
    if (action == 1) {
        const bool status = util::yesNo("Set in repair? (1 yes, 0 no): ");
        for (int id : targets) pipes_.at(id).setInRepair(status);
        std::cerr << "Batch pipe edit: IDs=";
        for (int id : targets) std::cerr << id << ' ';
        std::cerr << ", inRepair=" << status << '\n';
        std::cout << "Updated " << targets.size() << " pipe(s).\n";
    }
    else {
        for (int id : targets) pipes_.erase(id);
        std::cerr << "Batch pipe deletion: IDs=";
        for (int id : targets) std::cerr << id << ' ';
        std::cerr << '\n';
        std::cout << "Deleted " << targets.size() << " pipe(s).\n";
    }
}


void Pipeline::save() const {
    const std::string filename = util::text("File name to save: ");
    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "Cannot open file for saving.\n";
        std::cerr << "Save failed: cannot open " << filename << '\n';
        return;
    }

    out << "PIPELINE_DB 1\n"
        << std::setprecision(std::numeric_limits<double>::max_digits10);

    for (int id : ids(pipes_)) {
        const auto& pipe = pipes_.at(id);
        out << "PIPE " << id << ' ' << std::quoted(pipe.mark()) << ' '
            << pipe.length() << ' ' << pipe.diameter() << ' '
            << pipe.inRepair() << '\n';
    }

    for (int id : ids(stations_)) {
        const auto& station = stations_.at(id);
        out << "CS " << id << ' ' << std::quoted(station.name()) << ' '
            << station.totalWorkshops() << ' ' << station.workingWorkshops() << ' '
            << std::quoted(std::string(1, station.stationClass())) << '\n';
    }

    out.close();
    if (!out) {
        std::cout << "Error while writing data file.\n";
        std::cerr << "Save failed while writing " << filename << '\n';
        return;
    }

    std::cout << "Saved " << pipes_.size() << " pipe(s) and " << stations_.size()
        << " CS object(s) to " << filename << ".\n";
    std::cerr << "Saved database: file=" << filename << ", pipes="
        << pipes_.size() << ", CS=" << stations_.size() << '\n';
}

void Pipeline::load() {
    const std::string filename = util::text("File name to load: ");
    std::ifstream in(filename);
    if (!in) { std::cout << "Cannot open file; current data is unchanged.\n"; std::cerr << "Load failed: cannot open " << filename << '\n'; return; }
    std::string line;
    if (!std::getline(in, line) || util::trim(line) != "PIPELINE_DB 1") {
        std::cout << "Invalid file header; current data is unchanged.\n";
        std::cerr << "Load failed: missing/invalid file header in " << filename << '\n'; return;
    }
    std::unordered_map<int, Pipe> pipes;
    std::unordered_map<int, CompressorStation> stations;
    std::unordered_set<int> seen;
    int maxId = 0, lineNo = 1;
    std::string error;
    while (std::getline(in, line)) {
        ++lineNo;
        const auto content = util::trim(line);
        if (content.empty() || content[0] == '#') continue;
        std::istringstream row(content);
        std::string type; int id = 0;
        if (!(row >> type >> id) || id <= 0) { error = "invalid record header"; break; }
        if (!seen.insert(id).second) { error = "duplicate ID"; break; }
        if (type == "PIPE") {
            std::string mark; double length = 0; int diameter = 0, repair = -1;
            if (!(row >> std::quoted(mark) >> length >> diameter >> repair)) { error = "incomplete PIPE"; break; }
            row >> std::ws;
            if (!row.eof() || (repair != 0 && repair != 1)) { error = "invalid PIPE fields"; break; }
            try {
                Pipe pipe(mark, length, diameter, repair == 1);
                pipes.emplace(id, std::move(pipe));
            }
            catch (const std::invalid_argument&) {
                error = "invalid PIPE fields";
                break;
            }
        }
        else if (type == "CS") {
            std::string name, cls; int total = 0, working = 0;
            if (!(row >> std::quoted(name) >> total >> working >> std::quoted(cls))) { error = "incomplete CS"; break; }
            row >> std::ws;
            if (!row.eof() || cls.size() != 1) { error = "invalid CS record"; break; }
            try {
                CompressorStation station(name, total, working, cls[0]);
                stations.emplace(id, std::move(station));
            }
            catch (const std::invalid_argument&) {
                error = "invalid CS fields";
                break;
            }
        }
        else { error = "unknown record type"; break; }
        maxId = std::max(maxId, id);
    }
    if (error.empty() && in.bad()) error = "file read error";
    if (!error.empty()) {
        std::cout << "Load failed at line " << lineNo << ": " << error << ". Data unchanged.\n";
        std::cerr << "Load failed: file=" << filename << ", line=" << lineNo << ", reason=" << error << '\n';
        return;
    }
    pipes_.swap(pipes); stations_.swap(stations); nextId_ = static_cast<long long>(maxId) + 1;
    std::cout << "Loaded " << pipes_.size() << " pipe(s) and " << stations_.size() << " CS object(s).\n";
    std::cerr << "Loaded database: file=" << filename << ", pipes=" << pipes_.size()
        << ", CS=" << stations_.size() << '\n';
}
