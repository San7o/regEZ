#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <format>

namespace regez
{

template<typename T, typename C>
struct transition
{
    T* from;
    T* to;
    C condition;

    transition() = default;
    transition(T* from, T* to, C condition);
};

template<typename T, typename C>
transition<T, C>::transition(T* from, T* to, C condition)
        : condition(condition)
{
    this->from = from;
    this->to   = to;
}

template<typename T, typename C>
constexpr std::ostream& operator<<(std::ostream& os, const transition<T, C> t)
{
    os << *t.from << "->" << *t.to << ", " << t.condition;
    return os;
}

template<typename C>
struct state
{
    int id;
    std::vector<transition<state<C>, C>> transitions;

    state() = default;
    state(int id);
    void add_transition(state<C>* to, C condition);
};

template<typename C>
state<C>::state(int id) : id(id), transitions({}) {}

template<typename C>
void state<C>::add_transition(state<C>* to, C condition)
{
    this->transitions.push_back(transition<state<C>, C>(this, to, condition));
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const state<C>& s)
{
    os << s.id;
    return os;
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const std::vector<transition<state<C>, C>>& ts)
{
    for (const auto& t : ts)
    {
        os << t << " ";
    }
    return os;
}

template<typename C>
struct regex
{
    state<C> start;
    state<C> end;
    std::vector<state<C>> states;

    regex() = default;
    regex(const C& reg);
    regex(state<C> start, state<C> end, std::vector<state<C>> states);
};

template<typename C>
regex<C>::regex(state<C> start, state<C> end, std::vector<state<C>> states)
        : start(start), end(end), states(states) {}

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
    this->end = state<C>(2);
    this->states = {};
}

template<typename C>
constexpr std::ostream& operator<<(std::ostream& os, const regex<C>& r)
{
    os << r.start << " " << r.end << " " << r.states.size();
    return os;
}

} // namespace regex
