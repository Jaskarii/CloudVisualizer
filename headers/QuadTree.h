#pragma once
#include <vector>
#include "MessageParser.h"

using namespace MessageParser;

struct QuadTreeNode {
    std::vector<Point3D*> points;
    Point2D centrePoint;
    QuadTreeNode* children[4];
    int pointCount;
    bool isSplit;
    float growthRate;
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
    std::vector<Point2D> GetTreePositions(float densityLimit);


private:
    QuadTreeNode* root;
    double minX, minY, maxX, maxY;
    int maxPointsPerNode;
    int maxDepth;
    int fromLevel;

    void insertRecursive(QuadTreeNode* node, Point3D& point, double minX, double minY, double maxX, double maxY);
    int getChildIndex(const Point3D& point, double midX, double midY);
    void clearRecursive(QuadTreeNode* node);
    void calculateDensityRecursive(QuadTreeNode* node, double minX, double minY, double maxX, double maxY, int ParentPointCount);
    Point2D calculateCenter(double minX, double minY, double maxX, double maxY); // Method to calculate the center point of a node
    void getTreePositionsRecursive(QuadTreeNode* node, double minX, double minY, double maxX, double maxY, float densityLimit, std::vector<Point2D>& result);
};