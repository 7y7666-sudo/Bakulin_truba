#pragma once
#include "Models.h"
#include "utils.h"
#include <string>
#include <unordered_map>
#include <vector>

class Pipeline {
public:
    explicit Pipeline(Logger& logger) : log_(logger) {}
    void run();
private:
    std::unordered_map<int, Pipe> pipes_;
    std::unordered_map<int, CompressorStation> stations_;
    long long nextId_ = 1;
    Logger& log_;

    int newId();
    void menu() const;
    void addPipe();
    void addStation();
    void viewAll() const;
    void editPipe();
    void editStation();
    void searchPipesName() const;
    void searchPipesStatus() const;
    void searchStationsName() const;
    void searchStationsPercent() const;
    void batchPipes();
    void save() const;
    void load();
    std::vector<int> choosePipeFilter() const;
    std::vector<int> choosePipeTargets(const std::vector<int>& candidates) const;
};

