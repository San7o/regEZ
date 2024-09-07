/*
 * MIT License
 *
 * Copyright (c) 2024 Giovanni Santini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <optional>
#include <vector>
#include <iostream>
#include <ostream>
#include <format>
#include <array>
#include <stack>
#include <iterator>
#include <print>

namespace regez
{

#ifdef REGEZ_DEBUG
bool regez_debug = true;
#else
bool regez_debug = false;
#endif

#define DEBUG(...) \
    do { \
        if (regez_debug) \
            std::print(__VA_ARGS__); \
    } while(0)

template<typename C>
struct state;

template<typename C>
struct transition;

template<typename C>
struct closure;

template<typename C>
struct grammar;

template<typename C>
struct regex;

template<typename C>
struct transition
{
    using iter_type = typename C::value_type;
    state<C>* from;
    state<C>* to;
    std::optional<iter_type> condition;

    transition() = default;
    transition(state<C>* from, state<C>* to,
                    std::optional<iter_type> condition = std::nullopt);

    void set_condition(iter_type condition);
    void set_from(state<C>* from);
    void set_to(state<C>* to);
};

template<typename C>
transition<C>::transition(state<C>* state_from, state<C>* state_to,
                std::optional<iter_type> state_condition)
        : from(state_from), to(state_to), condition(state_condition) {}

template<typename C>
void transition<C>::set_condition(iter_type state_condition)
{
    this->condition = state_condition;
}

template<typename C>
void transition<C>::set_from(state<C>* state_from)
{
    this->from = state_from;
}

template<typename C>
void transition<C>::set_to(state<C>* state_to)
{
    this->to = to;
}

template<typename C>
struct closure
{
    using iter_type = typename C::value_type;
    state<C>* start;
    std::vector<std::pair<const state<C>*, iter_type>> states;

    closure() = default;
    closure(state<C>* start);
    void add_state(const state<C>* s, iter_type c);
};

template<typename C>
closure<C>::closure(state<C>* state_start)
        : start(state_start), states({}) {}

template<typename C>
void closure<C>::add_state(const state<C>* s, iter_type c)
{
    this->states.push_back(std::make_pair(s, c));
}

template<typename C>
struct state
{
    using iter_type = typename C::value_type;
    bool is_final;
    std::vector<transition<C>> transitions;
    closure<C> state_closure;

    state();
    state(bool is_final);
    int get_id() const;
    void add_transition(state<C>* to, std::optional<iter_type> condition = std::nullopt);

private:
    int id_;
    static int id_count;
};

template<typename C>
int state<C>::id_count = 0;

template<typename C>
state<C>::state()
        : is_final(false), transitions({})
{
    this->id_ = id_count++;
}

template<typename C>
state<C>::state(bool state_is_final)
        : is_final(state_is_final), transitions({})
{
    this->id_ = id_count++;
}

template<typename C>
int state<C>::get_id() const
{
    return this->id_;
}

template<typename C>
void state<C>::add_transition(state<C>* to, std::optional<typename C::value_type> condition)
{
    this->transitions.push_back(transition<C>(this, to, condition));
}

// In order of precedence (lowest to highest)
enum grammar_enum
{
    REGEZ_OR = 0,         // |
    REGEZ_CONCAT,
    REGEZ_ANY,            // *
    REGEZ_ONE_OR_MORE,    // +
    REGEZ_OPEN_GROUP,     // (
    REGEZ_CLOSE_GROUP,    // )
    REGEZ_OPEN_MATCH,     // [
    REGEZ_CLOSE_MATCH,    // ]
    REGEZ_ESCAPE,         // escape character
    _REGEZ_MAX
};

template<typename C>
struct grammar
{
    using iter_type = typename C::value_type;
    std::array<std::optional<iter_type>, _REGEZ_MAX> tokens;
    grammar() = default;
    void set_token(iter_type value, grammar_enum type);
};

template<typename C>
void grammar<C>::set_token(iter_type value, grammar_enum type)
{
    this->tokens[type] = value;
}

enum regez_error
{
    REGEZ_OK                     = 0,
    REGEZ_INVALID_GROUP          = -1,
    REGEZ_INVALID_MATCH          = -2,
    REGEZ_INVALID_TOKEN_IN_MATCH = -3,
    REGEZ_EMPTY                  = -4,
    _REGEZ_ERROR_MAX
};

template<typename C>
struct regex
{
    state<C> start;
    state<C> end;
    std::vector<regex<C>> regexes;

    regex() {};
    regex(C reg, grammar<C>* g);
    regex(state<C> start, state<C> end, std::vector<regex<C>> regexes, grammar<C>* g);
    void add_regex(regex<C> s = regex<C>());
    void set_grammar(grammar<C>* g);

    // TODO: make non static
    static regez_error check_correctness(const C& reg, grammar<C>* g);
    static std::optional<C> expand(const C& reg, grammar<C>* g);
    static std::optional<C> infix_to_postfix(const C& reg, grammar<C>* g);
    void thompson_algorithm(const C& reg);
    void calculate_dfa();

#ifndef REGEZ_DEBUG
private:
#endif
    regex<C>* regex_from_value(const typename C::value_type& a);
    regex<C>* regex_or(regex<C>& op1, regex<C>& op2);
    regex<C>* regex_concat(regex<C>& op1, regex<C>& op2);
    regex<C>* regex_any(regex<C>& op);
    regex<C>* regex_one_or_more(regex<C>& op);

    grammar<C>* _grammar;
};

template<typename C>
regex<C>::regex(state<C> state_start, state<C> state_end, std::vector<regex<C>> vec_regexes, grammar<C>* g)
        : start(state_start), end(state_end), regexes(vec_regexes), _grammar(g) {}

template<typename C>
void regex<C>::set_grammar(grammar<C>* g)
{
    this->_grammar = g;
}

template<typename C>
regex<C>::regex(C reg, grammar<C>* g)
{
    this->_grammar = g;

    DEBUG("Regex: {}\n", reg);
    regez_error err = check_correctness(reg, g);
    if (err != REGEZ_OK)
    {
        std::cerr << "Error: " << err << std::endl;
        throw std::runtime_error("Invalid regex.");
    }

    std::optional<C> exp = expand(reg, g);
    if (!exp.has_value())
    {
        throw std::runtime_error("Error during regex expansion.");
    }

    std::optional<C> pos = infix_to_postfix(exp.value(), g);
    if (!pos.has_value())
    {
        throw std::runtime_error("Error during regex conversion to postfix notation.");
    }
    DEBUG("Postfix: {}\n", pos.value());

    this->thompson_algorithm(pos.value());
    this->calculate_dfa();

    DEBUG("Regex done\n");

    // TODO: DFA to minimized DFA
}

template<typename C>
void regex<C>::add_regex(regex<C> s)
{
    this->regexes.push_back(s);
}

// check for:
// - correct number of open and close groups
// - correct number of open and close matches
// - no tokens inside matches
// - TODO: more checks
template<typename C>
regez_error regex<C>::check_correctness(const C& reg, grammar<C>* g)
{
    if (reg.empty())
        return REGEZ_EMPTY;

    int n_group = 0;
    int n_match = 0;
    bool is_in_match = false;
    bool escaped = false;

    for (auto& r : reg)
    {
        bool found = false;
        for (int i = 0; i < _REGEZ_MAX; i++)
        {
            if (g->tokens[i].has_value() && g->tokens[i].value() == r)
            {
                switch ((grammar_enum)i) // assuming token has value
                {
                    case REGEZ_OPEN_GROUP:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        if (escaped) escaped = false;
                        else n_group++;
                        break;
                    case REGEZ_CLOSE_GROUP:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        if (escaped) escaped = false;
                        else n_group--;
                        break;
                    case REGEZ_OPEN_MATCH:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        if (escaped) escaped = false;
                        else 
                        { 
                            is_in_match = true;
                            n_match++;
                        }
                        break;
                    case REGEZ_CLOSE_MATCH:
                        if (is_in_match && !escaped)
                        {
                            n_match--;
                            is_in_match = false;
                        }
                        else if (!is_in_match)
                            return REGEZ_INVALID_MATCH;
                        escaped = false;
                        break;
                    case REGEZ_CONCAT:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        escaped = false;
                        break;
                    case REGEZ_OR:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        escaped = false;
                        break;
                    case REGEZ_ANY:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        escaped = false;
                        break;
                    case REGEZ_ONE_OR_MORE:
                        if (is_in_match && !escaped)
                            return REGEZ_INVALID_TOKEN_IN_MATCH;
                        escaped = false;
                        break;
                    case REGEZ_ESCAPE:
                        escaped = !escaped;
                        break;
                    default:
                        break;
                }
                found = true;
                break;
            }
        }
        if (!found) escaped = false;
    }
    if (n_group != 0)
        return REGEZ_INVALID_GROUP;
    if (n_match != 0)
        return REGEZ_INVALID_MATCH;

    return REGEZ_OK;
}

// expand match groups
template<typename C>
std::optional<C> regex<C>::expand(const C& reg, grammar<C>* g)
{
    if (reg.empty())
        return std::nullopt;

    C expanded;
    if (reg.size() == 1)
    {
        expanded += reg;
        return expanded;
    }

    if (!g->tokens[REGEZ_OR].has_value() ||
        !g->tokens[REGEZ_OPEN_MATCH].has_value() ||
        !g->tokens[REGEZ_CLOSE_MATCH].has_value())
        return std::nullopt;

    bool is_in_match = false;
    bool first = false;
    bool escaped = false;
    for (auto& r : reg)
    {
        bool is_token = false;
        if (escaped && is_in_match && !first)
        {
            expanded += g->tokens[REGEZ_OR].value();
            expanded += g->tokens[REGEZ_ESCAPE].value();
            expanded += r;
            escaped = false;
            continue;
        }
        else if (escaped && is_in_match)
        {
            expanded += g->tokens[REGEZ_ESCAPE].value();
            expanded += r;
            escaped = false;
            first = false;
            continue;
        }
        for (int i = 0; i < _REGEZ_MAX; i++)
        {
            if (g->tokens[i].has_value() && g->tokens[i].value() == r)
            {
                is_token = true;
                switch ((grammar_enum)i) // assuming token has value
                {
                    case REGEZ_OPEN_MATCH:
                        first = true;
                        is_in_match = true;
                        break;
                    case REGEZ_CLOSE_MATCH:
                        is_in_match = false;
                        break;
                    case REGEZ_ESCAPE:
                        escaped = !escaped;
                        break;
                    default:
                        if (is_in_match && !first)
                            expanded += g->tokens[REGEZ_OR].value();
                        first = false;
                        expanded += r;
                        break;
                }
                break;
            }
        }
        if (is_in_match && !is_token && !first && !escaped)
            expanded += g->tokens[REGEZ_OR].value();
        if (!is_token && !escaped)
        {
            first = false;
            expanded += r;
        }
    }

    return expanded;
}

#define CHECK_PRECEDENCE(TOKEN) \
if (!token_stack.empty()) \
while(token_stack.top().first >= TOKEN && token_stack.top().first != REGEZ_OPEN_GROUP) \
{ \
    postfix += token_stack.top().second; \
    token_stack.pop(); \
    if (token_stack.empty()) \
        break; \
}

// assuming we have a valid regex in infix notation with explicit concatenation
// and no REGEX_OPEN_MATCH or REGEX_CLOSE_MATCH,
// does not check for validity of the regex
template<typename C>
std::optional<C> regex<C>::infix_to_postfix(const C& reg, grammar<C>* g)
{
    using iter_type = typename C::value_type;
    C postfix;
    std::stack<std::pair<grammar_enum, iter_type>> token_stack;
    bool is_escaped = false;
    for (auto& r : reg)
    {
        if (is_escaped)
        {
            is_escaped = false;
            postfix += r;
            continue;
        }
        bool found = false;
        for (int i = 0; i < _REGEZ_MAX; i++)
        {
            if (g->tokens[i].has_value() && g->tokens[i].value() == r)
            {
                found = true;
                switch ((grammar_enum)i) // assuming token has value
                {
                    case REGEZ_OPEN_GROUP:
                        token_stack.push(std::make_pair(REGEZ_OPEN_GROUP, r));
                        break;
                    case REGEZ_CLOSE_GROUP:
                        if (!token_stack.empty())
                        {
                            iter_type top = token_stack.top().second;
                            token_stack.pop();
                            while (top != g->tokens[REGEZ_OPEN_GROUP].value())
                            {
                                postfix += top;
                                if (token_stack.empty())
                                    break;
                                top = token_stack.top().second;
                                token_stack.pop();
                            }
                        }
                        else
                        {
                            return std::nullopt;
                        }
                        break;
                    case REGEZ_CONCAT:
                        CHECK_PRECEDENCE(REGEZ_CONCAT);
                        token_stack.push(std::make_pair(REGEZ_CONCAT, r));
                        break;
                    case REGEZ_OR:
                        CHECK_PRECEDENCE(REGEZ_OR);
                        token_stack.push(std::make_pair(REGEZ_OR, r));
                        break;
                    case REGEZ_ANY:
                        CHECK_PRECEDENCE(REGEZ_ANY);
                        token_stack.push(std::make_pair(REGEZ_ANY, r));
                        break;
                    case REGEZ_ONE_OR_MORE:
                        CHECK_PRECEDENCE(REGEZ_ONE_OR_MORE);
                        token_stack.push(std::make_pair(REGEZ_ONE_OR_MORE, r));
                        break;
                    case REGEZ_ESCAPE:
                        is_escaped = true;
                        break;
                    default:
                        // Invalid token
                        return std::nullopt;
                }
                break;
            }
        }
        if (!found)
            postfix += r;
    }
    while (!token_stack.empty())
    {
        postfix += token_stack.top().second;
        token_stack.pop();
    }
    return postfix;
}

template<typename C>
regex<C>* regex<C>::regex_from_value(const typename C::value_type& a)
{
    this->regexes.push_back(regex<C>());
    regex<C>* r = &this->regexes.back();
    r->start.add_transition(&r->end,
                    std::optional<typename C::value_type>(a));

    this->start = r->start;
    this->end = r->end;
    return r;
}

template<typename C>
regex<C>* regex<C>::regex_or(regex<C>& op1, regex<C>& op2)
{
    this->add_regex(regex<C>());
    regex<C>* out = &this->regexes.back();
    out->start.add_transition(&op1.start);
    out->start.add_transition(&op2.start);
    op1.end.add_transition(&out->end);
    op2.end.add_transition(&out->end);

    this->start = out->start;
    this->end = out->end;
    return out;
}

template<typename C>
regex<C>* regex<C>::regex_concat(regex<C>& op1, regex<C>& op2)
{
    this->add_regex(regex<C>());
    regex<C>* out = &this->regexes.back();
    out->start.add_transition(&op1.start);
    op1.end.add_transition(&op2.start);
    op2.end.add_transition(&out->end);

    this->start = out->start;
    this->end = out->end;
    return out;
}

template<typename C>
regex<C>* regex<C>::regex_any(regex<C>& op)
{
    this->add_regex(regex<C>());
    regex<C>* out = &this->regexes.back();
    out->start.add_transition(&op.start);
    out->start.add_transition(&out->end);
    op.end.add_transition(&out->start);

    this->start = out->start;
    this->end = out->end;
    return out;
}

template<typename C>
regex<C>* regex<C>::regex_one_or_more(regex<C>& op)
{
    this->add_regex(regex<C>());
    regex<C>* out = &this->regexes.back();
    out->start.add_transition(&op.start);
    op.end.add_transition(&op.start);
    op.end.add_transition(&out->end);

    this->start = out->start;
    this->end = out->end;
    return out;
}

template<typename C>
void regex<C>::thompson_algorithm(const C& reg)
{
    if (this->_grammar == nullptr)
        return;

    using iter_type = typename C::value_type;
    std::stack<regex<C>*> regex_stack;
    for (const iter_type& r : reg)
    {
        bool found = false;
        for (int i = 0; i < _REGEZ_MAX; i++)
        {
            if (this->_grammar->tokens[i].has_value() && this->_grammar->tokens[i].value() == r)
            {
                found = true;
                switch ((grammar_enum)i)
                {
                    case REGEZ_CONCAT:
                        if (regex_stack.size() < 2)
                        {
                            return;
                        }
                        {
                            regex<C>* op1 = regex_stack.top();
                            regex_stack.pop();
                            regex<C>* op2 = regex_stack.top();
                            regex_stack.pop();
                            regex_stack.push(this->regex_concat(*op1, *op2));
                        }
                        break;
                    case REGEZ_OR:
                        if (regex_stack.size() < 2)
                        {
                            return;
                        }
                        {
                            regex<C>* op1 = regex_stack.top();
                            regex_stack.pop();
                            regex<C>* op2 = regex_stack.top();
                            regex_stack.pop();
                            regex_stack.push(this->regex_or(*op1, *op2));
                        }
                        break;
                    case REGEZ_ANY:
                        if (regex_stack.empty())
                        {
                            return;
                        }
                        {
                            regex<C>* op = regex_stack.top();
                            regex_stack.pop();
                            regex_stack.push(this->regex_any(*op));
                        }
                        break;
                    case REGEZ_ONE_OR_MORE:
                        if (regex_stack.empty())
                        {
                            return;
                        }
                        {
                            regex<C>* op = regex_stack.top();
                            regex_stack.pop();
                            regex_stack.push(this->regex_one_or_more(*op));
                        }
                        break;
                    default:
                        return;
                }
                break;
            }
        }
        if (!found)
            regex_stack.push(this->regex_from_value(r));
    }
}

template<typename C>
void regex<C>::calculate_dfa()
{
    std::stack<state<C>*> current_closure;
    std::stack<state<C>*> next_closure;
    std::vector<int> visited_states;
    next_closure.push(&this->start);
    while (!next_closure.empty())
    {
        state<C>* current = next_closure.top();
        next_closure.pop();
        current_closure.push(current);
        while (!current_closure.empty())
        {
            state<C>* tmp = current_closure.top();
            current_closure.pop();
            for (const transition<C> t : tmp->transitions)
            {
                if (!t.condition.has_value()) // epsilon transition
                {
                    if (std::find(visited_states.begin(), visited_states.end(),
                                            t.to->get_id()) != visited_states.end())
                        continue;
                    current_closure.push(t.to);
                    visited_states.push_back(t.to->get_id());
                }
                else
                {
                    tmp->state_closure.add_state(t.to, t.condition.value());
                    if (std::find(visited_states.begin(), visited_states.end(),
                                            t.to->get_id()) != visited_states.end())
                        continue;
                    next_closure.push(t.to);
                    visited_states.push_back(t.to->get_id());
                }
            }
        }
    }
}

template<typename C>
bool operator==(const transition<C>& lhs, const transition<C>& rhs)
{
    return lhs.from == rhs.from &&
            lhs.to == rhs.to &&
            lhs.is_epsilon == rhs.is_epsilon &&
            lhs.condition == rhs.condition;
}

template<typename C>
bool operator==(const state<C>& lhs, const state<C>& rhs)
{
    return lhs.get_id() == rhs.get_id();
}

template<typename C>
bool operator==(const regex<C>& lhs, const regex<C>& rhs)
{
    return lhs.start == rhs.start &&
            lhs.end == rhs.end &&
            lhs.regexes == rhs.regexes;
}

// printing

std::string prettify(std::string s)
{
    std::string res;
    int spaces = 0;
    bool is_quoted = false;
    for(auto& c : s)
    {
        if (c == '\\')
        {
            res += c;
            continue;
        }
        if (c == '"') is_quoted = !is_quoted;
        if (is_quoted)
        {
            res += c;
            continue;
        }
        switch(c)
        {
            case '{':
                res += c;
                res += '\n';
                spaces += 2;
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                break;
            case '}':
                res += '\n';
                spaces -= 2;
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                res += c;
                break;
            case '[':
                res += c;
                res += '\n';
                spaces += 2;
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                break;
            case ']':
                res += '\n';
                spaces -= 2;
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                res += c;
                break;
            case '\n':
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                break;
            case ',':
                res += c;
                res += '\n';
                for(int i = 0; i < spaces; i++)
                    res += ' ';
                break;
            default:
                res += c;
                break;
        }
    }
    return res;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const transition<C> t)
{
    os << "{ \"transition\": { \"from\": " << *t.from << ", \"to\": "
       << *t.to << ", \"condition\": \"";
    if (t.condition.has_value())
    {
        std::string tmp = std::format("{}", t.condition.value());
        os << (tmp == "\\" ? "\\\\" : tmp);
    }
    else
        os << "ε";
    os << "\" } }";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const state<C>& s)
{
    os << "{ \"state\": { \"id\": " << s.get_id() << ", \"is_final\": "
       << (s.is_final ? "true" : "false") << " } }";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const regex<C>& r)
{
    os << "{ \"regex\": { \"start\": " << r.start << ", \"end\": "
       << r.end << ", \"regexes\": " << r.regexes << " } }";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os,
                const std::vector<transition<C>>& ts)
{
    auto it = ts.begin();
    if (it != ts.end())
        os << "[" <<*it++;
    for (; it != ts.end(); ++it)
        os << ", " << *it;
    os << "]";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os,
                const std::vector<state<C>>& ss)
{
    auto it = ss.begin();
    if (it != ss.end())
        os << "[" << *it++;
    for (; it != ss.end(); ++it)
        os << ", " << *it;
    os << "]";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os,
                const std::vector<regex<C>>& rs)
{
    auto it = rs.begin();
    os << "[";
    if (it != rs.end())
        os << *it++;
    for (; it != rs.end(); ++it)
        os << ", " << *it;
    os << "]";
    return os;
}

constexpr std::ostream& operator<<(std::ostream& os, const grammar_enum& ge)
{
    switch (ge)
    {
        case REGEZ_OPEN_GROUP:
            os << "REGEZ_OPEN_GROUP";
            break;
        case REGEZ_CLOSE_GROUP:
            os << "REGEZ_CLOSE_GROUP";
            break;
        case REGEZ_ANY:
            os << "REGEZ_ANY";
            break;
        case REGEZ_OR:
            os << "REGEZ_OR";
            break;
        case REGEZ_CONCAT:
            os << "REGEZ_CONCAT";
            break;
        case REGEZ_ESCAPE:
            os << "REGEZ_ESCAPE";
            break;
        case REGEZ_OPEN_MATCH:
            os << "REGEZ_OPEN_MATCH";
            break;
        case REGEZ_CLOSE_MATCH:
            os << "REGEZ_CLOSE_MATCH";
            break;
        case REGEZ_ONE_OR_MORE:
            os << "REGEZ_ONE_OR_MORE";
            break;
        default:
            os << "UNKNOWN";
            break;
    } 
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const grammar<C>& g)
{
    int count = 0;
    auto it = g.tokens.begin();
    if (it != g.tokens.end())
    {
        os << "{ \"grammar\": [{ \"token\": \"" << (grammar_enum)count << "\", \"value\": \"";
        if (it->has_value())
        {
            std::string tmp = std::format("{}", it->value());
            os << (tmp == "\\" ? "\\\\" : tmp);
        }
        else
            os << "NONE";
        os << "\" }";
    }
    for (++it; it != g.tokens.end(); ++it)
    {
        count++;
        os << ",{ \"token\": \"" << (grammar_enum)count << "\", \"value\": \"";
        if (it->has_value())
        {
            std::string tmp = std::format("{}", it->value());
            os << (tmp == "\\" ? "\\\\" : tmp);
        }
        else
            os << "NONE";
        os << "\" }";
    }
    os << "]}";

    // Escape the es
    return os;
}

constexpr std::ostream& operator<<(std::ostream& os, const regez_error& re)
{
    switch (re)
    {
        case REGEZ_OK:
            os << "REGEZ_OK";
            break;
        case REGEZ_INVALID_GROUP:
            os << "REGEZ_INVALID_GROUP";
            break;
        case REGEZ_INVALID_MATCH:
            os << "REGEZ_INVALID_MATCH";
            break;
        case REGEZ_INVALID_TOKEN_IN_MATCH:
            os << "REGEZ_INVALID_TOKEN_IN_MATCH";
            break;
        case REGEZ_EMPTY:
            os << "REGEZ_EMPTY";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

} // namespace regex

template <typename C>
struct std::formatter<regez::state<C>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::state<C>& obj, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(),
                "{{ \"state\": {{ \"id\": {}, \"is_final\": {} }} }}", obj.get_id(),
                (obj.is_final ? "true" : "false") );
    }
};

template <typename C>
struct std::formatter<regez::transition<C>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::transition<C>& obj, std::format_context& ctx) const
    {
        std::string cond;
        if (obj.condition.has_value())
        {
            std::string tmp = std::format("{}", obj.condition.value());
            cond = (tmp == "\\" ? "\\\\" : tmp);
        }
        else
            cond = "ε";
        return std::format_to(ctx.out(),
                "{{ \"transition\": {{ \"from\": {}, \"to\": {}, \"condition\": \"{}\" }} }}",
                *obj.from, *obj.to, cond);
    }
};

template <typename C>
struct std::formatter<std::vector<regez::transition<C>>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const std::vector<regez::transition<C>>& obj,
                    std::format_context& ctx) const
    {
        std::format_to(ctx.out(), "[");
        auto it = obj.begin();
        if (it != obj.end())
            std::format_to(ctx.out(), "{}", *it++);
        for (; it != obj.end(); ++it)
                std::format_to(ctx.out(), ",{}", *it);
        std::format_to(ctx.out(), "]");
        return ctx.out();
    }
};

template <typename C>
struct std::formatter<std::vector<regez::state<C>>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const std::vector<regez::state<C>>& obj,
                    std::format_context& ctx) const
    {
        std::format_to(ctx.out(), "[");
        auto it = obj.begin();
        if (it != obj.end())
            std::format_to(ctx.out(), "{}", *it++);
        for (; it != obj.end(); ++it)
                std::format_to(ctx.out(), ", {}", *it);
        std::format_to(ctx.out(), "]");
        return ctx.out();
    }
};

template <typename C>
struct std::formatter<regez::regex<C>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::regex<C>& obj, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(),
                "{{ \"regex\": {{ \"start\": {}, \"end\": {}, \"regexes\": {} }} }}",
                obj.start, obj.end, obj.regexes);
    }
};

template <typename C>
struct std::formatter<std::vector<regez::regex<C>>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const std::vector<regez::regex<C>>& obj,
                    std::format_context& ctx) const
    {
        std::format_to(ctx.out(), "[");
        auto it = obj.begin();
        if (it != obj.end())
            std::format_to(ctx.out(), "{}", *it++);
        for (; it != obj.end(); ++it)
                std::format_to(ctx.out(), ", {}", *it);
        std::format_to(ctx.out(), "]");
        return ctx.out();
    }
};

template<>
struct std::formatter<regez::grammar_enum>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::grammar_enum& obj, std::format_context& ctx) const
    {
        switch (obj)
        {
            case regez::REGEZ_OPEN_GROUP:
                return std::format_to(ctx.out(), "REGEZ_OPEN_GROUP");
            case regez::REGEZ_CLOSE_GROUP:
                return std::format_to(ctx.out(), "REGEZ_CLOSE_GROUP");
            case regez::REGEZ_ANY:
                return std::format_to(ctx.out(), "REGEZ_ANY");
            case regez::REGEZ_OR:
                return std::format_to(ctx.out(), "REGEZ_OR");
            case regez::REGEZ_CONCAT:
                return std::format_to(ctx.out(), "REGEZ_CONCAT");
            case regez::REGEZ_ESCAPE:
                return std::format_to(ctx.out(), "REGEZ_ESCAPE");
            case regez::REGEZ_OPEN_MATCH:
                return std::format_to(ctx.out(), "REGEZ_OPEN_MATCH");
            case regez::REGEZ_CLOSE_MATCH:
                return std::format_to(ctx.out(), "REGEZ_CLOSE_MATCH");
            case regez::REGEZ_ONE_OR_MORE:
                return std::format_to(ctx.out(), "REGEZ_ONE_OR_MORE");
            default:
                return std::format_to(ctx.out(), "UNKNOWN");
        }
    }
};

template <>
struct std::formatter<regez::regez_error>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::regez_error& obj, std::format_context& ctx) const
    {
        switch (obj)
        {
            case regez::REGEZ_OK:
                return std::format_to(ctx.out(), "REGEZ_OK");
            case regez::REGEZ_INVALID_GROUP:
                return std::format_to(ctx.out(), "REGEZ_INVALID_GROUP");
            case regez::REGEZ_INVALID_MATCH:
                return std::format_to(ctx.out(), "REGEZ_INVALID_MATCH");
            case regez::REGEZ_INVALID_TOKEN_IN_MATCH:
                return std::format_to(ctx.out(), "REGEZ_INVALID_TOKEN_IN_MATCH");
            case regez::REGEZ_EMPTY:
                return std::format_to(ctx.out(), "REGEZ_EMPTY");
            default:
                return std::format_to(ctx.out(), "UNKNOWN");
        }
    }
};

template <typename C>
struct std::formatter<regez::grammar<C>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const regez::grammar<C>& obj, std::format_context& ctx) const
    {
        std::format_to(ctx.out(), "{{ \"grammar\": [");
        int count = 0;
        auto it = obj.tokens.begin();
        std::string val;
        if (it != obj.tokens.end())
        {
            if (it->has_value())
            {
                std::string tmp = std::format("{}", it->value());
                val = (tmp == "\\" ? "\\\\" : tmp);
            }
            else
                val = "NONE";
            std::format_to(ctx.out(), "{{ \"token\": \"{}\", \"value\": \"{}\" }}",
                            (regez::grammar_enum)count, val);
        }
        for (++it; it != obj.tokens.end(); ++it)
        {
            count++;
            if (it->has_value())
            {
                std::string tmp = std::format("{}", it->value());
                val = (tmp == "\\" ? "\\\\" : tmp);
            }
            else
                val = "NONE";
            std::format_to(ctx.out(), ",{{ \"token\": \"{}\", \"value\": \"{}\" }}",
                            (regez::grammar_enum)count, val);
        }
        std::format_to(ctx.out(), "]}}");
        return ctx.out();
    }
};
