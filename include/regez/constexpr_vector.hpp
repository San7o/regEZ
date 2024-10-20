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

namespace regez
{

template <typename T, std::size_t N> class ConstexprVector
{
  public:
    constexpr explicit ConstexprVector() noexcept;
    constexpr void push_back(const T &value) noexcept;
    constexpr void pop_back() noexcept;
    constexpr T &back() noexcept;
    constexpr const T &back() const noexcept;
    constexpr std::size_t size() const noexcept;
    constexpr bool empty() const noexcept;
    constexpr bool contains(const T &value) const noexcept;
    constexpr T operator[](const std::size_t index) const noexcept;
    constexpr std::array<T, N>::const_iterator begin() const noexcept
    {
        return m_data.begin();
    }
    constexpr std::array<T, N>::const_iterator end() const noexcept
    {
        return m_data.begin() + m_size;
    }

  private:
    std::size_t m_size;
    std::array<T, N> m_data;
};

template <typename T, std::size_t N>
constexpr ConstexprVector<T, N>::ConstexprVector() noexcept : m_size(0)
{
}

template <typename T, std::size_t N>
constexpr void ConstexprVector<T, N>::push_back(const T &value) noexcept
{
    m_data[m_size] = value;
    if (m_size < N)
        ++m_size;
}

template <typename T, std::size_t N>
constexpr void ConstexprVector<T, N>::pop_back() noexcept
{
    if (m_size > 0)
        --m_size;
}

template <typename T, std::size_t N>
constexpr T &ConstexprVector<T, N>::back() noexcept
{
    return m_data[m_size - 1];
}

template <typename T, std::size_t N>
constexpr const T &ConstexprVector<T, N>::back() const noexcept
{
    return m_data[m_size - 1];
}

template <typename T, std::size_t N>
constexpr std::size_t ConstexprVector<T, N>::size() const noexcept
{
    return m_size;
}

template <typename T, std::size_t N>
constexpr bool ConstexprVector<T, N>::empty() const noexcept
{
    return m_size == 0;
}

template <typename T, std::size_t N>
constexpr bool ConstexprVector<T, N>::contains(const T &value) const noexcept
{
    for (std::size_t i = 0; i < m_size; ++i)
    {
        if (m_data[i] == value)
            return true;
    }
    return false;
}

template <typename T, std::size_t N>
constexpr T
ConstexprVector<T, N>::operator[](const std::size_t index) const noexcept
{
    return m_data[index];
}

} // namespace regez
