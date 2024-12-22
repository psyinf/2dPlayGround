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

template <size_t Depth>
class MarkovFrequencyMap
{
    static constexpr size_t depth = Depth;
    // ensure depth > 0
    static_assert(depth > 0, "Depth must be greater than 0");

    using LetterFrequency = std::unordered_map<char, Frequency>;
    using FrequencyMap = std::unordered_map<std::string, LetterFrequency>;

public:
    void add(std::string word)
    {
        // split if word contains multiple words
        auto words = pg::foundation::strings::tokenize(word, " ,-:") | std::ranges::to<std::vector<std::string>>();
        for (auto& w : words)
        {
            if (filterWord(w)) { addWord(w); }
        }
    }

    std::string key_at(size_t index) const
    {
        // TODO: caching
        auto states = getStartStates();
        return states[index];
    }

    size_t size() const { return frequencyMap.size(); }

    std::vector<std::string> getStartStates() const
    {
        return std::views::keys(frequencyMap) | std::ranges::to<std::vector<std::string>>();
    }

    std::vector<char> getNextStates(const std::string& key, Position pos) const
    {
        if (!frequencyMap.contains(key)) { return {}; }
        std::vector<char> result;
        auto              f = frequencyMap.at(key);
        for (auto& [letter, frequency] : f)
        {
            size_t count = 0;
            switch (pos)
            {
            case Position::Start:
                count = frequency.start;
                break;
            case Position::Within:
                count = frequency.within;

                break;
            case Position::End:
                count = frequency.end;
                break;
            }

            if (count == 0) { count = 1; }

            auto repeat_pattern = std::views::repeat(letter, count);
            // result.append_range(repeat_pattern); not working for gcc
            result.insert(result.end(), repeat_pattern.begin(), repeat_pattern.end());
        }
        return result;
    }

private:
    bool filterWord(std::string& word)
    {
        // pre-process
        //  remove non-alphabetic characters
        word.erase(std::remove_if(word.begin(),
                                  word.end(),
                                  [](char c) {
                                      bool al = c > 0 && std::isalpha(c);
                                      return !al;
                                  }),
                   word.end());

        if (word.length() < depth) { return false; }
        // all to lower case
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        return true;
    }

    void addWord(const std::string& word)
    {
        for (auto wordLetterIndex = 0u; wordLetterIndex < word.size() - depth; wordLetterIndex++)
        {
            // take the substring of the word
            const auto substring = word.substr(wordLetterIndex, depth);
            if (wordLetterIndex == 0)
            { // at the beginning
                addFrequency(substring, word[wordLetterIndex + depth], {1, 0, 0});
            }
            if (wordLetterIndex == word.size() - depth - 1)
            { // at the end
                addFrequency(substring, word[wordLetterIndex + depth], {0, 0, 1});
            }
            else
            { // in the middle
                addFrequency(substring, word[wordLetterIndex + depth], {0, 1, 0});
            }
        }
    }

    void addFrequency(const std::string& key, const char& letter, const Frequency& frequency)
    {
        auto& current_frequency = frequencyMap[key][letter];
        current_frequency.start += frequency.start;
        current_frequency.within += frequency.within;
        current_frequency.end += frequency.end;
    }

    FrequencyMap frequencyMap;
};

} // namespace pg::generators