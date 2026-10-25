#pragma once
#include <iosfwd>
#include <string>

class Pipe {
public:
    Pipe(std::string mark, double length, int diameter, bool inRepair);

    const std::string& mark() const { return mark_; }
    double length() const { return length_; }
    int diameter() const { return diameter_; }
    bool inRepair() const { return inRepair_; }
    void setInRepair(bool value) { inRepair_ = value; }
    bool valid() const;
    void print(std::ostream& out) const;

private:
    std::string mark_;
    double length_ = 0;
    int diameter_ = 0;
    bool inRepair_ = false;
};

class CompressorStation {
public:
    CompressorStation(std::string name, int total, int working, char stationClass);

    const std::string& name() const { return name_; }
    int totalWorkshops() const { return total_; }
    int workingWorkshops() const { return working_; }
    char stationClass() const { return class_; }
    int unusedWorkshops() const { return total_ - working_; }
    double unusedPercent() const;
    bool startWorkshop();
    bool stopWorkshop();
    bool valid() const;
    void print(std::ostream& out) const;

private:
    std::string name_;
    int total_ = 0;
    int working_ = 0;
    char class_ = 'A';
};
