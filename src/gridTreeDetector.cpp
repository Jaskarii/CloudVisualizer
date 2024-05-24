#include "gridTreeDetector.h"
#include <algorithm>

GridTreeDetector::GridTreeDetector()
{
}

GridTreeDetector::~GridTreeDetector()
{
}

std::vector<Point3D> GridTreeDetector::splitIntoGrids(Point3D *points, int pointCount)
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

  std::vector<Point3D> treePositions;
  treePositions.clear();
  std::vector<float> heightToCoutAvgs;
  heightToCoutAvgs.clear();

  return treePositions;
}