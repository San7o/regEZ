#include <string>
#include <iostream>
#include "regez.hpp"

int main()
{
    regez::state<std::string> s1(1);
    regez::state<std::string> s2(2);
    regez::transition<regez::state<std::string>, std::string> t(&s1, &s2, std::string("a"));
    s1.add_transition(&s2, std::string("a"));

    std::cout << s1 << std::endl;
    std::cout << "transitions: " << s1.transitions << std::endl;
    
    regez::regex<std::string> r(s1, s2, {});
    std::cout << r << std::endl;

    regez::regex<std::string> r2(std::string("abc"));
    std::cout << r2 << std::endl;

    return 0;
}
