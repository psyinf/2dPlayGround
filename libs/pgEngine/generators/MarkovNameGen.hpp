#pragma once
#include <pgEngine/math/Random.hpp>
#include <pgEngine/generators/markov/MarkovFrequencyMap.hpp>

namespace pg::generators { namespace markov {

static bool isVowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

template <size_t Depth>
static std::string generate(uint8_t                          minLength,
                            uint8_t                          maxLength,
                            const MarkovFrequencyMap<Depth>& frequencyMap,
                            const SeedGenerator&             seed_gen)
{
    const auto map_size = frequencyMap.size();
    if (map_size == 0) { throw std::runtime_error("Empty frequencies"); }
    std::string name;
    auto        start = frequencyMap.key_at(pg::randomBetween<size_t>(0u, map_size - 1, seed_gen));
    const auto  wordLength = pg::randomBetween<uint16_t>(minLength, maxLength, seed_gen);
    name += start;

    for (auto i = 0; i < wordLength; i++)
    {
        Position pos = //
            (i == 0)                ? Position::Start
            : (i == wordLength - 1) ? Position::End
                                    : Position::Within;
        std::vector<char> freq = frequencyMap.getNextStates(start, pos);
        if (freq.empty())
        {
            //
            break;
        }
        // append random letter
        name += freq[pg::randomBetween<size_t>(0u, freq.size() - 1, seed_gen)];
        start = name.substr(name.size() - Depth);
    }

    // check if the first two are consonants and add a ' in between
    if (!isVowel(name[0]) && !isVowel(name[1]))
    {
        name[1] = toupper(name[1]);
        name.insert(1, "'");
    }

    // post process
    name[0] = std::toupper(name[0]);
    return name;
}

}} // namespace pg::generators::markov