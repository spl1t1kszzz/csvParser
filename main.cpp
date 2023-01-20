#include <iostream>
#include <algorithm>
#include <string>
#include <tuple>
#include <typeinfo>

#include "Parser.h"
#include "Exceptions.h"

// Variadic templates
// std::tuple
// parameter pack - упаковка параметров


int main(int argc, char **argv) {

    if(*argv[1] == '\\')
        *argv[1] = '\n';
    try {
        if (argc < 3) {
            throw Exceptions("Too few program arguments", bad_program_arguments);
        }
        std::ifstream file("test.csv");
        if (!file.is_open()) {
            throw Exceptions("no csv file!", bad_file);
        }
        std::string curString;
        std::getline(file, curString);
        Parser<std::string, short, long, int, double, double, double, double, int> parser(argv, file);
        for (Parser<std::string, short, long, int, double, double, double, double, int> &it: parser) {
            std::cout << it;
            std::cout << std::endl;
        }
    }
    catch (const Exceptions& ex) {
        std::cerr << ex.whatReason() << std::endl;
        return ex.returnCode();
    }
    return 0;
}
