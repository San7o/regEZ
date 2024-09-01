/*
 * MIT License
 *
 * Copyright (c) 2024 Giovanni Santini

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 all
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
};

template<typename C>
transition<C>::transition(state<C>* from, state<C>* to, std::optional<C> condition)
        : from(from), to(to), condition(condition) {}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const transition<C> t)
{
    os << "{ \"transition\": { \"from\": " << *t.from << ", \"to\": "
       << *t.to << ", \"condition\": \"" << t.condition.value_or("ε")
       << "\" } }";
    return os;
}

template<typename C>
struct state
{
    int id;
    bool is_final = false;
    std::vector<transition<C>> transitions;

    state() = default;
    state(int id, bool is_final = false);
    void add_transition(state<C>* to, std::optional<C> condition = std::nullopt);
};

template<typename C>
state<C>::state(int id, bool is_final)
        : id(id), is_final(is_final), transitions({}) {}

template<typename C>
void state<C>::add_transition(state<C>* to, std::optional<C> condition)
{
    this->transitions.push_back(transition<C>(this, to, condition));
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const state<C>& s)
{
    os << "{ \"state\": { \"id\": " << s.id << ", \"is_final\": "
       << (s.is_final ? "true" : "false") << " } }";
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os,
                const std::vector<transition<C>>& ts)
{
    auto it = ts.begin();
    if (it != ts.end())
        os << "{ \"transitions\": [" <<*it++;
    for (; it != ts.end(); ++it)
        os << ", " << *it;
    os << "] }";
    return os;
}

template<typename C>
struct regex
{
    state<C> start;
    std::vector<state<C>> states;

    regex() = default;
    regex(const C& reg);
    regex(state<C> start, std::vector<state<C>> states);
    void add_state(state<C> s);
};

template<typename C>
regex<C>::regex(state<C> start, std::vector<state<C>> states)
        : start(start), states(states) {}

template<typename C>
regex<C>::regex(const C& reg)
{
    // TODO: Regex expansion

    // TODO: Regex infix to postfix

    // TODO: Regex postfix to NFA

    // TODO: NFA to DFA

    // TODO: DFA to minimized DFA

    // This is just a placeholder:
    auto tmp = reg;
    this->start = state<C>{1};
    this->states = {};
}

template<typename C>
void regex<C>::add_state(state<C> s)
{
    this->states.push_back(s);
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const regex<C>& r)
{
    os << "{ \"regex\": { \"start\": " << r.start << ", \"num_states\": "
       << r.states.size() << " } }";
    return os;
}

} // namespace regex

template <typename C>
struct std::formatter<regez::state<C>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const regez::state<C>& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(),
                "{{ \"state\": {{ \"id\": {}, \"is_final\": {} }} }}", obj.id,
                (obj.is_final ? "true" : "false") );
    }
};

template <typename C>
struct std::formatter<regez::transition<C>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const regez::transition<C>& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(),
                "{{ \"transition\": {{ \"from\": {}, \"to\": {}, \"condition\": \"{}\" }} }}",
                *obj.from, *obj.to, obj.condition.value_or("ε"));
    }
};

template <typename C>
struct std::formatter<std::vector<regez::transition<C>>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const std::vector<regez::transition<C>>& obj,
                    std::format_context& ctx) const {
        std::format_to(ctx.out(), "{{ \"transitions\": [");
        auto it = obj.begin();
        if (it != obj.end())
            std::format_to(ctx.out(), "{}", *it++);
        for (; it != obj.end(); ++it)
                std::format_to(ctx.out(), ", {}", *it);
        std::format_to(ctx.out(), "] }}");
        return ctx.out();
    }
};

template <typename C>
struct std::formatter<regez::regex<C>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const regez::regex<C>& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(),
                "{{ \"regex\": {{ \"start\": {}, \"num_states\": {} }} }}",
                obj.start, obj.states.size());
    }
};
