#pragma once

#include "MessageParser.h"

using namespace MessageParser;
class QuadTree {
public:
    QuadTree();
    ~QuadTree();

    void insert(const Point3D& point);
    std::vector<Point3D> query(const Point3D& center, float radius) const;
    void clear();

private:
    struct Node 
    {
        Point3D point;
        Node* children[4];

        Node(const Point3D& p) : point(p) 
        {
            for (int i = 0; i < 4; i++) {
                children[i] = nullptr;
            }
        }
    };

    Node* root;

    void insertRecursive(Node* node, const Point3D& point);
    void queryRecursive(Node* node, const Point3D& center, float radius, std::vector<Point3D>& result) const;
    void clearRecursive(Node* node);

};
