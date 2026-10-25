#include "Pipeline.h"
#include <exception>
#include <iostream>

int main() {
    Logger logger("user_actions.log");
    if (!logger.isOpen()) {
        std::cerr << "Cannot open user_actions.log.\n";
        return 1;
    }
    try {
        Pipeline app(logger);
        app.run();
    }
    catch (const util::EndOfInput&) {
        std::cout << "\nInput ended. Exiting safely.\n";
        logger.log("Application exited: end of input");
    }
    catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << '\n';
        logger.log(std::string("Unexpected exception: ") + error.what());
        return 1;
    }
    return 0;
}
