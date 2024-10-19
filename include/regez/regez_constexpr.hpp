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
constexpr Type
VocabularyConstexpr<Type>::get(const Operators op) const noexcept
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

  private:
    constexpr Container
    infix_to_postfix(const Container &pattern,
                     const VocabularyConstexpr<value_type> &voc) const noexcept;
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

    [[maybe_unused]] Container rpn = infix_to_postfix(pattern, vocab);

    // TODO: Thompson's construction
    // TODO: NFA to DFA
    // TODO: Minimize the DFAtype/value mismatch at argument 2 in template
    // parameter list for 'template<class Container, class Alloc> class Regex
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

template <class Container, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr Container RegexConstexpr<Container, N>::infix_to_postfix(
    const Container &pattern,
    const VocabularyConstexpr<value_type> &voc) const noexcept
{
    [[maybe_unused]] Container postfix;
    [[maybe_unused]] regez::ConstexprStack<typename Container::value_type, N>
        ops;
    for (const auto c : pattern)
    {
        if (c == voc.get(Operators::op_or))
        {
        }
    }
    return pattern;
}

} // namespace regez
