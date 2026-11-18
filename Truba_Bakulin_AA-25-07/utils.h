#pragma once
#include <iostream>
#include <string>
#include <limits>

template <typename T>
void readValidNumber(T& value, const std::string& prompt, const T& a, const T& b = std::numeric_limits<T>::max()) {
	while (true) {
		std::cout << prompt;
		if (std::cin.peek() == EOF) {
			std::exit(0);
		}
		if (std::cin.peek() == ' ' || std::cin.peek() == '\t' || std::cin.peek() == '\n') {
			std::cout << "Error: the value starts with a space/tab/line break" << std::endl;
			std::cin.ignore(100, '\n');
			continue;
		}
		std::cin >> value;
		if (std::cin.fail() || std::cin.peek() != '\n') {
			std::cout << "Error: the value must be without any extra characters." << std::endl;
			std::cin.clear();
			std::cin.ignore(100, '\n');
			continue;
		}
		else if (value < a || value > b) {
			std::cout << "Error: the value must be from " << a << " to " << b << std::endl;
			std::cin.ignore(100, '\n');
			continue;
		}
		else {
			std::cin.ignore(100, '\n');
			std::cerr << value << std::endl;
			break;
		}
	}
}

inline void readValidName(std::string& str, const std::string& prompt) {
	while (true) {
		std::cout << prompt;
		if (!getline(std::cin, str)) {
			std::exit(0);
		}
		if (str.empty() || str[0] == ' ' || str[0] == '\t' ||
			str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t') {
			std::cout << "Error: invalid input" << std::endl;
			continue;
		}
		else {
			std::cerr << str << std::endl;
			break;
		}
	}
}

class redirect_output_wrapper {
private:
	std::ostream& stream;
	std::streambuf* const old_buf;
public:
	redirect_output_wrapper(std::ostream& src)
		:stream(src), old_buf(src.rdbuf()) {
	} 
	~redirect_output_wrapper() {
		stream.rdbuf(old_buf);
	} 
	void redirect(std::ostream& target)
	{
		stream.rdbuf(target.rdbuf());
	}
};