#pragma once
#include <chrono>
#include <random>
#include <type_traits>

/** Use to generate random numbers with different distribution */
// Freely redistributable, courtesy of learncpp.com (https://www.learncpp.com/cpp-tutorial/global-random-numbers-random-h/)
namespace Random
{
    inline std::mt19937 generate()
    {
        std::random_device rd{};

        // Create seed_seq with clock and 7 random numbers from std::random_device
        std::seed_seq ss{
                static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
                rd(), rd(), rd(), rd(), rd(), rd(), rd() };

        return std::mt19937{ ss };
    }

    inline std::mt19937 mt{ generate() }; // generates a seeded std::mt19937 and copies it into our global object

    template<typename T>
    T GetUniform(T Min, T Max)
    {
        return std::uniform_real_distribution<T>{Min, Max}(mt);
    }

    template<>
    int GetUniform<int>(int Min, int Max)
    {
        return std::uniform_int_distribution<int>{Min, Max}(mt);
    }

    template<typename T>
    T GetNormal(T Mean, T Dev)
    {
        static_assert(!std::is_same_v<T, int>); // only allow double, long double and float
        return std::normal_distribution<T>{Mean, Dev}(mt);
    }

    template<typename T>
    T GetCauchy(T Min, T Max)
    {
        static_assert(!std::is_same_v<T, int>); // only allow double, long double and float
        return std::cauchy_distribution<T>{Min, Max}(mt);

    }
}