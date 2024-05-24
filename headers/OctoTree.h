#pragma once
#include <vector>
#include "MessageParser.h"

using namespace MessageParser;

struct OctoTreeNode {
    std::vector<Point3D*> points;
    OctoTreeNode* children[8];
    bool isSplit;
    float depth;
    OctoTreeNode(float depth);
    ~OctoTreeNode();
};

class OctoTree {
public:
    OctoTree(double minX, double minY, double minZ, double maxX, double maxY, double maxZ, int maxPointsPerNode, int maxDepth);
    ~OctoTree();
    void insert(Point3D& point);
    void clear();
    void calculateDensity();

private:
    OctoTreeNode* root;
    double minX, minY, minZ, maxX, maxY, maxZ;
    int maxPointsPerNode;
    int maxDepth;

    void insertRecursive(OctoTreeNode* node, Point3D& point, double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
    int getChildIndex(const Point3D& point, double midX, double midY, double midZ);
    void clearRecursive(OctoTreeNode* node);
    void calculateDensityRecursive(OctoTreeNode* node, double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
};