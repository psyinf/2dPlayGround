#include <fstream>
#include <pgEngine/generators/MarkovNameGen.hpp>
#include <iostream>

int main()
{
    std::ifstream fileStreamIn("../data/stars.txt", std::ios_base::binary);
    std::ifstream fileStreamIn2("../data/boys.txt", std::ios_base::binary);

    pg::generators::MarkovFrequencyMap fmg;
    while (!fileStreamIn.eof())
    {
        std::string word;
        fileStreamIn >> word;
        fmg.add(word);
    }
    while (!fileStreamIn2.eof())
    {
        std::string word;
        fileStreamIn2 >> word;
        fmg.add(word);
    }

    for (int i = 0; i < 100; i++)
    {
        std::cout << pg::generators::markov::generate(3, 10, fmg, pg::DefaultSeedGenerator) << "\n";
    }

    return 0;
}