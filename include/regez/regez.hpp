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

#include <regez/operators.hpp>

namespace regez
{

template <class Type> class Vocabulary
{
  public:
    using value_type = Type;
    explicit Vocabulary() noexcept;
    value_type get(const Operators op) const noexcept;
    Vocabulary &&set(Operators op, value_type value) noexcept;

  private:
    std::array<value_type, Operators::_op_max> _vocab;
};

template <class Type> Vocabulary<Type>::Vocabulary() noexcept
{
}

template <class Type>
typename Vocabulary<Type>::value_type
Vocabulary<Type>::get(const Operators op) const noexcept
{
    return _vocab[op];
}

template <class Type>
Vocabulary<Type> &&Vocabulary<Type>::set(Operators op,
                                         value_type value) noexcept
{
    _vocab[op] = value;
    return std::move(*this);
}

template <class Container,
          class Alloc = std::allocator<typename Container::value_type>>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
class Regex
{
  public:
    using value_type = Container::value_type;
    explicit Regex(const Container &pattern,
                   const Vocabulary<value_type> &vocab) noexcept;
    bool match(const Container &text) const noexcept;

  private:
    const Vocabulary<value_type> _vocab;
    const Alloc _alloc;
    Container infix2postfix(const Container &pattern) const noexcept;
};

template <class Container, class Alloc>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
Regex<Container, Alloc>::Regex(
    const Container &pattern,
    const Vocabulary<typename Container::value_type> &vocab) noexcept
    : _vocab(vocab), _alloc()
{
    static_assert(std::is_same<typename Alloc::value_type, value_type>::value,
                  "The allocator must have the same value_type as the "
                  "container's value_type");

    // TODO: Check Correctness of the pattern
    // TODO: Expand the pattern

    [[maybe_unused]] Container rpn = infix2postfix(pattern);

    // TODO: Thompson's construction
    // TODO: NFA to DFA
    // TODO: Minimize the DFA
}

template <class Container, class Alloc>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
bool Regex<Container, Alloc>::match(const Container &text) const noexcept
{
    // TODO: Match the text with the pattern
    return false;
}

template <class Container, class Alloc>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
Container Regex<Container, Alloc>::infix2postfix(
    const Container &pattern) const noexcept
{
    [[maybe_unused]] std::stack<value_type, std::deque<value_type, Alloc>>
        stack;

    // TODO: Implement the infix to postfix algorithm

    return pattern;
}

} // namespace regez
