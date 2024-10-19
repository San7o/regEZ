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
#include <stack>
#if __cplusplus > 201703L // C++ 17
#include <concepts>
#endif

#include <regez/constexpr_stack.hpp>
#include <regez/constexpr_vector.hpp>
#include <regez/operators.hpp>

namespace regez
{

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
    constexpr static ConstexprVector<value_type, N>
    infix2postfix(const Container &pattern,
                     const VocabularyConstexpr<value_type> &voc);
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

    [[maybe_unused]] ConstexprVector<value_type, N> rpn =
        infix2postfix(pattern, vocab);

    // TODO: Thompson's construction
    // TODO: NFA to DFA
    // TODO: Minimize the DFA
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
    const Container &pattern,
    const VocabularyConstexpr<value_type> &voc)
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
        {  // Terminal symbol
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

} // namespace regez
