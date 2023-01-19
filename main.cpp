#include <iostream>
#include <string>
#include <tuple>
#include "PrintTuple.h"

// Variadic templates
// std::tuple
// parameter pack - упаковка параметров


using namespace std;


template <class... Args>
void print(Args... args) { }


template <class Head, class... Args>
void print(Head head, Args... args) {
    cout << head << endl;
    print(args...);
}

int main() {
    auto super_tuple = std::tuple_cat(std::make_tuple(1,2,3), std::make_tuple(4,5,6));
    std::cout << super_tuple << std::endl;
    return 0;
}
