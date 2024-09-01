#include <string>
#include <iostream>
#include <print>
#include "regez.hpp"


int main()
{
    regez::state<std::string> s1;
    regez::state<std::string> s2(true);
    regez::transition<std::string> t;
    t.set_from(&s1)
     .set_to(&s2)
     .set_condition(std::string("b"));

    regez::regex<std::string> r2(std::string("abc"));
    std::print("{}\n", r2);

    return 0;
}
