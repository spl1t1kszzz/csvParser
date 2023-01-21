#include <iostream>
#include <algorithm>
#include <string>
#include <ctime>
#include <fstream>
#include "Parser.h"
#include "Exceptions.h"

template <class ...Args> char Parser<Args...>::dataSeparator;
template <class ...Args> char Parser<Args...>::shield;
template <class ...Args> int Parser<Args...>::line = 1;

int main(int argc, char **argv) {
    const char* outname = "C:\\Users\\User\\CLionProjects\\csvParser\\out.txt";
    const char* csvName = "C:\\Users\\User\\CLionProjects\\csvParser\\test.csv";
    clock_t start = clock();
    std::ifstream file(csvName);
    std::ofstream output(outname);
    if (*argv[1] == '\\')
        *argv[1] = '\n';
    if(*argv[4] == '\000')
        *argv[4] = '"';
    try {
        if (!file.is_open())
            throw Exceptions("no csv file!", bad_file);
        if (argc < 3) {
            throw Exceptions("Too few program arguments", bad_program_arguments);
        }
        Parser<std::string, short, long, int, double, double, double, double, int>::dataSeparator = *argv[2];
        Parser<std::string, short, long, int, double, double, double, double, int>::shield = *argv[4];
        Parser<std::string, short, long, int, double, double, double, double, int> parser(argv, file);
        for (Parser<std::string, short, long, int, double, double, double, double, int> &it: parser) {
            output << it;
            output << std::endl;
        }
        //std::cout << (clock() - start)  / CLOCKS_PER_SEC;
    }
    catch (const Exceptions& ex) {
        std::cerr << ex.whatReason() << std::endl;
        std::fstream clear_file(outname, std::ios::out);
        clear_file.close();

        return ex.returnCode();
    }
    return 0;
}
