#include <string>
#include <iostream>
#include <sstream>
#include "regez.hpp"
#include "test.hpp"

int errors = 0;
int num_assertions = 0;

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

void test_correctness()
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

    regez::regez_error res;
    // valid regex
    res = regez::regex<std::string>::check_correctness(std::string("a"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("a|b"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("a|b|c|d|e"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("(a|b)*c"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("(a.b|c)*"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("[abc]"), &g);
    ASSERT(res == regez::REGEZ_OK);
    res = regez::regex<std::string>::check_correctness(std::string("[a\\.b]"), &g);
    ASSERT(res == regez::REGEZ_OK);

    // invalid regex
    res = regez::regex<std::string>::check_correctness(std::string(""), &g);
    ASSERT(res == regez::REGEZ_EMPTY);
    res = regez::regex<std::string>::check_correctness(std::string("("), &g);
    ASSERT(res == regez::REGEZ_INVALID_GROUP);
    res = regez::regex<std::string>::check_correctness(std::string(")"), &g);
    ASSERT(res == regez::REGEZ_INVALID_GROUP);
    res = regez::regex<std::string>::check_correctness(std::string("["), &g);
    ASSERT(res == regez::REGEZ_INVALID_MATCH);
    res = regez::regex<std::string>::check_correctness(std::string("]"), &g);
    ASSERT(res == regez::REGEZ_INVALID_MATCH);
    res = regez::regex<std::string>::check_correctness(std::string("(()"), &g);
    ASSERT(res == regez::REGEZ_INVALID_GROUP);
    res = regez::regex<std::string>::check_correctness(std::string("(a|b))"), &g);
    ASSERT(res == regez::REGEZ_INVALID_GROUP);
}

void test_expansion()
{
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

    auto r = regez::regex<std::string>::expand(std::string("a"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "a");
    r = regez::regex<std::string>::expand(std::string("ab"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "ab");
    r = regez::regex<std::string>::expand(std::string("abc"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "abc");
    r = regez::regex<std::string>::expand(std::string("a|b"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "a|b");
    r = regez::regex<std::string>::expand(std::string("[ab]"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "a|b");
    r = regez::regex<std::string>::expand(std::string("[ab\\.c]"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "a|b|\\.|c");
    r = regez::regex<std::string>::expand(std::string("(a|b)*"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "(a|b)*");
    r = regez::regex<std::string>::expand(std::string("[abc\\[de].f.g"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "a|b|c|\\[|d|e.f.g");
    r = regez::regex<std::string>::expand(std::string("[\\.abc].d"), &g);
    ASSERT(r.has_value());
    ASSERT_EQ(r.value(), "\\.|a|b|c.d");
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

void test_nfa_value()
{
    auto r = regez::regex<std::string>();
    regez::regex<std::string>* a = r.regex_from_value('a');
    ASSERT(r.start.transitions.size() == 1);
    ASSERT(r.start.transitions[0].condition.has_value());
    ASSERT_EQ(r.start.transitions[0].condition.value(), 'a');
    ASSERT(*r.start.transitions[0].to == r.end);
}

void test_nfa_or()
{
    auto r = regez::regex<std::string>();
    regez::regex<std::string>* a = r.regex_from_value('a');
    regez::regex<std::string>* b = r.regex_from_value('b');
    auto* _ = r.regex_or(*a, *b);
    ASSERT(r.start.transitions.size() == 2);
    ASSERT(!r.start.transitions[0].condition.has_value());
    ASSERT(*r.start.transitions[0].to == a->start);
    ASSERT(!r.start.transitions[1].condition.has_value());
    ASSERT(*r.start.transitions[1].to == b->start);
    auto a_start = r.start.transitions[0].to;
    auto b_start = r.start.transitions[1].to;
    ASSERT(*a_start == a->start);
    ASSERT(*b_start == b->start);
}

void test_nfa_concat()
{
    auto r = regez::regex<std::string>();
    regez::regex<std::string>* a = r.regex_from_value('a');
    regez::regex<std::string>* b = r.regex_from_value('b');
    auto* _ = r.regex_concat(*a, *b);
    ASSERT(r.start.transitions.size() == 1);
    ASSERT(!r.start.transitions[0].condition.has_value());
    ASSERT(*r.start.transitions[0].to == a->start);
    auto a_start = r.start.transitions[0].to;
    ASSERT(*a_start == a->start);
}

void test_nfa_any()
{
    auto r = regez::regex<std::string>();
    regez::regex<std::string>* a = r.regex_from_value('a');
    auto* _ = r.regex_any(*a);
    ASSERT(r.start.transitions.size() == 2);
    ASSERT(!r.start.transitions[0].condition.has_value());
    ASSERT(*r.start.transitions[0].to == a->start);
    ASSERT(!r.start.transitions[1].condition.has_value());
    ASSERT(*r.start.transitions[1].to == r.end);
}

void test_nfa_one_or_more()
{
    auto r = regez::regex<std::string>();
    regez::regex<std::string>* a = r.regex_from_value('a');
    auto* _ = r.regex_one_or_more(*a);
    ASSERT(r.start.transitions.size() == 1);
    ASSERT(!r.start.transitions[0].condition.has_value());
    ASSERT(*r.start.transitions[0].to == a->start);
    ASSERT(a->end.transitions.size() == 2);
    ASSERT(!a->end.transitions[0].condition.has_value());
    ASSERT(*a->end.transitions[0].to == a->start);
    ASSERT(!a->end.transitions[1].condition.has_value());
    ASSERT(*a->end.transitions[1].to == r.end);
}

void test_thompson_algorithm()
{
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
    
    auto r = regez::regex<std::string>();
    auto old = r;
    r.set_grammar(&g);
    r.thompson_algorithm(std::string("a"));
    ASSERT(old.start != r.start);
}


void test_nfa()
{
    test_nfa_value();
    test_nfa_or();
    test_nfa_concat();
    test_nfa_any();
    test_nfa_one_or_more();
    test_thompson_algorithm();
}

void test_dfa()
{
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

    auto r = regez::regex<std::string>();
    r.set_grammar(&g);
    r.thompson_algorithm(std::string("a"));
    r.calculate_dfa();
    ASSERT(r.start.state_closure.states.size() > 0);

    r = regez::regex<std::string>();
    r.set_grammar(&g);
    r.thompson_algorithm(std::string("ab|"));
    r.calculate_dfa();
    ASSERT(r.start.state_closure.states.size() > 0);
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

int main()
{
    test_state();
    test_correctness();
    test_expansion();
    test_infix_to_postfix();
    test_nfa();
    test_dfa();
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
