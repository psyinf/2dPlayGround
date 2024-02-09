#pragma once
#include <pgEngine/math/Box.hpp>
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

// Define the Quadtree node class
class QuadTreeNode
{
public:
    fBox                                         bounds;
    std::vector<fBox>                            boxes;
    std::array<std::unique_ptr<QuadTreeNode>, 4> children; // Use std::unique_ptr instead of raw pointers

    QuadTreeNode(const fBox& b)
      : bounds(b)
    {
    }

    void accept(CellVisitor<QuadTreeNode>& visitor) { visitor.apply(*this); }

    // Helper function to check if the node is a leaf (has no children)
    bool isLeaf() const { return children[0] == nullptr; }
};

// Define the Quadtree class
class Quadtree
{
public:
    std::unique_ptr<QuadTreeNode> root; // Use std::unique_ptr for the root node

    Quadtree(const fBox& b) { root = std::make_unique<QuadTreeNode>(b); }

    // Function to insert a point into the quadtree
    void insert(const fBox& box, std::unique_ptr<QuadTreeNode>& node)
    {
        //  Check if the point is within the node's bounds
        if (!node->bounds.intersects(box)) { return; }

        // If the node has children, insert the point into the appropriate child
        if (!node->isLeaf())
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(box, node->children[i]);
            }
            return;
        }

        // Otherwise, insert the point into the node's points vector
        node->boxes.push_back(box);

        // Split the node if it exceeds the capacity (e.g., number of points per node)
        if (node->boxes.size() > MAX_POINTS_PER_NODE) { splitNode(node); }
    }

    // Function to split a node into four child nodes
    void splitNode(std::unique_ptr<QuadTreeNode>& node)
    {
        auto [midX, midY] = node->bounds.midpoint();
        const auto splitBoxDim = pg::fVec2{node->bounds.width() / 2, node->bounds.height() / 2};
        node->children[0] =
            std::make_unique<QuadTreeNode>(fBox({node->bounds.left(), node->bounds.top()}, splitBoxDim));
        node->children[1] = std::make_unique<QuadTreeNode>(fBox({midX, node->bounds.top()}, splitBoxDim));
        node->children[2] = std::make_unique<QuadTreeNode>(fBox({node->bounds.left(), midY}, splitBoxDim));
        node->children[3] = std::make_unique<QuadTreeNode>(fBox({midX, midY}, splitBoxDim));

        // Distribute points to children
        for (const auto& point : node->boxes)
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(point, node->children[i]);
            }
        }
        node->boxes.clear();
    }

    std::vector<fBox> rangeQuery(const fBox& queryBox, DebugIntersectionCollector& collector) const
    {
        std::vector<fBox> intersectedPoints;
        rangeQueryHelper(root, queryBox, intersectedPoints, collector);
        // sort by distance to queryBox mid
        std::ranges::sort_heap(
            intersectedPoints.begin(), intersectedPoints.end(), [&queryBox](const fBox& a, const fBox& b) {
                return lengthSquared(a.midpoint() - queryBox.midpoint()) <
                       lengthSquared(b.midpoint() - queryBox.midpoint());
            });
        return intersectedPoints;
    }

    // Function to perform a range query and return intersected points
    std::vector<fBox> rangeQuery(const fBox& queryBox) const
    {
        DebugIntersectionCollector collector;
        return rangeQuery(queryBox, collector);
    }

    // Recursive helper function for range query
    void rangeQueryHelper(const std::unique_ptr<QuadTreeNode>& node,
                          const fBox&                          queryBox,
                          std::vector<fBox>&                   intersectedPoints,
                          DebugIntersectionCollector&          collector) const
    {
        if (!node) { return; }

        if (node->bounds.intersects(queryBox))
        {
            if (!node->isLeaf())
            {
                for (int i = 0; i < 4; ++i)
                {
                    rangeQueryHelper(node->children[i], queryBox, intersectedPoints, collector);
                }
            }
            else
            {
                collector.intersectedQuadTreeNodes.push_back(node->bounds);
                for (const auto& point : node->boxes)
                {
                    if (queryBox.intersects(point)) //
                    {
                        intersectedPoints.push_back(point);
                    }
                }
            }
        }
    }

private:
    static constexpr int MAX_POINTS_PER_NODE = 2; // Adjust as needed
};

} // namespace pg