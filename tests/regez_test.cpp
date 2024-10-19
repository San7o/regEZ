/*
 * MIT License
 *
 * Copyright (c) 2024 Giovanni Santini
 *
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

#include <regez/regez.hpp>
#include <regez/regez_constexpr.hpp>
#include <string>
#include <valfuzz/valfuzz.hpp>

TEST(regez_constructor, "regez constructor")
{
    regez::Vocabulary<char> vocab = regez::Vocabulary<char>()
                                        .set(regez::operators::op_or, '|')
                                        .set(regez::operators::op_any, '*')
                                        .set(regez::operators::op_concat, '.');
    [[maybe_unused]] regez::Regex<std::string> regez(std::string("a"), vocab);
}

TEST(regez_constructor_constexpr, "regez constructor constexpr")
{
    constexpr regez::VocabularyConstexpr<char> vocab({'a', 'b', 'c'});
    constexpr std::array<char, 1> regez_str = {'a'};
    [[maybe_unused]] constexpr regez::RegexConstexpr<std::array<char, 1>, 1>
        regez(regez_str, vocab);
}
