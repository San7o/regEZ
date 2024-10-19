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

#include <array>
#include <memory>
#include <utility>
#if __cplusplus > 201703L // C++ 17
#include <concepts>
#endif

#include <regez/constexpr_stack.hpp>
#include <regez/constexpr_vector.hpp>
#include <regez/operators.hpp>

namespace regez
{

template <typename T>
#if __cplusplus > 201703L // C++ 20
    requires std::copy_constructible<T>
#endif
constexpr T pow(const T _base, const T _exp)
{
    T result = 1;
    T exp = _exp;
    T base = _base;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

template <class Type> class VocabularyConstexpr
{
  public:
    using value_type = Type;
    constexpr VocabularyConstexpr(
        std::array<value_type, Operators::_op_max> vocab) noexcept
        : _vocab(vocab)
    {
    }
    constexpr value_type get(const Operators op) const noexcept;

  private:
    const std::array<value_type, Operators::_op_max> _vocab;
};

template <class Type>
constexpr Type VocabularyConstexpr<Type>::get(const Operators op) const noexcept
{
    return _vocab[op];
}

typedef long unsigned int StateID;

template <class T> class Transition
{
  public:
    using value_type = T;
    constexpr explicit Transition() noexcept = default;
    constexpr Transition(StateID from, StateID to, T symbol) noexcept;
    constexpr Transition(StateID from, StateID to, bool epsilon) noexcept;

  private:
    StateID _from;
    StateID _to;
    T _symbol;
    bool _epsilon;
};

template <class T>
constexpr Transition<T>::Transition(StateID from, StateID to, T symbol) noexcept
    : _from(from), _to(to), _symbol(symbol)
{
}

template <class T>
constexpr Transition<T>::Transition(StateID from, StateID to,
                                    bool epsilon) noexcept
    : _from(from), _to(to), _symbol(T()), _epsilon(epsilon)
{
}

template <class T, std::size_t N> class StateMachine
{
  public:
    using value_type = T;
    constexpr explicit StateMachine() noexcept = default;
    constexpr StateID add_state() noexcept;
    constexpr void add_transition(StateID from, StateID to,
                                          T symbol) noexcept;
    constexpr void add_epsilon_transition(StateID from,
                                                StateID to) noexcept;
#ifndef REGEZ_DEBUG
  private:
#endif
    ConstexprVector<StateID, N> _states;
    ConstexprVector<Transition<T>, N * N> _transitions;
};

template <class T, std::size_t N>
constexpr StateID StateMachine<T, N>::add_state() noexcept
{
    StateID new_state = _states.size();
    _states.push_back(new_state);
    return new_state;
}

template <class T, std::size_t N>
constexpr void StateMachine<T, N>::add_transition(StateID from,
                                                           StateID to,
                                                           T symbol) noexcept
{
    Transition<T> new_transition(from, to, symbol);
    _transitions.push_back(new_transition);
    return;
}

template <class T, std::size_t N>
constexpr void StateMachine<T, N>::add_epsilon_transition(StateID from,
                                                           StateID to) noexcept
{
    Transition<T> new_transition(from, to, true);
    _transitions.push_back(new_transition);
    return;
}

template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
class RegexConstexpr
{
  public:
    using value_type = Container::value_type;
    constexpr explicit RegexConstexpr(
        const Container &pattern,
        const VocabularyConstexpr<value_type> &vocab) noexcept;
    constexpr bool
    match(const Container &text,
          const VocabularyConstexpr<value_type> &vocab) const noexcept;
#ifndef REGEZ_DEBUG
  private:
#endif
    StateMachine<value_type, pow((std::size_t) 2, N)> _sm;
    constexpr static ConstexprVector<value_type, N>
    infix2postfix(const Container &pattern,
                  const VocabularyConstexpr<value_type> &voc);
    constexpr static StateMachine<value_type, pow((std::size_t) 2, N)>
    thompson_construction(const ConstexprVector<value_type, N> &rpn,
                          const VocabularyConstexpr<value_type> &voc) noexcept;
};

template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr RegexConstexpr<Container, N>::RegexConstexpr(
    const Container &pattern,
    const VocabularyConstexpr<typename Container::value_type> &vocab) noexcept
{
    // TODO: Check Correctness of the pattern
    // TODO: Expand the pattern

    ConstexprVector<value_type, N> rpn = infix2postfix(pattern, vocab);

    StateMachine<value_type, pow((std::size_t) 2, N)> sm =
        thompson_construction(rpn, vocab);

    // TODO: NFA to DFA
    // TODO: Minimize the DFA

    _sm = sm;
}

template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr bool RegexConstexpr<Container, N>::match(
    const Container &text,
    const VocabularyConstexpr<value_type> &voc) const noexcept
{
    // TODO: Match the text with the pattern
    return false;
}

// Assuming a well-formed pattern with no open match or close match tokens
template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr ConstexprVector<typename Container::value_type, N>
RegexConstexpr<Container, N>::infix2postfix(
    const Container &pattern, const VocabularyConstexpr<value_type> &voc)
{
    regez::ConstexprVector<value_type, N> postfix;
    regez::ConstexprStack<Operators, N> ops;
    bool is_escaped = false;
    for (const auto &c : pattern)
    {
        if (is_escaped)
        {
            postfix.push_back(voc.get(Operators::op_escape));
            postfix.push_back(c);
            is_escaped = false;
            continue;
        }

        if (c == voc.get(Operators::op_open_group))
        {
            ops.push(Operators::op_open_group);
        }
        else if (c == voc.get(Operators::op_close_group))
        {
            while (!ops.empty() && Operators::op_open_group != ops.top())
            {
                postfix.push_back(voc.get(ops.top()));
                ops.pop();
            }
            ops.pop();
        }
        else if (c == voc.get(Operators::op_escape))
        {
            is_escaped = true;
        }
        else if (c == voc.get(Operators::op_any))
        {
            if (ops.empty() || Operators::op_any > ops.top()
                || ops.contains(Operators::op_open_group))
            {
                ops.push(Operators::op_any);
            }
            else
            {
                while (!ops.empty() && Operators::op_any <= ops.top())
                {
                    postfix.push_back(voc.get(ops.top()));
                    ops.pop();
                }
                ops.push(Operators::op_any);
            }
        }
        else if (c == voc.get(Operators::op_one_or_more))
        {
            if (ops.empty() || Operators::op_one_or_more > ops.top()
                || ops.contains(Operators::op_open_group))
            {
                ops.push(Operators::op_one_or_more);
            }
            else
            {
                while (!ops.empty() && Operators::op_one_or_more <= ops.top())
                {
                    postfix.push_back(voc.get(ops.top()));
                    ops.pop();
                }
                ops.push(Operators::op_one_or_more);
            }
        }
        else if (c == voc.get(Operators::op_or))
        {
            if (ops.empty() || Operators::op_or > ops.top()
                || ops.contains(Operators::op_open_group))
            {
                ops.push(Operators::op_or);
            }
            else
            {
                while (!ops.empty() && Operators::op_or <= ops.top())
                {
                    postfix.push_back(voc.get(ops.top()));
                    ops.pop();
                }
                ops.push(Operators::op_or);
            }
        }
        else if (c == voc.get(Operators::op_concat))
        {
            if (ops.empty() || Operators::op_concat > ops.top()
                || ops.contains(Operators::op_open_group))
            {
                ops.push(Operators::op_concat);
            }
            else
            {
                while (!ops.empty() && Operators::op_concat <= ops.top())
                {
                    postfix.push_back(voc.get(ops.top()));
                    ops.pop();
                }
                ops.push(Operators::op_concat);
            }
        }
        else
        { // Terminal symbol
            postfix.push_back(c);
        }
    }
    while (!ops.empty())
    {
        postfix.push_back(voc.get(ops.top()));
        ops.pop();
    }
    return postfix;
}

template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr StateMachine<typename Container::value_type, pow((std::size_t) 2, N)>
RegexConstexpr<Container, N>::thompson_construction(
    const ConstexprVector<typename Container::value_type, N> &rpn,
    const VocabularyConstexpr<typename Container::value_type> &voc) noexcept
{
    auto sm =
        StateMachine<typename Container::value_type, pow((std::size_t) 2, N)>();
    ConstexprStack<std::pair<StateID, StateID>, N> state_stack;
    for (std::size_t i = 0; i < rpn.size(); ++i)
    {
        value_type s = rpn[i];
        if (s == voc.get(Operators::op_any))
        {
            if (state_stack.empty()) // Not enough operands
            {
                return sm;
            }

            std::pair<StateID, StateID> regex = state_stack.top();
            StateID state_from = sm.add_state();
            StateID state_to = sm.add_state();
            sm.add_epsilon_transition(state_from, regex.first);
            sm.add_epsilon_transition(regex.second, state_to);
            sm.add_epsilon_transition(state_from, state_to);
            sm.add_epsilon_transition(regex.second, regex.first);
            state_stack.push(std::make_pair(state_from, state_to));
        }
        else if (s == voc.get(Operators::op_one_or_more))
        {
            if (state_stack.empty()) // Not enough operands
            {
                return sm;
            }

            std::pair<StateID, StateID> regex = state_stack.top();
            StateID state_from = sm.add_state();
            StateID state_to = sm.add_state();
            sm.add_epsilon_transition(state_from, regex.first);
            sm.add_epsilon_transition(regex.second, state_to);
            sm.add_epsilon_transition(regex.second, regex.first);
            state_stack.push(std::make_pair(state_from, state_to));
        }
        else if (s == voc.get(Operators::op_or))
        {
            if (state_stack.size() < 2) // Not enough operands
            {
                return sm;
            }

            std::pair<StateID, StateID> regex_a = state_stack.top();
            state_stack.pop();
            std::pair<StateID, StateID> regex_b = state_stack.top();
            state_stack.pop();

            StateID state_from = sm.add_state();
            StateID state_to = sm.add_state();
            sm.add_epsilon_transition(state_from, regex_a.first);
            sm.add_epsilon_transition(state_from, regex_b.first);
            sm.add_epsilon_transition(regex_a.second, state_to);
            sm.add_epsilon_transition(regex_b.second, state_to);
            state_stack.push(std::make_pair(state_from, state_to));
        }
        else if (s == voc.get(Operators::op_concat))
        {
            if (state_stack.size() < 2) // Not enough operands
            {
                return sm;
            }

            std::pair<StateID, StateID> regex_a = state_stack.top();
            state_stack.pop();
            std::pair<StateID, StateID> regex_b = state_stack.top();
            state_stack.pop();
            sm.add_epsilon_transition(regex_b.second, regex_a.first);
            state_stack.push(std::make_pair(regex_b.first, regex_a.second));
        }
        else // Terminal symbol
        {
            StateID state_from = sm.add_state();
            StateID state_to = sm.add_state();
            sm.add_transition(state_from, state_to, s);
            state_stack.push(std::make_pair(state_from, state_to));
        }
    }
    return sm;
}

} // namespace regez
