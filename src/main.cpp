#include <string>
#include <iostream>
#include <print>
#include "regez.hpp"

int main()
{
    // Define the regex grammar
    regez::grammar<std::string> g;
    g.set_token(std::string("("), regez::REGEZ_OPEN_GROUP);
    g.set_token(std::string(")"), regez::REGEZ_CLOSE_GROUP);
    g.set_token(std::string("["), regez::REGEZ_OPEN_MATCH);
    g.set_token(std::string("]"), regez::REGEZ_CLOSE_MATCH);
    g.set_token(std::string("|"), regez::REGEZ_OR);
    g.set_token(std::string("*"), regez::REGEZ_ANY);
    g.set_token(std::string("."), regez::REGEZ_CONCAT);
    g.set_token(std::string(R"(\\)"), regez::REGEZ_ESCAPE);

    std::cout << regez::prettify(std::format("{}\n", g)) << std::endl;

    // Create a regex object
    regez::regex<std::string> r2(std::string("(a|b)*c"));

    std::cout << regez::prettify(std::format("{}\n", r2)) << std::endl;

    return 0;
}
