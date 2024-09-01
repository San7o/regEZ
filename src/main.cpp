#include <string>
#include <iostream>
#include <print>
#include "regez.hpp"

int main()
{
    regez::state<std::string> s1(1);
    regez::state<std::string> s2(2, true);
    regez::transition<std::string> t(&s1, &s2, std::string("a"));
    s1.add_transition(&s2);

    std::print("{}\n", s1.transitions);
    
    regez::regex<std::string> r2(std::string("abc"));
    std::print("{}\n", r2);

    return 0;
}
