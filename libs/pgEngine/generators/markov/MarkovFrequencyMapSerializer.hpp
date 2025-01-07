#pragma once
// #include <cereal/types/unordered_map.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <fstream>

#include <pgEngine/generators/markov/MarkovFrequencyMap.hpp>

namespace pg::generators {

template <class Archive>
void serialize(Archive& archive, Frequency& frequency)
{
    archive(frequency.start, frequency.within, frequency.end);
}

void saveFrequencyMap(const std::string& fileName, const pg::generators::FrequencyMap& frequencyMap)
{
    std::ofstream ofs(fileName, std::ios::binary);
    if (!ofs.is_open()) { throw std::runtime_error("Could not open file"); }
    cereal::BinaryOutputArchive archive(ofs);
    archive(frequencyMap);
}

pg::generators::FrequencyMap loadFrequencyMap(const std::string& fileName)
{
    pg::generators::FrequencyMap frequencyMap;
    std::ifstream                ifs(fileName, std::ios::binary);
    if (!ifs.is_open()) { throw std::runtime_error("Could not open file"); }
    cereal::BinaryInputArchive archive(ifs);
    archive(frequencyMap);
    return frequencyMap;
}
} // namespace pg::generators