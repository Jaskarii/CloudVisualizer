#include "OctoTree.h"
#include <iostream>

OctoTreeNode::OctoTreeNode(float depth)
    : isSplit(false), depth(depth) {
    std::fill(std::begin(children), std::end(children), nullptr);
}

OctoTreeNode::~OctoTreeNode() {
    for (auto& child : children) {
        delete child;
    }
}

OctoTree::OctoTree(double minX, double minY, double minZ, double maxX, double maxY, double maxZ, int maxPointsPerNode, int maxDepth)
    : root(nullptr), minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ), maxPointsPerNode(maxPointsPerNode), maxDepth(maxDepth) {}

OctoTree::~OctoTree() {
    clear();
}

void OctoTree::insert(Point3D& point) {
    if (root == nullptr) {
        root = new OctoTreeNode(0.0f);
    }
    insertRecursive(root, point, minX, minY, minZ, maxX, maxY, maxZ);
}

void OctoTree::clear() {
    clearRecursive(root);
    root = new OctoTreeNode(0.0f); // Reset the root after clearing
}

void OctoTree::clearRecursive(OctoTreeNode* node) {
    if (node == nullptr) {
        return;
    }
    for (auto& child : node->children) {
        clearRecursive(child);
        child = nullptr; // Set pointer to null after deletion
    }
    delete node;
}

void OctoTree::insertRecursive(OctoTreeNode* node, Point3D& point, double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
    if (node->depth >= maxDepth) {
        node->points.push_back(&point);
        return;
    }

    if (node->isSplit) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;
        double midZ = (minZ + maxZ) / 2;
        int childIndex = getChildIndex(point, midX, midY, midZ);
        insertRecursive(node->children[childIndex], point,
                        childIndex & 1 ? midX : minX, childIndex & 2 ? midY : minY, childIndex & 4 ? midZ : minZ,
                        childIndex & 1 ? maxX : midX, childIndex & 2 ? maxY : midY, childIndex & 4 ? maxZ : midZ);
        return;
    }

    node->points.push_back(&point);

    if (node->points.size() >= maxPointsPerNode) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;
        double midZ = (minZ + maxZ) / 2;
        node->isSplit = true;

        for (int i = 0; i < 8; ++i) {
            node->children[i] = new OctoTreeNode(node->depth + 1.0f);
        }

        for (auto& oldPoint : node->points) {
            int childIndex = getChildIndex(*oldPoint, midX, midY, midZ);
            node->children[childIndex]->points.push_back(oldPoint);
        }

        node->points.clear();

        int childIndex = getChildIndex(point, midX, midY, midZ);
        insertRecursive(node->children[childIndex], point,
                        childIndex & 1 ? midX : minX, childIndex & 2 ? midY : minY, childIndex & 4 ? midZ : minZ,
                        childIndex & 1 ? maxX : midX, childIndex & 2 ? maxY : midY, childIndex & 4 ? maxZ : midZ);
    }
}

int OctoTree::getChildIndex(const Point3D& point, double midX, double midY, double midZ) {
    int childIndex = 0;
    if (point.x >= midX) childIndex |= 1;
    if (point.y >= midY) childIndex |= 2;
    if (point.z >= midZ) childIndex |= 4;
    return childIndex;
}

void OctoTree::calculateDensity() {
    if (root == nullptr) return;
    calculateDensityRecursive(root, minX, minY, minZ, maxX, maxY, maxZ);
}

void OctoTree::calculateDensityRecursive(OctoTreeNode* node, double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
    if (node == nullptr) return;

    double volume = (maxX - minX) * (maxY - minY) * (maxZ - minZ);
    if (volume == 0) return;

    for (auto& pointPtr : node->points) 
    {
        float dens = (node->points.size()) / static_cast<float>(volume);
        pointPtr->density = static_cast<float>(node->points.size()) / static_cast<float>(volume);
    }

    if (node->isSplit) {
        double midX = (minX + maxX) / 2;
        double midY = (minY + maxY) / 2;
        double midZ = (minZ + maxZ) / 2;

        for (int i = 0; i < 8; ++i) {
            calculateDensityRecursive(node->children[i],
                                      i & 1 ? midX : minX, i & 2 ? midY : minY, i & 4 ? midZ : minZ,
                                      i & 1 ? maxX : midX, i & 2 ? maxY : midY, i & 4 ? maxZ : midZ);
        }
    }
}
