#include "regez.hpp"

#include <iostream>
#include <print>
#include <string>

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
    regez::regex<std::string> r(std::string("(a|b)*.c"), g);

    std::cout << regez::prettify(std::format("{}\n", r)) << std::endl;

    // Match the regex against a string
    bool found = r.match(std::string("a"));
    std::cout << "Regex \"a\" " << (found ? "found" : "not found") << std::endl;

    // Debug
    std::cout << "first closure: " << r.start->state_closure << std::endl;
    std::cout << "num transitions: " << r.start->transitions.size() << std::endl;
    auto second_state =  r.start->transitions[0].to;
    std::cout << "second closure: " << *second_state->state_closure.states[0].first << std::endl;

    return 0;
}
