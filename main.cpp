#include "app.h"

int main(int argc, char** argv)
{
    
    app application;
    try {
        application.init(&argc, argv);
        application.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception was thrown: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    //CUDA::helloWorld();
    return EXIT_SUCCESS;
} 