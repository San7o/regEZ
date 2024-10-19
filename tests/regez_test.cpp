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
                                        .set(regez::Operators::op_or, '|')
                                        .set(regez::Operators::op_any, '.')
                                        .set(regez::Operators::op_concat, '*');
    [[maybe_unused]] regez::Regex<std::string> regez(std::string("a"), vocab);
}

TEST(regez_constructor_constexpr, "regez constructor constexpr")
{
    constexpr regez::VocabularyConstexpr<char> vocab({'|', '.', '*'});
    constexpr std::array<char, 1> regez_str = {'a'};
    [[maybe_unused]] constexpr regez::RegexConstexpr<std::array<char, 1>, 1>
        regez(regez_str, vocab);
}

#ifdef REGEZ_DEBUG
TEST(regez_infix2postfix_constexpr, "regez infix to postfix")
{
    constexpr regez::VocabularyConstexpr<char> vocab(
        {'|', '.', '*', '+', '(', ')', '\\'});
    constexpr regez::ConstexprVector<char, 3> postfix =
        regez::RegexConstexpr<std::string, 3>::infix2postfix(std::string("a|b"),
                                                             vocab);
    static_assert(postfix.size() == 3);
    static_assert(postfix[0] == 'a');
    static_assert(postfix[1] == 'b');
    static_assert(postfix[2] == '|');

    constexpr regez::ConstexprVector<char, 3> postfix2 =
        regez::RegexConstexpr<std::string, 3>::infix2postfix(std::string("a.b"),
                                                             vocab);
    static_assert(postfix2.size() == 3);
    static_assert(postfix2[0] == 'a');
    static_assert(postfix2[1] == 'b');
    static_assert(postfix2[2] == '.');

    constexpr regez::ConstexprVector<char, 2> postfix3 =
        regez::RegexConstexpr<std::string, 2>::infix2postfix(std::string("a*"),
                                                             vocab);
    static_assert(postfix3.size() == 2);
    static_assert(postfix3[0] == 'a');
    static_assert(postfix3[1] == '*');

    constexpr regez::ConstexprVector<char, 6> postfix4 =
        regez::RegexConstexpr<std::string, 6>::infix2postfix(
            std::string("a|b.c*"), vocab);
    static_assert(postfix4.size() == 6);
    static_assert(postfix4[0] == 'a');
    static_assert(postfix4[1] == 'b');
    static_assert(postfix4[2] == 'c');
    static_assert(postfix4[3] == '*');
    static_assert(postfix4[4] == '.');
    static_assert(postfix4[5] == '|');

    constexpr regez::ConstexprVector<char, 8> postfix5 =
        regez::RegexConstexpr<std::string, 8>::infix2postfix(
            std::string("(a|b)*.c"), vocab);
    static_assert(postfix5.size() == 6);
    static_assert(postfix5[0] == 'a');
    static_assert(postfix5[1] == 'b');
    static_assert(postfix5[2] == '|');
    static_assert(postfix5[3] == '*');
    static_assert(postfix5[4] == 'c');
    static_assert(postfix5[5] == '.');

    constexpr regez::ConstexprVector<char, 4> postfix6 =
        regez::RegexConstexpr<std::string, 4>::infix2postfix(
            std::string("a.\\."), vocab);
    static_assert(postfix6.size() == 4);
    static_assert(postfix6[0] == 'a');
    static_assert(postfix6[1] == '\\');
    static_assert(postfix6[2] == '.');
    static_assert(postfix6[3] == '.');
}
#endif
