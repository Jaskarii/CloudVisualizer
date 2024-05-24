#pragma once
#include <vector>
#include "MessageParser.h"

using namespace MessageParser;

struct QuadTreeNode {
    std::vector<Point3D*> points;
    QuadTreeNode* children[4];
    bool isSplit;
    float depth;
    QuadTreeNode(float depth);
    ~QuadTreeNode();
};

class QuadTree {
public:
    QuadTree(double minX, double minY, double maxX, double maxY, int maxPointsPerNode, int maxDepth);
    ~QuadTree();
    void insert(Point3D& point);
    void clear();
    void calculateDensity();

private:
    QuadTreeNode* root;
    double minX, minY, maxX, maxY;
    int maxPointsPerNode;
    int maxDepth;

    void insertRecursive(QuadTreeNode* node, Point3D& point, double minX, double minY, double maxX, double maxY);
    int getChildIndex(const Point3D& point, double midX, double midY);
    void clearRecursive(QuadTreeNode* node);
    void calculateDensityRecursive(QuadTreeNode* node, double minX, double minY, double maxX, double maxY);
};