#pragma once
#include <pgEngine/math/Random.hpp>
#include <pgEngine/generators/markov/MarkovFrequencyMap.hpp>

namespace pg::generators { namespace markov {
template <size_t Depth>
static std::string generate(uint8_t minLength, uint8_t maxLength, const MarkovFrequencyMap<Depth>& frequencyMap)
{
    const auto map_size = frequencyMap.size();
    if (map_size == 0) { throw std::runtime_error("Empty frequencies"); }
    std::string name;
    auto        start = frequencyMap.key_at(pg::randomBetween<size_t>(0u, map_size - 1));
    const auto  wordLength = pg::randomBetween<uint16_t>(minLength, maxLength);
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
        name += freq[pg::randomBetween<size_t>(0u, freq.size() - 1)];
        start = name.substr(name.size() - Depth);
    }
    return name;
}

}} // namespace pg::generators::markov