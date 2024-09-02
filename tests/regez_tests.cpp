#include <string>
#include <iostream>
#include <sstream>
#include "regez.hpp"

int errors = 0;
int num_assertions = 0;

#define ASSERT(x) \
        num_assertions++; \
        if (!(x)) { \
                std::cerr << "Line " << __LINE__ << " in file " \
                << __FILE__ << ": Assertion failed: " << #x << std::endl; \
                errors++;\
        }

#define ASSERT_EQ(x, y) \
        num_assertions++; \
        if ((x) != (y)) { \
                std::cerr << "Line " << __LINE__ << " in file " \
                << __FILE__ << ": Assertion failed: " << x \
                << " != " << y << std::endl; \
                errors++;\
        }

void test_state()
{
    regez::state<std::string> s1;
    regez::state<std::string> s2;
    regez::state<std::string> s3;
    regez::state<std::string> s4;
    regez::state<std::string> s5;
    ASSERT_EQ(s1.get_id(), 0);
    ASSERT_EQ(s2.get_id(), 1);
    ASSERT_EQ(s3.get_id(), 2);
    ASSERT_EQ(s4.get_id(), 3);
    ASSERT_EQ(s5.get_id(), 4);
}

// test if both printing via stream and std::format work
// and produce the same output
void test_print()
{
    // printing states
    regez::state<std::string> s1;
    std::ostringstream stream;
    stream << s1;
    ASSERT(stream.str() != "");
    ASSERT_EQ(stream.str(), std::format("{}", s1));

    // printing transitions
    regez::state<std::string> s2;
    regez::transition<std::string> t1(&s1, &s2, 'a');
    stream.str("");
    stream << t1;
    ASSERT(stream.str() != "");
    ASSERT_EQ(stream.str(), std::format("{}", t1));

    // printing grammar
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
    stream.str("");
    stream << g;
    ASSERT(stream.str() != "");
    ASSERT_EQ(stream.str(), std::format("{}", g));

    // printing regex
    regez::regex<std::string> r1(std::string("a"), &g);
    stream.str("");
    stream << r1;
    ASSERT(stream.str() != "");
    ASSERT_EQ(stream.str(), std::format("{}", r1));
}

void test_infix_to_postfix()
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

    std::optional<std::string> out;

    // valid regex

    std::string infix = "a";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "a");

    infix = "a|b";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab|");

    infix = "a|b|c|d|e";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab|c|d|e|");

    infix = "(a|b)*.c";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab|*c.");

    infix = "(a.b|c)*";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab.c|*");

    infix = "a.b|c*";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab.c*|");

    infix = "a.b|c*|d";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab.c*|d|");

    infix = "a.b|c*|d|e";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab.c*|d|e|");

    infix = "a*.b";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "a*b.");

    infix = "a.\\*.b";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "a*.b.");

    infix = "(a|b).(c|d.(e|f)*).g";
    out = regez::regex<std::string>::infix_to_postfix(infix, &g);
    ASSERT(out.has_value());
    ASSERT_EQ(out.value(), "ab|cdef|*.|.g.");
}

int main()
{
    test_state();
    test_infix_to_postfix();
    test_print();

    if (errors > 0)
    {
        std::cerr << errors << " tests failed" << std::endl;
        return 1;
    }
    else
    {
        std::cout << num_assertions <<" assertions passed" << std::endl;
        return 0;
    }
}
