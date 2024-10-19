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

namespace regez
{

template <typename T, std::size_t N> class ConstexprStack
{
  public:
    constexpr explicit ConstexprStack() noexcept;
    constexpr void push(const T &value) noexcept;
    constexpr void pop() noexcept;
    constexpr T &top() noexcept;
    constexpr const T &top() const noexcept;
    constexpr std::size_t size() const noexcept;
    constexpr bool empty() const noexcept;
    constexpr bool contains(const T &value) const noexcept;

  private:
    std::size_t m_size;
    T m_data[N];
};

template <typename T, std::size_t N>
constexpr ConstexprStack<T, N>::ConstexprStack() noexcept : m_size(0)
{
}

template <typename T, std::size_t N>
constexpr void ConstexprStack<T, N>::push(const T &value) noexcept
{
    m_data[m_size] = value;
    ++m_size;
}

template <typename T, std::size_t N>
constexpr void ConstexprStack<T, N>::pop() noexcept
{
    if (m_size > 0)
        --m_size;
}

template <typename T, std::size_t N>
constexpr T &ConstexprStack<T, N>::top() noexcept
{
    return m_data[m_size - 1];
}

template <typename T, std::size_t N>
constexpr const T &ConstexprStack<T, N>::top() const noexcept
{
    return m_data[m_size - 1];
}

template <typename T, std::size_t N>
constexpr std::size_t ConstexprStack<T, N>::size() const noexcept
{
    return m_size;
}

template <typename T, std::size_t N>
constexpr bool ConstexprStack<T, N>::empty() const noexcept
{
    return m_size == 0;
}

template <typename T, std::size_t N>
constexpr bool ConstexprStack<T, N>::contains(const T &value) const noexcept
{
    for (std::size_t i = 0; i < m_size; ++i)
    {
        if (m_data[i] == value)
        {
            return true;
        }
    }
    return false;
}

} // namespace regez
