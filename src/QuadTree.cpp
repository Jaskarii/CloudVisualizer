#include "QuadTree.h"
#include <iostream>

QuadTreeNode::QuadTreeNode(float depth)
    : isSplit(false), depth(depth) {
    std::fill(std::begin(children), std::end(children), nullptr);
}

QuadTreeNode::~QuadTreeNode() {
    for (auto& child : children) {
        delete child;
    }
}

QuadTree::QuadTree(double minX, double minY, double maxX, double maxY, int maxPointsPerNode, int maxDepth)
    : root(nullptr), minX(minX), minY(minY), maxX(maxX), maxY(maxY), maxPointsPerNode(maxPointsPerNode), maxDepth(maxDepth) {}

QuadTree::~QuadTree() {
    clear();
}

void QuadTree::insert(Point3D& point) {
    if (root == nullptr) {
        root = new QuadTreeNode(0.0f);
    }
    insertRecursive(root, point, minX, minY, maxX, maxY);
}

void QuadTree::clear() {
    clearRecursive(root);
    root = new QuadTreeNode(0.0f); // Reset the root after clearing
}

void QuadTree::clearRecursive(QuadTreeNode* node) {
    if (node == nullptr) {
        return;
    }
    for (auto& child : node->children) {
        clearRecursive(child);
        child = nullptr; // Set pointer to null after deletion
    }
    delete node;
}

void QuadTree::insertRecursive(QuadTreeNode* node, Point3D& point, double minX, double minY, double maxX, double maxY) {
    if (node->depth >= maxDepth) {
        node->points.push_back(&point);
        return;
    }

    if (node->isSplit) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;
        int childIndex = getChildIndex(point, midX, midY);
        insertRecursive(node->children[childIndex], point,
                        childIndex & 1 ? midX : minX, childIndex & 2 ? midY : minY,
                        childIndex & 1 ? maxX : midX, childIndex & 2 ? maxY : midY);
        return;
    }

    node->points.push_back(&point);

    if (node->points.size() >= maxPointsPerNode) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;
        node->isSplit = true;

        for (int i = 0; i < 4; ++i) {
            node->children[i] = new QuadTreeNode(node->depth + 1.0f);
        }

        for (auto& oldPoint : node->points) {
            int childIndex = getChildIndex(*oldPoint, midX, midY);
            node->children[childIndex]->points.push_back(oldPoint);
        }

        node->points.clear();

        int childIndex = getChildIndex(point, midX, midY);
        insertRecursive(node->children[childIndex], point,
                        childIndex & 1 ? midX : minX, childIndex & 2 ? midY : minY,
                        childIndex & 1 ? maxX : midX, childIndex & 2 ? maxY : midY);
    }
}

int QuadTree::getChildIndex(const Point3D& point, double midX, double midY) {
    int childIndex = 0;
    if (point.x >= midX) childIndex |= 1;
    if (point.y >= midY) childIndex |= 2;
    return childIndex;
}

void QuadTree::calculateDensity() {
    if (root == nullptr) return;
    calculateDensityRecursive(root, minX, minY, maxX, maxY);
}

void QuadTree::calculateDensityRecursive(QuadTreeNode* node, double minX, double minY, double maxX, double maxY) {
    if (node == nullptr) return;

    double area = (maxX - minX) * (maxY - minY);
    if (area == 0) return;

    for (auto& pointPtr : node->points) {
        pointPtr->density = static_cast<float>(node->points.size()) / static_cast<float>(area);
    }

    if (node->isSplit) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;

        for (int i = 0; i < 4; ++i) {
            calculateDensityRecursive(node->children[i],
                                      i & 1 ? midX : minX, i & 2 ? midY : minY,
                                      i & 1 ? maxX : midX, i & 2 ? maxY : midY);
        }
    }
}