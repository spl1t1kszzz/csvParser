#ifndef CSVPARSER_PARSER_H
#define CSVPARSER_PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <iterator>
#include "PrintTuple.h"
#include "Exceptions.h"

template<typename ...Args>
class Parser {
private:
    template<class Iterator>
    class csvIterator {
        friend class Parser;

    public:
        typedef Iterator type;
        typedef type &reference;
        type *value;

        typename csvIterator::reference operator*() const {
            return *value;
        }

        bool operator!=(csvIterator const &another) const {
            return value != another.value;
        }

        bool operator==(csvIterator const &another) const {
            return value == another.value;
        }

        csvIterator &operator++() {
            if (value->file.eof()) {
                *this = nullptr;
                return *this;
            }
            std::getline(value->file, value->curString, value->lineSeparator);
            value->string_to_tuple();
            return *this;
        }

        csvIterator(Iterator *it) : value(it) {};

        csvIterator(const csvIterator &it) : value(it.value) {};
    };

    static char dataSeparator;
    char lineSeparator{};
    std::ifstream file;
    std::string curString;
    std::vector<std::string> elements;
    std::tuple<Args...> tuple;
public:
    void string_to_tuple() {
        for (auto &i: this->curString) {
            int curElemIndex = 0;
            if (i == this->dataSeparator && i != ' ') {
                i = ' ';
                continue;
            }
            if (i == ' ' && i != this->dataSeparator) {
                i = this->dataSeparator;
            }
        }
        auto os = std::istringstream{this->curString};
        std::apply([&os](auto &...x) { (os >> ... >> x); }, this->tuple);
        std::apply([](auto &&... args) { (((std::istringstream(insertSpaces(args))) >> args), ...); }, this->tuple);
    }

public:
    auto getTuple() const {
        return this->tuple;
    }
    static auto constexpr insertSpaces = [](auto &&x) {
        std::string elemType = typeid(x).name();
        std::string stringElem;
        if (elemType.find("string") != std::string::npos) {
            std::ostringstream str;
            str << x;
            stringElem = str.str();
            for (auto &i: stringElem) {
                if (i == Parser<Args...>::dataSeparator)
                    i = 0;
            }
        }
        return stringElem;
    };

    Parser(char **args, std::basic_ios<char> &fin) {
        file.basic_ios<char>::rdbuf(fin.rdbuf());
        this->lineSeparator = *args[1];
        this->dataSeparator = *args[2];
        int skip = *args[3] - '0';
        if (skip < 0)
            throw Exceptions("skip value should be more than zero!", bad_skip);
        for (int i = 0; i < skip; ++i) {
            if (!std::getline(this->file, this->curString, this->lineSeparator))
                throw Exceptions("Too few lines in csv", small_csv_file);
        }
        std::getline(this->file, this->curString, this->lineSeparator);
        this->string_to_tuple();
    }

    csvIterator<Parser> begin() {
        Parser<Args...> *parser = this;
        return csvIterator<Parser>(parser);
    }

    csvIterator<Parser> end() {
        return nullptr;
    }

    Parser() = default;

    ~Parser() = default;

};


template<class ...Args>
char Parser<Args...>::dataSeparator = ';';

template<class ...Args>
auto operator<<(std::ostream &out, const Parser<Args...> &parser) {
    return out << parser.getTuple();
}


#endif //CSVPARSER_PARSER_H
