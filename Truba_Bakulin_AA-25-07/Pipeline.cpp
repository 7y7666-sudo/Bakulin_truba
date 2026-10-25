#include "Pipeline.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace {
    const std::filesystem::path DATA_FILE = "pipeline_data.txt";

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

    template<class Map> void eraseOne(Map& objects, const std::string& label, Logger& log) {
        if (objects.empty()) {
            std::cout << "No " << label << " objects to delete.\n";
            log.log(label + " deletion requested on empty collection");
            return;
        }
        const int id = util::id("Enter " + label + " ID to delete: ", log);
        if (objects.erase(id)) {
            std::cout << label << " deleted.\n";
            log.log(label + " deleted: ID=" + std::to_string(id));
        }
        else {
            std::cout << "ID not found.\n";
            log.log(label + " deletion failed: unknown ID=" + std::to_string(id));
        }
    }

    bool fileReplace(const std::filesystem::path& temp, const std::filesystem::path& target) {
        std::error_code ec;
        std::filesystem::rename(temp, target, ec);
        if (!ec) return true;

        ec.clear();
        if (!std::filesystem::exists(target, ec) || ec) return false;
        if (!std::filesystem::is_regular_file(target, ec) || ec) return false;
        auto backup = target;
        backup += ".bak";
        if (std::filesystem::exists(backup, ec) || ec) return false;
        ec.clear();
        std::filesystem::rename(target, backup, ec);
        if (ec) return false;
        std::filesystem::rename(temp, target, ec);
        if (ec) {
            std::error_code rollback;
            std::filesystem::rename(backup, target, rollback);
            return false;
        }
        std::filesystem::remove(backup, ec);
        return true;
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
        << "Data file: pipeline_data.txt (current working directory)\n";
}

int Pipeline::newId() {
    if (nextId_ > std::numeric_limits<int>::max()) {
        std::cout << "No IDs left.\n";
        log_.log("Object creation failed: ID range exhausted");
        return 0;
    }
    return static_cast<int>(nextId_++);
}

void Pipeline::run() {
    while (true) {
        menu();
        const int choice = util::number<int>("Choose action: ", 0, 14, log_);
        log_.log("Menu action: " + std::to_string(choice));
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
        case 12: eraseOne(pipes_, "Pipe", log_); break;
        case 13: eraseOne(stations_, "CS", log_); break;
        case 14: batchPipes(); break;
        case 0: std::cout << "Exiting.\n"; log_.log("Application exited"); return;
        }
    }
}

void Pipeline::addPipe() {
    const auto mark = util::text("Pipe mark/name: ", log_);
    const double length = util::positiveDouble("Length in km (>0): ", log_);
    const int diameter = util::number<int>("Diameter in mm (>0): ", 1,
        std::numeric_limits<int>::max(), log_);
    const bool inRepair = util::yesNo("In repair? (1 yes, 0 no): ", log_);
    Pipe pipe(mark, length, diameter, inRepair);
    if (!pipe.valid()) {
        std::cout << "Invalid pipe data.\n"; log_.log("Pipe addition rejected by validation"); return;
    }
    const int id = newId();
    if (!id) return;
    pipes_.emplace(id, pipe);
    std::cout << "Pipe added. ID=" << id << '\n';
    log_.log("Pipe added: ID=" + std::to_string(id) + ", mark=" + pipe.mark());
}

void Pipeline::addStation() {
    const std::string name = util::text("CS name: ", log_);
    const int total = util::number<int>("Total workshops (at least 1): ", 1, std::numeric_limits<int>::max(), log_);
    const int working = util::number<int>("Working workshops: ", 0, total, log_);
    char cls = 'A';
    while (true) {
        const auto value = util::trim(util::line("Station class (one non-space character): ", log_));
        if (value.size() == 1 && std::isprint(static_cast<unsigned char>(value[0])) && value[0] != ' ') {
            cls = value[0]; break;
        }
        std::cout << "Enter exactly one printable non-space character.\n";
        log_.log("Invalid station class input");
    }
    CompressorStation station(name, total, working, cls);
    if (!station.valid()) {
        std::cout << "Invalid CS data.\n"; log_.log("CS addition rejected by validation"); return;
    }
    const int id = newId();
    if (!id) return;
    stations_.emplace(id, station);
    std::cout << "CS added. ID=" << id << '\n';
    log_.log("CS added: ID=" + std::to_string(id) + ", name=" + name);
}

void Pipeline::viewAll() const {
    showAll(pipes_, "Pipes");
    showAll(stations_, "Compressor stations");
    log_.log("Viewed all objects: pipes=" + std::to_string(pipes_.size()) +
        ", CS=" + std::to_string(stations_.size()));
}

void Pipeline::editPipe() {
    if (pipes_.empty()) { std::cout << "No pipes to edit.\n"; log_.log("Pipe edit on empty collection"); return; }
    const int id = util::id("Pipe ID: ", log_);
    auto it = pipes_.find(id);
    if (it == pipes_.end()) { std::cout << "ID not found.\n"; log_.log("Pipe edit failed: ID=" + std::to_string(id)); return; }
    const bool status = util::yesNo("Set in repair? (1 yes, 0 no): ", log_);
    it->second.setInRepair(status);
    std::cout << "Pipe updated.\n";
    log_.log("Pipe status updated: ID=" + std::to_string(id) + ", inRepair=" + std::to_string(status));
}

void Pipeline::editStation() {
    if (stations_.empty()) { std::cout << "No CS to edit.\n"; log_.log("CS edit on empty collection"); return; }
    const int id = util::id("CS ID: ", log_);
    auto it = stations_.find(id);
    if (it == stations_.end()) { std::cout << "ID not found.\n"; log_.log("CS edit failed: ID=" + std::to_string(id)); return; }
    const bool started = util::yesNo("Start one workshop? (1 start, 0 stop): ", log_);
    const bool success = started ? it->second.startWorkshop() : it->second.stopWorkshop();
    std::cout << (success ? (started ? "Workshop started.\n" : "Workshop stopped.\n")
        : (started ? "All workshops already work.\n" : "No working workshops.\n"));
    log_.log(std::string("CS workshop operation ") + (started ? "start" : "stop") +
        (success ? " succeeded" : " rejected") + ": ID=" + std::to_string(id));
}

void Pipeline::searchPipesName() const {
    const auto query = util::text("Text to find in pipe name: ", log_);
    const auto found = findIds(pipes_, [&](const Pipe& p) { return p.mark().find(query) != std::string::npos; });
    show(pipes_, found, "Pipe name contains: " + query);
    log_.log("Pipe substring search: query=" + query + ", matches=" + std::to_string(found.size()));
}

void Pipeline::searchPipesStatus() const {
    const bool status = util::yesNo("Filter in repair? (1 yes, 0 no): ", log_);
    const auto found = findIds(pipes_, [=](const Pipe& p) { return p.inRepair() == status; });
    show(pipes_, found, status ? "Pipes in repair" : "Operational pipes");
    log_.log("Pipe repair filter: status=" + std::to_string(status) + ", matches=" + std::to_string(found.size()));
}

void Pipeline::searchStationsName() const {
    const auto query = util::text("Text to find in CS name: ", log_);
    const auto found = findIds(stations_, [&](const CompressorStation& s) { return s.name().find(query) != std::string::npos; });
    show(stations_, found, "CS name contains: " + query);
    log_.log("CS substring search: query=" + query + ", matches=" + std::to_string(found.size()));
}

void Pipeline::searchStationsPercent() const {
    double low = util::number<double>("Minimum unused percent (0-100): ", 0.0, 100.0, log_);
    double high = util::number<double>("Maximum unused percent (0-100): ", 0.0, 100.0, log_);
    while (low > high) {
        std::cout << "Minimum must not exceed maximum.\n";
        log_.log("Invalid unused-workshop percentage range");
        low = util::number<double>("Minimum (0-100): ", 0.0, 100.0, log_);
        high = util::number<double>("Maximum (0-100): ", 0.0, 100.0, log_);
    }
    const auto found = findIds(stations_, [=](const CompressorStation& s) {
        return s.unusedPercent() >= low && s.unusedPercent() <= high;
        });
    std::ostringstream title; title << "CS unused workshop percentage in [" << low << ", " << high << "]";
    show(stations_, found, title.str());
    log_.log("CS percentage filter: min=" + std::to_string(low) + ", max=" +
        std::to_string(high) + ", matches=" + std::to_string(found.size()));
}

std::vector<int> Pipeline::choosePipeFilter() const {
    std::cout << "1 All pipes\n2 Name contains substring\n3 Repair status\n";
    const int filter = util::number<int>("Filter: ", 1, 3, log_);
    if (filter == 1) return ids(pipes_);
    if (filter == 2) {
        const auto query = util::text("Name substring: ", log_);
        return findIds(pipes_, [&](const Pipe& p) { return p.mark().find(query) != std::string::npos; });
    }
    const bool status = util::yesNo("In repair? (1 yes, 0 no): ", log_);
    return findIds(pipes_, [=](const Pipe& p) { return p.inRepair() == status; });
}

std::vector<int> Pipeline::choosePipeTargets(const std::vector<int>& candidates) const {
    std::cout << "1 Apply to all found\n2 Choose IDs\n";
    if (util::number<int>("Target set: ", 1, 2, log_) == 1) return candidates;
    const std::unordered_set<int> allowed(candidates.begin(), candidates.end());
    while (true) {
        std::istringstream input(util::line("IDs separated by spaces: ", log_));
        std::unordered_set<int> selected;
        std::string token;
        bool valid = true;
        while (input >> token) {
            auto value = util::parse<int>(token);
            if (!value || !allowed.count(*value)) { valid = false; break; }
            selected.insert(*value);
        }
        if (valid && !selected.empty()) {
            std::vector<int> result(selected.begin(), selected.end());
            std::sort(result.begin(), result.end());
            return result;
        }
        std::cout << "Choose one or more IDs from the displayed results.\n";
        log_.log("Invalid batch target selection");
    }
}

void Pipeline::batchPipes() {
    if (pipes_.empty()) { std::cout << "No pipes to process.\n"; log_.log("Batch operation on empty pipe collection"); return; }
    const auto candidates = choosePipeFilter();
    show(pipes_, candidates, "Batch candidates");
    if (candidates.empty()) { log_.log("Batch operation cancelled: no matching pipes"); return; }
    const auto targets = choosePipeTargets(candidates);
    std::cout << "1 Edit repair status\n2 Delete\n";
    const int action = util::number<int>("Batch action: ", 1, 2, log_);
    if (action == 1) {
        const bool status = util::yesNo("Set in repair? (1 yes, 0 no): ", log_);
        for (int id : targets) pipes_.at(id).setInRepair(status);
        log_.log("Batch pipe edit: IDs=" + [&] { std::ostringstream s; for (int id : targets) s << id << ' '; return s.str(); }() +
            ", inRepair=" + std::to_string(status));
        std::cout << "Updated " << targets.size() << " pipe(s).\n";
    }
    else {
        for (int id : targets) pipes_.erase(id);
        log_.log("Batch pipe deletion: count=" + std::to_string(targets.size()));
        std::cout << "Deleted " << targets.size() << " pipe(s).\n";
    }
}

//bool Pipeline::isLogPath(const std::filesystem::path& path) const {
//    std::error_code e1, e2;
//    const auto target = std::filesystem::weakly_canonical(path, e1);
//    const auto logPath = std::filesystem::weakly_canonical(
//        std::filesystem::u8path(log_.path()), e2);
//    if (!e1 && !e2) return target == logPath;
//
//    std::error_code e3, e4;
//    const auto absoluteTarget = std::filesystem::absolute(path, e3).lexically_normal();
//    const auto absoluteLog = std::filesystem::absolute(
//        std::filesystem::u8path(log_.path()), e4).lexically_normal();
//    return !e3 && !e4 && absoluteTarget == absoluteLog;
//}

void Pipeline::save() const {
    const std::filesystem::path& target = DATA_FILE;


    auto temp = target;
    temp += ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) {
        std::cout << "Cannot create temporary file in the current working directory. "
            "Check folder permissions.\n";
        log_.log("Save failed: cannot create temporary file for " + target.string());
        return;
    }

    out << "PIPELINE_DB 1\n"
        << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (int id : ids(pipes_)) {
        const auto& p = pipes_.at(id);
        out << "PIPE " << id << ' ' << std::quoted(p.mark()) << ' ' << p.length() << ' '
            << p.diameter() << ' ' << p.inRepair() << '\n';
    }
    for (int id : ids(stations_)) {
        const auto& station = stations_.at(id);
        out << "CS " << id << ' ' << std::quoted(station.name()) << ' '
            << station.totalWorkshops() << ' ' << station.workingWorkshops() << ' '
            << std::quoted(std::string(1, station.stationClass())) << '\n';
    }

    out.close();
    if (!out || !fileReplace(temp, target)) {
        std::error_code ec;
        std::filesystem::remove(temp, ec);
        std::cout << "Save failed; original file was kept where possible.\n";
        log_.log("Save failed while writing/replacing: " + target.string());
        return;
    }

    std::cout << "Saved " << pipes_.size() << " pipe(s) and " << stations_.size()
        << " CS object(s) to " << target.string() << ".\n";
    log_.log("Saved database: file=" + target.string() + ", pipes=" +
        std::to_string(pipes_.size()) + ", CS=" + std::to_string(stations_.size()));
}

void Pipeline::load() {
    const std::filesystem::path& filename = DATA_FILE;

    std::ifstream in(filename);
    if (!in) { std::cout << "Cannot open file; current data is unchanged.\n"; log_.log("Load failed: cannot open " + filename.string()); return; }
    std::string line;
    if (!std::getline(in, line) || util::trim(line) != "PIPELINE_DB 1") {
        std::cout << "Invalid file header; current data is unchanged.\n";
        log_.log("Load failed: missing/invalid file header in " + filename.string()); return;
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
        log_.log("Load failed: file=" + filename.string() + ", line=" + std::to_string(lineNo) + ", reason=" + error);
        return;
    }
    pipes_.swap(pipes); stations_.swap(stations); nextId_ = static_cast<long long>(maxId) + 1;
    std::cout << "Loaded " << pipes_.size() << " pipe(s) and " << stations_.size() << " CS object(s).\n";
    log_.log("Loaded database: file=" + filename.string() + ", pipes=" + std::to_string(pipes_.size()) +
        ", CS=" + std::to_string(stations_.size()));
}
