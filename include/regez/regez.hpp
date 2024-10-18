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

namespace regez
{

enum operators
{
    op_or = 0,
    op_concat,
    op_any,         // *
    op_one_or_more, // +
    op_open_group,  // (
    op_close_group, // )
    op_open_match,  // [
    op_close_match, // ]
    op_escape,
    _op_max
};

template <class Type> class Vocabulary
{
  public:
    using value_type = Type;
    constexpr Vocabulary(
        std::array<value_type, operators::_op_max> vocab) noexcept
        : _vocab(vocab)
    {
    }

  private:
    const std::array<value_type, operators::_op_max> _vocab;
};

/**
 * @brief Regular expression class
 *
 * @tparam Container
 * @tparam Alloc for Container::value_type
 *
 * @note
 * The container must be a sequence container with a value_type member type,
 * must have a constructor and be iterable.
 * The allocator must have the same value_type as the container.
 */
template <class Container,
          class Alloc = std::allocator<typename Container::value_type>,
          std::size_t N = 0 >
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
class Regex
{
  public:
    using value_type = Container::value_type;
    constexpr explicit Regex(const Container &pattern,
                             const Vocabulary<value_type> &vocab) noexcept;
    constexpr bool match(const Container &text) const noexcept;

  private:
    const Vocabulary<value_type> _vocab;
    const Alloc _alloc;
    constexpr Container
    infix_to_postfix(const Container &pattern) const noexcept;
};

template <class Container, class Alloc, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr Regex<Container, Alloc, N>::Regex(
    const Container &pattern,
    const Vocabulary<typename Container::value_type> &vocab) noexcept
    : _vocab(vocab), _alloc()
{
    static_assert(std::is_same<typename Alloc::value_type, value_type>::value,
                  "The allocator must have the same value_type as the "
                  "container");

    // TODO: Check Correctness of the pattern
    // TODO: Expand the pattern
    // TODO: Write the pattern in reverse polish notation

    [[maybe_unused]] Container rpn = infix_to_postfix(pattern);

    // TODO: Thompson's construction
    // TODO: NFA to DFA
    // TODO: Minimize the DFA
}

template <class Container, class Alloc, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr bool
Regex<Container, Alloc, N>::match(const Container &text) const noexcept
{
    // TODO: Match the text with the pattern
    return false;
}

template <class Container, class Alloc, std::size_t N>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr Container Regex<Container, Alloc, N>::infix_to_postfix(
    [[maybe_unused]] const Container &pattern) const noexcept
{
    Container rpn = Container();
    if constexpr (N > 0)
    {
        [[maybe_unused]] regez::ConstexprStack<typename Container::value_type, N> stack;
    }
    else { // runtime
        [[maybe_unused]] std::stack<typename Container::value_type> stack;

    }
    return rpn;
}

} // namespace regez
