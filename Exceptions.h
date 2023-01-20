#ifndef CSVPARSER_EXCEPTIONS_H
#define CSVPARSER_EXCEPTIONS_H


#include <exception>
#include <string>
#include <utility>

enum {
    bad_file = 1,
    bad_program_arguments,
    small_csv_file,
    bad_skip,
};



class Exceptions: public std::exception {
    std::string what;
    int code{};
public:
    Exceptions() = default;
    ~Exceptions() = default;
    explicit Exceptions(std::string message, int code): what(std::move(message)), code(code){};
    std::string whatReason() const;
    int returnCode() const;
};

#endif //CSVPARSER_EXCEPTIONS_H
