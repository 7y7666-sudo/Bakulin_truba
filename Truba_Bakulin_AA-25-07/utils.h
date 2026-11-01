#pragma once
#include <iostream>
#include <limits>
#include <string>

class redirect_output_wrapper {
    std::ostream& stream_;
    std::streambuf* old_buffer_;

public:
    redirect_output_wrapper(std::ostream& stream)
        : stream_(stream), old_buffer_(stream.rdbuf()) {
    }

    ~redirect_output_wrapper() {
        stream_.rdbuf(old_buffer_);
    }

    void redirect(std::ostream& destination) {
        stream_.rdbuf(destination.rdbuf());
    }
};

namespace util {

    inline std::string trim(const std::string& value) {
        const std::string spaces = " \t\r\n\f\v";
        const auto first = value.find_first_not_of(spaces);
        if (first == std::string::npos) return {};
        const auto last = value.find_last_not_of(spaces);
        return value.substr(first, last - first + 1);
    }

    inline std::string line(const std::string& prompt) {
        std::cout << prompt;
        std::string value;
        if (!std::getline(std::cin, value)) throw std::ios_base::failure("Input ended");
        std::cerr << "Input [" << trim(prompt) << "] = [" << value << "]\n";
        return value;
    }

    template<class T>
    T GetCorrectNumber(T min, T max) {
        T value{};
        std::string extra;

        while (true) {
            if (std::cin >> value) {
                std::getline(std::cin, extra);
                if (extra.find_first_not_of(" \t\r") == std::string::npos &&
                    value >= min && value <= max) {
                    std::cerr << value << '\n';
                    return value;
                }
            }
            else {
                if (std::cin.eof()) throw std::ios_base::failure("Input ended");
                std::cin.clear();
                std::getline(std::cin, extra);
            }

            std::cout << "Type a number between " << min << " and " << max << ": ";
            std::cerr << "Input rejected: invalid number or value out of range\n";
        }
    }

    template<class T>
    T number(const std::string& prompt, T min, T max) {
        std::cout << prompt;
        return GetCorrectNumber<T>(min, max);
    }

    inline double positiveDouble(const std::string& prompt) {
        while (true) {
            const double value = number<double>(prompt, 0.0, std::numeric_limits<double>::max());
            if (value > 0.0) return value;
            std::cout << "Value must be greater than zero.\n";
            std::cerr << "Input rejected: expected a positive number\n";
        }
    }

    inline std::string text(const std::string& prompt) {
        while (true) {
            const auto value = trim(line(prompt));
            if (!value.empty()) return value;
            std::cout << "Text must not be empty.\n";
            std::cerr << "Input rejected: empty text\n";
        }
    }

    inline int id(const std::string& prompt) {
        return number<int>(prompt, 1, std::numeric_limits<int>::max());
    }

    inline bool yesNo(const std::string& prompt) {
        return number<int>(prompt, 0, 1) == 1;
    }

}
