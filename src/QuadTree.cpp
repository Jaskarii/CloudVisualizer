#include "QuadTree.h"

QuadTree::QuadTree()
{
}

QuadTree::~QuadTree()
{
}

void QuadTree::insert(const Point3D &point)
{
}

std::vector<Point3D> QuadTree::query(const Point3D &center, float radius) const
{
    std::vector<Point3D> result;
    queryRecursive(root, center, radius, result);
    return result;
}

void QuadTree::clear()
{
    clearRecursive(root);
    root = nullptr;
}

void QuadTree::insertRecursive(Node *node, const Point3D &point)
{
        // Assuming that the space is divided into 4 equal quadrants
    // We need to decide in which quadrant the point belongs
    int index;
    if (point.x < node->point.x) {
        index = point.y < node->point.y ? 0 : 1;
    } else {
        index = point.y < node->point.y ? 2 : 3;
    }

    if (node->children[index] == nullptr) {
        node->children[index] = new Node(point);
    } else {
        insertRecursive(node->children[index], point);
    }
}

void QuadTree::queryRecursive(Node *node, const Point3D &center, float radius, std::vector<Point3D> &result) const
{
    if (node == nullptr) 
    {
        return;
    }

    float distance = sqrt(pow(node->point.x - center.x, 2) + pow(node->point.y - center.y, 2) + pow(node->point.z - center.z, 2));
    if (distance <= radius) {
        result.push_back(node->point);
    }

    // Check in which quadrant the sphere of radius might fall into
    for (int i = 0; i < 4; i++) {
        if (node->children[i] != nullptr) {
            float distanceToChild = sqrt(pow(node->children[i]->point.x - center.x, 2) + pow(node->children[i]->point.y - center.y, 2) + pow(node->children[i]->point.z - center.z, 2));
            if (distanceToChild - radius <= 0) { // If the sphere intersects the quadrant
                queryRecursive(node->children[i], center, radius, result);
            }
        }
    }
}

void QuadTree::clearRecursive(Node *node)
{
        if (node == nullptr) {
        return;
    }

    for (int i = 0; i < 4; i++) {
        clearRecursive(node->children[i]);
    }

    delete node;
}
