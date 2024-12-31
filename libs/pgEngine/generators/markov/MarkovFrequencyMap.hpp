#pragma once

#include <algorithm>
#include <ranges>
#include <string>
#include <unordered_map>

#include <pgf/strings/StringTools.hpp>

namespace pg::generators {

struct Frequency
{
    size_t start{};
    size_t within{};
    size_t end{};
};

enum class Position
{
    Start,
    Within,
    End
};

enum class Strategy
{
    OnlyStarts,
    ReturnAny,
};
using LetterFrequency = std::unordered_map<char, Frequency>;
using FrequencyMap = std::unordered_map<std::string, LetterFrequency>;

class MarkovFrequencyMap
{
public:
    MarkovFrequencyMap(size_t depth = 4)
      : depth(depth)
    {
    }

    MarkovFrequencyMap(FrequencyMap frequencyMap, size_t depth = 4)
    {
        this->frequencyMap = std::move(frequencyMap);
        this->depth = depth;
    }

    const size_t getDepth() const;

    void add(std::string word);

    std::string key_at(size_t index) const;

    size_t size() const;

    std::vector<std::string> getStartStates() const;

    std::vector<char> getNextStates(const std::string& key, Position pos) const;

    auto getFrequencyMap() const -> const FrequencyMap& { return frequencyMap; }

private:
    bool filterWord(std::string& word);

    void addWord(const std::string& word);

    void addFrequency(const std::string& key, const char& letter, const Frequency& frequency);

    FrequencyMap frequencyMap;
    size_t       depth = 4;
};

} // namespace pg::generators