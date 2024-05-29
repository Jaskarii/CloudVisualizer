#include <map>
#include "MessageParser.h"

using namespace MessageParser;
class GridTreeDetector 
{

struct GridCell {
    std::vector<Point3D*> points;
    float groundThreshold = 0.0f; // Default value for groundThreshold
    bool isVegetation = false;
    float avgX = 0.0f;
    float avgY = 0.0f;
    float avgZ = 0.0f;
    float varianceX = 0.0f;
    float varianceY = 0.0f;
    float varianceZ = 0.0f;
    float heightToCountVar = 0.0f;
    int heightStages = 0;
    float CountToHeightAvg = 0;
    float pointDensityToNeighbours = 0.0f;
    std::map<int, int> heightToCount;

    // Default constructor
    GridCell() = default;

    // Parameterized constructor
    GridCell(float _groundThreshold) : groundThreshold(_groundThreshold) {}

    void addPoint(int index, Point3D* point) 
    {
        points.push_back(point);
    }

    void calculateAveragesAndVariances() 
    {
        avgX = sumX / points.size();
        avgY = sumY / points.size();
        avgZ = sumZ / points.size();

        for (const auto & point : points) {
            varianceX += std::pow(point->x - avgX, 2);
            varianceY += std::pow(point->y - avgY, 2);
            varianceZ += std::pow(point->z - avgZ, 2);
        }

        varianceX /= points.size();
        varianceY /= points.size();
        varianceZ /= points.size();
        // Calculate average count per height index
        CountToHeightAvg = static_cast<float>(points.size()) / heightToCount.size();

        // Calculate variance of heightToCount
        heightToCountVar = 0.0f;
        for (const auto& heightToCountPair : heightToCount) 
        {
            heightToCountVar += std::pow(heightToCountPair.second - CountToHeightAvg, 2);
        }
        heightToCountVar /= heightToCount.size();
    }

    private:
    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumZ = 0.0f;

};

const float GridSize;
const float GroundThreshold;
const int vegetrationThreshold;

public:
    GridTreeDetector(float gridSize = 0.1f, float groundThreshold = 0.20f, int vegetrationThreshold = 1);
    ~GridTreeDetector();

    void edgeDetection();

    int threshold = 10;


    // Method to split point cloud into XY grids
    void splitIntoGrids(Point3D *points, int pointCount);
    void SeparateVegetation(Point3D *points, int pointCount);

private:
    std::map<std::pair<int, int>, GridCell> gridCells;
    std::vector<int> vegetationIndices;
};