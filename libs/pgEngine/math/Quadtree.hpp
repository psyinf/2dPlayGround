#pragma once
#include <pgEngine/math/Box.hpp>
#include <pgEngine/math/QuadtreeVisitor.hpp>
#include <iostream>
#include <vector>
#include <memory>    // Include for smart pointers
#include <algorithm> // Include for std::min and std::max

namespace pg {

// Define the Quadtree node class
class QuadTreeNode
{
public:
    fBox                                         bounds;
    std::vector<fBox>                            points;
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

    Quadtree(const fBox& b)
    {
        root = std::make_unique<QuadTreeNode>(b); // Use std::make_unique to create the root node
    }

    // Function to insert a point into the quadtree
    void insert(const fBox& point, std::unique_ptr<QuadTreeNode>& node)
    {
        // TODO: this seems to test in the wrong ways
        //  Check if the point is within the node's bounds
        if (!point.contains(node->bounds)) { return; }

        // If the node has children, insert the point into the appropriate child
        if (!node->isLeaf())
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(point, node->children[i]);
            }
            return;
        }

        // Otherwise, insert the point into the node's points vector
        node->points.push_back(point);

        // Split the node if it exceeds the capacity (e.g., number of points per node)
        if (node->points.size() > MAX_POINTS_PER_NODE) { splitNode(node); }
    }

    // Function to split a node into four child nodes
    void splitNode(std::unique_ptr<QuadTreeNode>& node)
    {
        auto [midX, midY] = node->bounds.midpoint();

        node->children[0] = std::make_unique<QuadTreeNode>(
            fBox({node->bounds.left(), node->bounds.top()}, {node->bounds.width() / 2, node->bounds.height() / 2}));
        node->children[1] = std::make_unique<QuadTreeNode>(
            fBox({midX, node->bounds.top()}, {node->bounds.width() / 2, node->bounds.height() / 2}));
        node->children[2] = std::make_unique<QuadTreeNode>(
            fBox({node->bounds.left(), midY}, {node->bounds.width() / 2, node->bounds.height() / 2}));
        node->children[3] =
            std::make_unique<QuadTreeNode>(fBox({midX, midY}, {node->bounds.width() / 2, node->bounds.height() / 2}));

        // Distribute points to children
        for (const auto& point : node->points)
        {
            for (int i = 0; i < 4; ++i)
            {
                insert(point, node->children[i]);
            }
        }
        node->points.clear();
    }

    // Function to perform a range query and return intersected points
    std::vector<fBox> rangeQuery(const fBox& queryBox) const
    {
        std::vector<fBox> intersectedPoints;
        rangeQueryHelper(root, queryBox, intersectedPoints);
        return intersectedPoints;
    }

    // Recursive helper function for range query
    void rangeQueryHelper(const std::unique_ptr<QuadTreeNode>& node,
                          const fBox&                          queryBox,
                          std::vector<fBox>&                   intersectedPoints) const
    {
        if (!node) { return; }

        if (node->bounds.intersects(queryBox))
        {
            for (const auto& point : node->points)
            {
                if (queryBox.contains(point.midpoint())) { intersectedPoints.push_back(point); }
            }

            if (!node->isLeaf())
            {
                for (int i = 0; i < 4; ++i)
                {
                    rangeQueryHelper(node->children[i], queryBox, intersectedPoints);
                }
            }
        }
    }

private:
    static constexpr int MAX_POINTS_PER_NODE = 4; // Adjust as needed
};

} // namespace pg