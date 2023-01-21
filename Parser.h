#ifndef CSVPARSER_PARSER_H
#define CSVPARSER_PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <iterator>
#include <limits>
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
            value->line++;
            return *this;
        }

        csvIterator(Iterator *it) : value(it) {};

        csvIterator(const csvIterator &it) : value(it.value) {};
    };

    char lineSeparator{};
    std::ifstream file;
    std::string curString;
    std::vector<std::string> elements;
    std::tuple<Args...> tuple;
    std::vector<int> usedShields;
public:
    static char dataSeparator;
    static char shield;
    static int line;
    void string_to_tuple() {
        bool shieldOpened = false;
        int openIndex = 0, closeIndex = 0, shieldCount, dataSepCount = 0;
        for (auto& i: this->curString) {
            if (!shieldOpened && i == Parser<Args...>::shield) {
                shieldOpened = true;
                shieldCount++;
                continue;
            }
            if (shieldOpened && i == Parser<Args...>::shield) {
                shieldOpened = false;
                shieldCount++;
                continue;
            }
            if (i == Parser<Args...>::dataSeparator) {
                if (!shieldOpened)
                    dataSepCount++;
            }
        }
        if (dataSepCount != sizeof...(Args) - 1)
            throw Exceptions("not enough argumnets in " + std::to_string(this->line) +" line", bad_line);
        for (auto i = 0; i < this->curString.size(); ++i) {
            if (!shieldOpened && curString[i] == shield &&
                (std::find(usedShields.begin(), usedShields.end(), i) == usedShields.end())) {
                shieldOpened = true;
                this->usedShields.push_back(i);
                auto nextShieldIndex = this->curString.find(this->shield, i + 1);
                if (nextShieldIndex == std::string::npos)
                    throw Exceptions("bad shield in " + std::to_string(line) + " line", bad_shield);
                continue;
            }
            if (shieldOpened && curString[i] == shield &&
                (std::find(usedShields.begin(), usedShields.end(), i) == usedShields.end())) {
                shieldOpened = false;
                this->usedShields.push_back(i);
            }
            if (this->curString[i] == this->dataSeparator && this->curString[i] != ' ' && !shieldOpened) {
                this->curString[i] = ' ';
                continue;
            }
            if (this->curString[i] == ' ' && this->curString[i] != this->dataSeparator) {
                this->curString[i] = this->dataSeparator;
            }
        }
        auto os = std::istringstream{this->curString};
        //std::apply([&os](auto &...x) { (os >> ... >> x); }, this->tuple);
        std::apply([&os](auto &...x) { (os >> ... >> x); }, this->tuple);
        std::apply([](auto &&... args) { (((insertSpaces(args))), ...); }, this->tuple);
        if (!os.eof())
            throw Exceptions("bad type of tuple argument in " + std::to_string(Parser<Args...>::line) + " line!", bad_line);
    }


    auto getTuple() const {
        return this->tuple;
    }
    auto static constexpr checkTypes = [](std::istringstream& os, auto& x, auto type) {
//        char symbol = os.get();
//        if (symbol == EOF) {
//            throw Exceptions("bad type of tuple argument in " + std::to_string(Parser<Args...>::line) + " line!", bad_line);
//        }
//        else {
//            os.seekg(-1, std::ios::cur);
//        }
        os >> x;

        return 0;

    };


    auto static constexpr insertSpaces = [](auto &&x) {
        std::string elemType = typeid(x).name();
        //std::cout << elemType << ' ' ;
        std::stringstream os;
        std::string stringX;
        if (elemType.find("string") != std::string::npos) {
            os << x;
            stringX = os.str();
            bool shieldOpened;
            int openIndex, closeIndex;
            std::vector<int> shields;
            for (int i = 0; i < stringX.size(); ++i) {
                if (!shieldOpened && stringX[i] == Parser<Args...>::shield &&
                    (std::find(shields.begin(), shields.end(), i) == shields.end())) {
                    shieldOpened = true;
                    openIndex = i;
                    shields.push_back(i);
                    continue;
                }
                if (shieldOpened && stringX[i] == shield &&
                    (std::find(shields.begin(), shields.end(), i) == shields.end())) {
                    closeIndex = i;
                    shieldOpened = false;
                    shields.push_back(closeIndex);
                    auto dataSubStr = stringX.substr(openIndex + 1, closeIndex - 1);
                    stringX.erase(openIndex, 1);
                    stringX.erase(closeIndex - 1, 1);
                }
                if (stringX[i] == Parser<Args...>::dataSeparator)
                    if (!shieldOpened)
                        stringX[i] = ' ';
            }
            std::istringstream istringstream(stringX);
            if (stringX[0] == ' ') {
                x = ' ';
                for (int i = 1; i < stringX.size(); ++i) {
                    x += stringX[i];
                }
            } else {
                istringstream >> x;
                os.str("");
                os.clear();
                os << x;
                for (auto i = os.str().size(); i < stringX.size(); ++i)
                    x += stringX[i];
            }
        }
    };

    Parser(char **args, std::basic_ios<char> &fin) {
        this->line = 1;
        file.basic_ios<char>::rdbuf(fin.rdbuf());
        this->lineSeparator = *args[1];
        int skip = *args[3] - '0';
        if (skip < 0)
            throw Exceptions("skip value should be more than zero!", bad_skip);
        for (int i = 0; i < skip; ++i) {
            if (!std::getline(this->file, this->curString, this->lineSeparator))
                throw Exceptions("Too few lines in csv", small_csv_file);
            this->line++;
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
auto operator<<(std::ostream &out, const Parser<Args...> &parser) {
    return out << parser.getTuple();
}


#endif //CSVPARSER_PARSER_H
