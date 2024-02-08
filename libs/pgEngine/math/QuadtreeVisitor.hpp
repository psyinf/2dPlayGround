#pragma once
#include <pgEngine/math/Box.hpp>
#include <vector>

template <class Cell>
class CellVisitor
{
public:
    virtual void apply(Cell& c) { traverse(c); }

    virtual void done(Cell& c) {}

    void traverse(Cell& c)
    {
        for (auto& child : c.children)
        {
            if (child)
            {
                currentDepth++;
                apply(*child);
                currentDepth--;
            }
        }
        done(c);
    }

protected:
    size_t currentDepth{};
};

template <class T>
class CollectChildrenVisitor : public CellVisitor<T>
{
public:
    using ResultVector = std::vector<pg::fBox>;

    CollectChildrenVisitor(ResultVector& collection)
      : mResults(collection)
    {
    }

    virtual void apply(T& c)
    {
        if (0 != c.size()) { mResults.emplace_back(c.bounds); }
        else {}

        traverse(c);
    }

    virtual void done(T& c) {}

protected:
    ResultVector& mResults;
};