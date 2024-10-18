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

#include <memory>
#include <stack>
#if __cplusplus > 201703L // C++ 17
#include <concepts>
#endif

namespace regez
{

enum operators
{
    regez_or = 0,
    regez_concat,
    regez_any,         // *
    regez_one_or_more, // +
    regez_open_group,  // (
    regez_close_group, // )
    regez_open_match,  // [
    regez_close_match, // ]
    regez_escape,
    _regez_max
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
          class Alloc = std::allocator<typename Container::value_type>>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
class regex
{
  public:
    using type = Container::value_type;
    constexpr explicit regex(Container &&pattern) noexcept
    {
        static_assert(std::is_same<typename Alloc::value_type, type>::value,
                      "The allocator must have the same value_type as the "
                      "container");

        // TODO: Check Correctness of the pattern
        // TODO: Expand the pattern
        // TODO: Write the pattern in reverse polish notation

        [[maybe_unused]] Container rpn = infix_to_postfix(std::move(pattern));

        // TODO: Thompson's construction
        // TODO: NFA to DFA
        // TODO: Minimize the DFA
    }

    constexpr bool match(Container &&text) noexcept
    {
        // TODO: Match the text with the pattern
        return false;
    }

  private:
    constexpr Container infix_to_postfix(Container &&pattern) const noexcept;
};

template <class Container, class Alloc>
#if __cplusplus > 201703L // C++ 20
    requires std::default_initializable<Container>
#endif
constexpr Container regex<Container, Alloc>::infix_to_postfix(
    [[maybe_unused]] Container &&pattern) const noexcept
{
    Container rpn;

    // TODO

    return rpn;
}

} // namespace regez
