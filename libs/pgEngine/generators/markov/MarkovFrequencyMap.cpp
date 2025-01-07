#include "MarkovFrequencyMap.hpp"

void pg::generators::MarkovFrequencyMap::addFrequency(const std::string& key,
                                                      const char&        letter,
                                                      const Frequency&   frequency)
{
    auto& current_frequency = frequencyMap[key][letter];
    current_frequency.start += frequency.start;
    current_frequency.within += frequency.within;
    current_frequency.end += frequency.end;
}

void pg::generators::MarkovFrequencyMap::addWord(const std::string& word)
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

bool pg::generators::MarkovFrequencyMap::filterWord(std::string& word)
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

std::vector<char> pg::generators::MarkovFrequencyMap::getNextStates(const std::string& key, Position pos) const
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

std::vector<std::string> pg::generators::MarkovFrequencyMap::getStartStates() const
{
    return std::views::keys(frequencyMap) | std::ranges::to<std::vector<std::string>>();
}

size_t pg::generators::MarkovFrequencyMap::size() const
{
    return frequencyMap.size();
}

std::string pg::generators::MarkovFrequencyMap::key_at(size_t index) const
{
    // TODO: caching
    auto states = getStartStates();
    return states[index];
}

void pg::generators::MarkovFrequencyMap::add(std::string word)
{
    // split if word contains multiple words
    auto words = pg::foundation::strings::tokenize(word, " ,-:") | std::ranges::to<std::vector<std::string>>();
    for (auto& w : words)
    {
        if (filterWord(w)) { addWord(w); }
    }
}

const size_t pg::generators::MarkovFrequencyMap::getDepth() const
{
    return depth;
}
