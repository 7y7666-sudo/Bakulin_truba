#include "Models.h"
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace {
    bool hasText(const std::string& value) {
        return value.find_first_not_of(" \t\r\n\f\v") != std::string::npos;
    }
}

Pipe::Pipe(std::string mark, double length, int diameter, bool inRepair)
    : mark_(std::move(mark)), length_(length), diameter_(diameter), inRepair_(inRepair) {
    if (!valid()) {
        throw std::invalid_argument("Invalid pipe data");
    }
}

bool Pipe::valid() const {
    return hasText(mark_) && std::isfinite(length_) && length_ > 0 && diameter_ > 0;
}

void Pipe::print(std::ostream& out) const {
    out << "Mark: " << mark_ << "\nLength: " << length_ << " km\nDiameter: "
        << diameter_ << " mm\nStatus: " << (inRepair_ ? "In repair" : "Operational") << '\n';
}

CompressorStation::CompressorStation(std::string name, int total, int working, char stationClass)
    : name_(std::move(name)), total_(total), working_(working), class_(stationClass) {
    if (!valid()) {
        throw std::invalid_argument("Invalid compressor station data");
    }
}

double CompressorStation::unusedPercent() const {
    return total_ == 0 ? 0.0 : 100.0 * unusedWorkshops() / total_;
}

bool CompressorStation::startWorkshop() {
    if (working_ >= total_) return false;
    ++working_;
    return true;
}

bool CompressorStation::stopWorkshop() {
    if (working_ <= 0) return false;
    --working_;
    return true;
}

bool CompressorStation::valid() const {
    return hasText(name_) && total_ > 0 && working_ >= 0 && working_ <= total_ &&
        class_ >= 33 && class_ <= 126 && class_ != ' ';
}

void CompressorStation::print(std::ostream& out) const {
    const auto flags = out.flags();
    const auto precision = out.precision();
    out << "Name: " << name_ << "\nTotal workshops: " << total_
        << "\nWorking workshops: " << working_ << "\nUnused workshops: " << unusedWorkshops()
        << "\nUnused percentage: " << std::fixed << std::setprecision(2) << unusedPercent()
        << "%\nClass: " << class_ << '\n';
    out.flags(flags);
    out.precision(precision);
}
