#include <iostream>

// Variadic templates
// std::tuple
// parameter pack - упаковка параметров


using namespace std;

template <class... Args>
void print(Args... args) {
    cout << "true" << endl;
}


int main() {
    print();
    print(1);
    print(1,2, "C++");
    return 0;
}
