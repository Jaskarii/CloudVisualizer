#include "gridTreeDetector.h"
#include <algorithm>

GridTreeDetector::GridTreeDetector(float gridSize, float groundThreshold, int vegetrationThreshold):
GridSize(gridSize), GroundThreshold(groundThreshold), vegetrationThreshold(vegetrationThreshold)
{
}

GridTreeDetector::~GridTreeDetector()
{
}

void GridTreeDetector::splitIntoGrids(Point3D *points, int pointCount)
{
  gridCells.clear();
  vegetationIndices.clear();
  // Split the point cloud into XY grids based on GridSize
  for (size_t i = 0; i < pointCount; i++) 
  {
    if (!points[i].isVegetation)
    {
      continue;
    }
    
    auto & point = points[i];
    int gridX = static_cast<int>(point.x / GridSize);
    int gridY = static_cast<int>(point.y / GridSize);
    // Check if grid cell exists, if not create one
    auto key = std::make_pair(gridX, gridY);
    if (gridCells.find(key) == gridCells.end()) 
    {
      gridCells[key] = GridCell(point.z + GroundThreshold); // Create a new grid cell
    }

    GridCell & gridCell = gridCells[key];
    gridCell.addPoint(i, &point);    
  }
  edgeDetection();
}

void GridTreeDetector::SeparateVegetation(Point3D *points, int pointCount)
{
    // Sort the entire point cloud based on z coordinate
  std::sort(points, points + pointCount, [](const Point3D & a, const Point3D & b) {
      return a.z < b.z;
  });

  //octTree->clear();
  gridCells.clear();
  vegetationIndices.clear();
  // Split the point cloud into XY grids based on GridSize
  for (size_t i = 0; i < pointCount; i++) 
  {
    auto & point = points[i];
    int gridX = static_cast<int>(point.x / GridSize);
    int gridY = static_cast<int>(point.y / GridSize);
    // Check if grid cell exists, if not create one
    auto key = std::make_pair(gridX, gridY);
    if (gridCells.find(key) == gridCells.end()) 
    {
      gridCells[key] = GridCell(point.z + GroundThreshold); // Create a new grid cell
    }

    GridCell & gridCell = gridCells[key];

    if (gridCell.isVegetation) 
    {
      point.isVegetation = 1.0f;
      gridCell.addPoint(i, &point);
      continue;
    }

    gridCell.isVegetation = point.z > gridCell.groundThreshold;
  }
}

void GridTreeDetector::edgeDetection() 
{
    // Sobel kernels
    int sobelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int sobelY[3][3] = {
        {-1, -2, -1},
        {0,  0,  0},
        {1,  2,  1}
    };

    std::map<std::pair<int, int>, float> densityMap;
    for (const auto& gridCellPair : gridCells) {
        const auto& key = gridCellPair.first;
        const auto& gridCell = gridCellPair.second;
        densityMap[key] = gridCell.points.size();
    }

    for (auto& gridCellPair : gridCells) {
        const auto& key = gridCellPair.first;
        float gradX = 0.0f;
        float gradY = 0.0f;

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                auto neighborKey = std::make_pair(key.first + dx, key.second + dy);
                float density = 0.0f;
                if (densityMap.find(neighborKey) != densityMap.end()) {
                    density = densityMap[neighborKey];
                }
                gradX += density * sobelX[dy + 1][dx + 1];
                gradY += density * sobelY[dy + 1][dx + 1];
            }
        }

        float magnitude = std::sqrt(gradX * gradX + gradY * gradY);
        bool isEdgeCell = magnitude > 3.5f;

        for (auto& point : gridCellPair.second.points) {
            point->density = isEdgeCell ? 1.0f : 0.0f;
        }
    }
}