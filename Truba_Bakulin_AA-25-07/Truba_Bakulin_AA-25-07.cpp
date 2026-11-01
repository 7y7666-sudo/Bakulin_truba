#include "Pipeline.h"
#include "utils.h"
#include <exception>
#include <fstream>
#include <iostream>

int main() {
    std::ofstream logfile("user_actions.log", std::ios::app);
    redirect_output_wrapper cerr_out(std::cerr);

    if (logfile)
        cerr_out.redirect(logfile);

    try {
        Pipeline app;
        app.run();
    }
    catch (const std::ios_base::failure&) {
        std::cout << "\nInput ended. Exiting safely.\n";
        std::cerr << "Application exited: end of input\n";
    }
    catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}