#include "Application.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    try {
        Application app{argc, argv};
        return app.run();
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
