#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

class Logger {
public:
    explicit Logger(const std::string& path) : path_(path), file_(path, std::ios::app) {}
    bool isOpen() const { return file_.is_open() && static_cast<bool>(file_); }
    const std::string& path() const { return path_; }
    bool log(const std::string& message) {
        if (!isOpen()) return false;
        file_ << message << '\n';
        file_.flush();
        return static_cast<bool>(file_);
    }
private:
    std::string path_;
    std::ofstream file_;
};

namespace util {
    struct EndOfInput {};

    inline std::string trim(const std::string& s) {
        const auto first = s.find_first_not_of(" \t\r\n\f\v");
        if (first == std::string::npos) return {};
        return s.substr(first, s.find_last_not_of(" \t\r\n\f\v") - first + 1);
    }

    inline std::string line(const std::string& prompt, Logger& log) {
        std::cout << prompt;
        std::string value;
        if (!std::getline(std::cin, value)) throw EndOfInput{};
        log.log("Input [" + trim(prompt) + "] = [" + value + "]");
        return value;
    }

    template<class T> std::optional<T> parse(const std::string& text) {
        std::istringstream in(text);
        T value{};
        if (!(in >> value)) return std::nullopt;
        in >> std::ws;
        if (!in.eof()) return std::nullopt;
        if constexpr (std::is_floating_point_v<T>)
            if (!std::isfinite(value)) return std::nullopt;
        return value;
    }

    template<class T> T number(const std::string& prompt, T low, T high, Logger& log) {
        while (true) {
            const auto value = parse<T>(trim(line(prompt, log)));
            if (value && *value >= low && *value <= high) return *value;
            std::cout << "Invalid number. Allowed range: " << low << ".." << high << ".\n";
            log.log("Input rejected: number out of range or invalid");
        }
    }

    inline double positiveDouble(const std::string& prompt, Logger& log) {
        while (true) {
            const auto value = parse<double>(trim(line(prompt, log)));
            if (value && *value > 0.0) return *value;
            std::cout << "Value must be a finite number greater than zero.\n";
            log.log("Input rejected: expected a positive finite number");
        }
    }
    inline std::string text(const std::string& prompt, Logger& log) {
        while (true) {
            auto value = trim(line(prompt, log));
            if (!value.empty()) return value;
            std::cout << "Text must not be empty.\n";
            log.log("Input rejected: empty text");
        }
    }
    inline int id(const std::string& prompt, Logger& log) {
        return number<int>(prompt, 1, std::numeric_limits<int>::max(), log);
    }
    inline bool yesNo(const std::string& prompt, Logger& log) {
        return number<int>(prompt, 0, 1, log) == 1;
    }
}
