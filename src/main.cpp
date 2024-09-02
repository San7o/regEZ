#include <string>
#include <iostream>
#include <print>
#include "regez.hpp"

int main()
{
    // Define the regex grammar
    regez::grammar<std::string> g;
    g.set_token('(', regez::REGEZ_OPEN_GROUP);
    g.set_token(')', regez::REGEZ_CLOSE_GROUP);
    g.set_token('[', regez::REGEZ_OPEN_MATCH);
    g.set_token(']', regez::REGEZ_CLOSE_MATCH);
    g.set_token('|', regez::REGEZ_OR);
    g.set_token('*', regez::REGEZ_ANY);
    g.set_token('.', regez::REGEZ_CONCAT);
    g.set_token('+', regez::REGEZ_ONE_OR_MORE);
    g.set_token('\\', regez::REGEZ_ESCAPE);

    std::cout << regez::prettify(std::format("{}\n", g)) << std::endl;

    // Create a regex object
    regez::regex<std::string> r(std::string("(a|b)*.c"), &g);

    std::cout << regez::prettify(std::format("{}\n", r)) << std::endl;

    return 0;
}
