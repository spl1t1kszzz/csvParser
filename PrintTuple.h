#ifndef CSVPARSER_PRINTTUPLE_H
#define CSVPARSER_PRINTTUPLE_H

#include <iostream>
#include <tuple>

template<std::size_t>
struct int_ {
};

template<class Tuple, typename Ch, typename Tr>
std::ostream &print_tuple(std::basic_ostream<Ch, Tr> &out, const Tuple &t, int_<1>) {
    return out << std::get<std::tuple_size<Tuple>::value - 1>(t);
}

template<class Tuple, typename Ch, typename Tr, std::size_t Pos>
std::ostream &print_tuple(std::basic_ostream<Ch, Tr> &out, const Tuple &t, int_<Pos>) {
        out << std::get<std::tuple_size<Tuple>::value - Pos>(t) << ", ";
    return print_tuple(out, t, int_<Pos - 1>());
}


template<class... Args, typename Ch, typename Tr>
auto operator<<(std::basic_ostream<Ch, Tr> &out, const std::tuple<Args...> &t) {
    out << "(";
    print_tuple(out, t, int_<sizeof...(Args)>());
    out << ")";
    return;
}




#endif //CSVPARSER_PRINTTUPLE_H
