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

namespace regez
{

template<typename C>
struct state;

template<typename C>
struct transition;

template<typename C>
struct regex;

template<typename C>
struct transition
{
    state<C>* from;
    state<C>* to;
    std::optional<C> condition;

    transition() = default;
    transition(state<C>* from, state<C>* to, std::optional<C> condition = std::nullopt);

    void set_condition(C condition);
    void set_from(state<C>* from);
    void set_to(state<C>* to);
};

template<typename C>
transition<C>::transition(state<C>* from, state<C>* to, std::optional<C> condition)
        : from(from), to(to), condition(condition) {}

template<typename C>
void transition<C>::set_condition(C condition)
{
    this->condition = condition;
}

template<typename C>
void transition<C>::set_from(state<C>* from)
{
    this->from = from;
}

template<typename C>
void transition<C>::set_to(state<C>* to)
{
    this->to = to;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const transition<C> t)
{
    os << "{ \"transition\": { \"from\": " << *t.from << ", \"to\": "
       << *t.to << ", \"condition\": \"" << t.condition.value_or("ε")
       << "\" } }";
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
struct state
{
    bool is_final;
    std::vector<transition<C>> transitions;

    state();
    state(bool is_final);
    int get_id() const;
    void add_transition(state<C>* to, std::optional<C> condition = std::nullopt);

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
state<C>::state(bool is_final)
        : is_final(is_final), transitions({})
{
    this->id_ = id_count++;
}

template<typename C>
int state<C>::get_id() const
{
    return this->id_;
}

template<typename C>
void state<C>::add_transition(state<C>* to, std::optional<C> condition)
{
    this->transitions.push_back(transition<C>(this, to, condition));
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const state<C>& s)
{
    os << "{ \"state\": { \"id\": " << s.get_id() << ", \"is_final\": "
       << (s.is_final ? "true" : "false") << " } }";
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
    __REGEZ_MAX
};

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
struct grammar
{
    using iter_type = typename C::value_type;
    std::array<std::optional<iter_type>, __REGEZ_MAX> tokens;
    grammar() = default;
    void set_token(iter_type value, grammar_enum type);
};

template<typename C>
void grammar<C>::set_token(iter_type value, grammar_enum type)
{
    this->tokens[type] = value;
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
            if (it->value() == '\\')
                os << "\\\\";
            else
                os << it->value();
        else
            os << "NONE";
        os << "\" }";
    }
    for (++it; it != g.tokens.end(); ++it)
    {
        count++;
        os << ",{ \"token\": \"" << (grammar_enum)count << "\", \"value\": \"";
        if (it->has_value())
            if (it->value() == '\\')
                os << "\\\\";
            else
                os << it->value();
        else
            os << "NONE";
        os << "\" }";
    }
    os << "]}";

    // Escape the es
    return os;
}

template<typename C>
struct regex
{
    state<C> start;
    state<C> end;
    std::vector<state<C>> states;

    regex() = default;
    regex(C reg, grammar<C>* g);
    regex(state<C> start, state<C> end, std::vector<state<C>> states, grammar<C>* g);
    void add_state(state<C> s);

    static std::optional<C> infix_to_postfix(const C& reg, grammar<C>* g);
private:
    grammar<C>* _grammar;
};

template<typename C>
regex<C>::regex(state<C> start, state<C> end, std::vector<state<C>> states, grammar<C>* g)
        : start(start), end(end), states(states), _grammar(g) {}

template<typename C>
regex<C>::regex(C reg, grammar<C>* g)
{
    // TODO: Regex expansion and correctness check

    std::optional<C> pos = infix_to_postfix(reg, g);
    if (!pos.has_value())
    {
        throw std::runtime_error("Error during regex conversion to postfix notation.");
    }

    // TODO: Regex postfix to NFA

    // TODO: NFA to DFA

    // TODO: DFA to minimized DFA

    // This is just a placeholder:
    auto tmp = reg;
    this->start = state<C>();
    this->states = {state<C>(true), state<C>()};
    this->_grammar = g;
}

template<typename C>
void regex<C>::add_state(state<C> s)
{
    this->states.push_back(s);
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
            postfix += g->tokens[REGEZ_ESCAPE].value();
            postfix += r;
            continue;
        }
        bool found = false;
        for (int i = 0; i < __REGEZ_MAX; i++)
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
constexpr std::ostream& operator<<(std::ostream& os, const regex<C>& r)
{
    os << "{ \"regex\": { \"start\": " << r.start << ", \"states\": "
       << r.states << " } }";
    return os;
}

std::string prettify(std::string s)
{
    std::string res;
    int spaces = 0;
    bool is_quoted = false;
    for(auto& c : s) {
        if (c == '\\') {
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
        return std::format_to(ctx.out(),
                "{{ \"transition\": {{ \"from\": {}, \"to\": {}, \"condition\": \"{}\" }} }}",
                *obj.from, *obj.to, obj.condition.value_or("ε"));
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
                std::format_to(ctx.out(), ",{}", *it);
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
                "{{ \"regex\": {{ \"start\": {}, \"states\": {} }} }}",
                obj.start, obj.states);
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
                if (it->value() == '\\')
                    val = "\\\\";
                else
                    val = std::format("{}", it->value());
            else
                val = "NONE";
            std::format_to(ctx.out(), "{{ \"token\": \"{}\", \"value\": \"{}\" }}",
                            (regez::grammar_enum)count, val);
        }
        for (++it; it != obj.tokens.end(); ++it)
        {
            count++;
            if (it->has_value())
                if (it->value() == '\\')
                    val = "\\\\";
                else
                    val = std::format("{}", it->value());
            else
                val = "NONE";
            std::format_to(ctx.out(), ",{{ \"token\": \"{}\", \"value\": \"{}\" }}",
                            (regez::grammar_enum)count, val);
        }
        std::format_to(ctx.out(), "]}}");
        return ctx.out();
    }
};
