#pragma once
#include <pgEngine/math/Box.hpp>
#include <pgEngine/math/VecOps.hpp>
#include <pgEngine/math/QuadtreeVisitor.hpp>
#include <iostream>
#include <vector>
#include <memory>    // Include for smart pointers
#include <algorithm> // Include for std::min and std::max

namespace pg {

struct DebugIntersectionCollector
{
    std::vector<pg::fBox> intersectedQuadTreeNodes;
};

template <typename T>
class QuadTreeNode
{
public:
    struct Box
    {
        fBox           box;
        std::vector<T> data;
    };

    fBox                                         bounds;
    std::vector<Box>                             boxes;
    std::array<std::unique_ptr<QuadTreeNode>, 4> children;

    QuadTreeNode(const fBox& b)
      : bounds(b)
    {
    }

    void accept(CellVisitor<QuadTreeNode>& visitor) { visitor.apply(*this); }

    // Helper function to check if the node is a leaf (has no children)
    bool isLeaf() const { return children[0] == nullptr; }
};

template <typename T = int>
class Quadtree
{
public:
    using Node = QuadTreeNode<T>;
    using Data = std::vector<T>;
    using Result = Node::Box;
    std::unique_ptr<Node> root; // Use std::unique_ptr for the root node

    Quadtree(const fBox& b) { root = std::make_unique<Node>(b); }

    void insert(const fBox& box, T data, std::unique_ptr<Node>& node) { insert(box, Data{{data}}, node); }

    // Function to insert a point into the quadtree
    void insert(const fBox& box, Data data, std::unique_ptr<Node>& node)
    {
        //  Check if the point is within the node's bounds
        if (!node->bounds.intersects(box)) { return; }

        // If the node has children, insert the point into the appropriate child
        if (!node->isLeaf())
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(box, data, node->children[i]);
            }
            return;
        }

        // Otherwise, insert the point into the node's points vector

        node->boxes.push_back({.box{box}, .data{data}});

        // Split the node if it exceeds the capacity (e.g., number of points per node)
        if (node->boxes.size() > MAX_POINTS_PER_NODE) { splitNode(node); }
    }

    // Function to split a node into four child nodes
    void splitNode(std::unique_ptr<Node>& node)
    {
        auto [midX, midY] = node->bounds.midpoint();
        const auto splitBoxDim = pg::fVec2{node->bounds.width() / 2, node->bounds.height() / 2};
        node->children[0] = std::make_unique<Node>(fBox({node->bounds.left(), node->bounds.top()}, splitBoxDim));
        node->children[1] = std::make_unique<Node>(fBox({midX, node->bounds.top()}, splitBoxDim));
        node->children[2] = std::make_unique<Node>(fBox({node->bounds.left(), midY}, splitBoxDim));
        node->children[3] = std::make_unique<Node>(fBox({midX, midY}, splitBoxDim));

        // Distribute points to children
        for (const auto& [point, data] : node->boxes)
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(point, data, node->children[i]);
            }
        }
        node->boxes.clear();
    }

    std::vector<Result> debugRangeQuery(const fBox& queryBox, DebugIntersectionCollector& collector) const
    {
        std::vector<Result> intersections;
        rangeQueryHelper(root, queryBox, intersections, collector);
        // sort by distance to queryBox mid
        std::ranges::sort(intersections, [&queryBox](const Result& a, const Result& b) {
            return pg::lengthSquared(a.box.midpoint() - queryBox.midpoint()) <
                   pg::lengthSquared(b.box.midpoint() - queryBox.midpoint());
        });
        return intersections;
    }

    template <typename QueryPrimitive>
    // Function to perform a range query and return intersected points
    std::vector<Result> rangeQuery(const QueryPrimitive& prim) const
    {
        std::vector<Result> intersections;
        internalRangeQuery(root, prim, intersections);
        std::ranges::sort(intersections, [&prim](const Result& a, const Result& b) {
            return pg::lengthSquared(a.box.midpoint() - prim.midpoint()) <
                   pg::lengthSquared(b.box.midpoint() - prim.midpoint());
        });
        return intersections;
    }

protected:
    template <typename QueryPrimitive>
    void internalRangeQuery(const std::unique_ptr<Node>& node,
                            const QueryPrimitive&        queryPrim,
                            std::vector<Result>&         intersections) const
    {
        if (!node) { return; }

        if (node->bounds.intersects(queryPrim))
        {
            if (!node->isLeaf())
            {
                for (int i = 0; i < 4; ++i)
                {
                    internalRangeQuery(node->children[i], queryPrim, intersections);
                }
            }
            else
            {
                for (const auto& [point, data] : node->boxes)
                {
                    if (queryPrim.intersects(point)) //
                    {
                        intersections.push_back({point, data});
                    }
                }
            }
        }
    }

    // Recursive helper function for range query
    void rangeQueryHelper(const std::unique_ptr<Node>& node,
                          const fBox&                  queryBox,
                          std::vector<Result>&         intersections,
                          DebugIntersectionCollector&  collector) const
    {
        if (!node) { return; }

        if (node->bounds.intersects(queryBox))
        {
            if (!node->isLeaf())
            {
                for (int i = 0; i < 4; ++i)
                {
                    rangeQueryHelper(node->children[i], queryBox, intersections, collector);
                }
            }
            else
            {
                collector.intersectedQuadTreeNodes.push_back(node->bounds);
                for (const auto& [point, data] : node->boxes)
                {
                    if (queryBox.intersects(point)) //
                    {
                        intersections.push_back({point, data});
                    }
                }
            }
        }
    }

private:
    static constexpr int MAX_POINTS_PER_NODE = 2; // Adjust as needed
};

} // namespace pg