#pragma once
#include <pgEngine/math/Box.hpp>
#include <vector>

template <class Cell>
class CellVisitor
{
public:
    virtual void apply(Cell& c) { traverse(c); }

    virtual void done(const Cell&) {}

    void traverse(const Cell& c)
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

    virtual void apply(T& c)
    {
        if (0 != c.points.size()) { results.emplace_back(c.bounds); }
        else {}

        CellVisitor<T>::traverse(c);
    }

    virtual void done(T& c) {}

    ResultVector results;
};

template <class T>
class CollectBoundsVisitor : public CellVisitor<T>
{
public:
    using ResultVector = std::vector<pg::fBox>;

    virtual void apply(T& c)
    {
        results.emplace_back(c.bounds);

        CellVisitor<T>::traverse(c);
    }

    virtual void done(T&) {}

    ResultVector results;
};