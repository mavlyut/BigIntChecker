#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <random>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "../big_integer.h"
#include "big_integer_gmp.h"

namespace
{
    constexpr size_t NUMBER_OF_ITERATIONS = 10;
    constexpr size_t MAX_SIZE = 2048;

    int64_t shifted_rand()
    {
        int64_t val = rand() - RAND_MAX / 2;
        if (val != 0)
        {
            return val;
        }
        else
        {
            return 1;
        }
    }
} // namespace

TEST(correctness_random, mul_div_randomized)
{
    for (unsigned itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        std::vector<int64_t> multipliers;

        for (size_t i = 0; i != NUMBER_OF_ITERATIONS; ++i)
        {
            multipliers.push_back(shifted_rand());
        }

        big_integer accumulator = 1;

        for (size_t i = 0; i != NUMBER_OF_ITERATIONS; ++i)
        {
            accumulator *= multipliers[i];
        }

        std::shuffle(multipliers.begin(), multipliers.end(),
            std::mt19937(std::random_device()()));

        for (size_t i = 1; i != NUMBER_OF_ITERATIONS; ++i)
            accumulator /= multipliers[i];

        EXPECT_TRUE(accumulator == multipliers[0]);
    }
}

namespace
{
    template <typename T>
    void erase_unordered(std::vector<T>& v, typename std::vector<T>::iterator pos)
    {
        std::swap(v.back(), *pos);
        v.pop_back();
    }

    template <typename T>
    T extract_random_element(std::vector<T>& v)
    {
        size_t index = rand() % v.size();
        T copy = v[index];
        erase_unordered(v, v.begin() + index);
        return copy;
    }

    template <typename T>
    void merge_two(std::vector<T>& v)
    {
        assert(v.size() >= 2);

        T a = extract_random_element(v);
        T b = extract_random_element(v);

        T ab = a * b;
        ASSERT_TRUE(ab / a == b);
        ASSERT_TRUE(ab / b == a);

        v.push_back(ab);
    }

    template <typename T>
    T merge_all(std::vector<T> v)
    {
        assert(!v.empty());

        while (v.size() >= 2)
            merge_two(v);

        return v[0];
    }
} // namespace

TEST(correctness_random, mul_merge_randomized)
{
    for (unsigned itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        std::vector<big_integer> x;
        for (size_t i = 0; i != NUMBER_OF_ITERATIONS; ++i)
            x.emplace_back(shifted_rand());

        big_integer a = merge_all(x);
        big_integer b = merge_all(x);

        EXPECT_TRUE(a == b);
    }
}

namespace
{
    big_integer rand_big(size_t size)
    {
        big_integer result = rand();

        for (size_t i = 0; i != size; ++i)
        {
            result *= RAND_MAX;
            result += rand();
        }

        return result;
    }
} // namespace

TEST(correctness_random, div_randomized)
{
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS * NUMBER_OF_ITERATIONS;
        ++itn)
    {
        big_integer divident = rand_big(10);
        big_integer divisor = rand_big(6);
        big_integer quotient = divident / divisor;
        big_integer residue = divident % divisor;
        ASSERT_EQ(divident - quotient * divisor, residue);
        EXPECT_GE(residue, 0);
        EXPECT_LT(residue, divisor);
    }
}

TEST(correctness_random, cmp)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE, rng);
        big_integer A = big_integer(to_string(a));
        big_integer B = big_integer(to_string(b));
        EXPECT_EQ(a < b, A < B);
        EXPECT_EQ(a > b, A > B);
        EXPECT_EQ(a <= b, A <= B);
        EXPECT_EQ(a >= b, A >= B);
        EXPECT_EQ(a == b, A == B);
    }
}

TEST(correctness_random, add)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE, rng);
        big_integer_gmp c = a + b;
        big_integer R = big_integer(to_string(a)) + big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, sub)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE, rng);
        big_integer_gmp c = a - b;
        big_integer R = big_integer(to_string(a)) - big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, mul)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE, rng);
        big_integer_gmp c = a * b;
        big_integer R = big_integer(to_string(a)) * big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, div)
{
    std::default_random_engine rng(322);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE / 2, rng);
        big_integer_gmp c = a / b;
        big_integer R = big_integer(to_string(a)) / big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));

        c = b / a;
        R = big_integer(to_string(b)) / big_integer(to_string(a));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, mod)
{
    std::default_random_engine rng(322);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE / 2, rng);
        big_integer_gmp c = a % b;
        big_integer R = big_integer(to_string(a)) % big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));

        c = b % a;
        R = big_integer(to_string(b)) % big_integer(to_string(a));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, bitwise)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a, b;
        a.random(MAX_SIZE, rng);
        b.random(MAX_SIZE, rng);
        big_integer_gmp c = a & b;
        big_integer R = big_integer(to_string(a)) & big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));

        c = a | b;
        R = big_integer(to_string(a)) | big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));

        c = a ^ b;
        R = big_integer(to_string(a)) ^ big_integer(to_string(b));
        EXPECT_EQ(to_string(c), to_string(R));
    }
}

TEST(correctness_random, bit_shifts)
{
    std::default_random_engine rng(42);
    for (size_t itn = 0; itn != NUMBER_OF_ITERATIONS; ++itn)
    {
        big_integer_gmp a;
        a.random(MAX_SIZE, rng);
        int shift = shifted_rand() % MAX_SIZE;
        big_integer R = big_integer(to_string(a));

        EXPECT_EQ(to_string(a << shift), to_string(R << shift));
        EXPECT_EQ(to_string(a >> shift), to_string(R >> shift));
    }
}
