#include "app.h"


int main()
{
    app application;
    try {
        application.init();
        application.run();
    }
    catch (const std::exception& e) {
        std::cerr << "I TOLD YOU: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
} 