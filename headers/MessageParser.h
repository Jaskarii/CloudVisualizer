#pragma once
#include <iostream>
#include <vector>
#include <sstream>  // For std::istringstream
#include <string>   // For std::string
#include "UDPSocket.h"


namespace MessageParser 
{
    struct MinMaxValues 
    {
        int pointCount = 0;
        float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
        void Reset()
        {
            pointCount = 0;
            minX = 0;
            maxX = 0;
            minY = 0;
            maxY = 0;
            minZ = 0;
            maxZ = 0;
        }
    };

    struct ColoredPoint3D
    {
        float x, y, z, r;

        ColoredPoint3D() : x(0), y(0), z(0), r(0) {}
        ColoredPoint3D(float x_, float y_, float z_, float r_) : x(x_), y(y_), z(z_), r(r_) {}
    };

    struct Point3D {
        float x, y, z;
        float density = 0.0f;  // Renamed from depth to density
        float isVegetation = 1.0f;
        Point3D() : x(0), y(0), z(0) {}
        Point3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    };

    struct Point2D
    {
        float x, y;
    };

    void AddCloudToArray(const char *message, size_t size, std::vector<float> &buffer);
    MinMaxValues ReadCloudBufferToPoints(std::vector<float> &buffer, Point3D *points);
    int AddTreesToBuffer(const char *message, int Fromindex, size_t size, Point2D *tree_points);
}