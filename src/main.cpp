#include <OpenGLWindow.h>
#include <UDPSocket.h>
#include <VertexBuffer.h>
#include <Shader.h>
#include <cmath>
#include <cstring>
#include <random>
#include <VertexBufferLayout.h>
#include <Vertexarray.h>
#include "coordFrame.h"
#include <ImGuiManager.h>
#include "gridTreeDetector.h"
#include <OctoTree.h>
#include <QuadTree.h>

char ipBuffer[1024] = ""; // Buffer to hold IP
char portBuffer[256] = ""; // Buffer to hold port
char cloudFileBuffer[1024] = "";
char treeFileBuffer[1024] = "";

const char *cloudPath = "/home/anssi/tda/feature/ThinningDensityAssistant/build/src/samples/whole_cloud.pcd";
const char *treeXmlPath = "/mnt/drive/TDA kirjasto/Vierema.xml";

void InitializeDefaultFilePaths()
{
    std::strncpy(cloudFileBuffer, cloudPath, sizeof(cloudFileBuffer) - 1);
    cloudFileBuffer[sizeof(cloudFileBuffer) - 1] = '\0';

    std::strncpy(treeFileBuffer, treeXmlPath, sizeof(treeFileBuffer) - 1);
    treeFileBuffer[sizeof(treeFileBuffer) - 1] = '\0';
}

void checkGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << std::hex << err << std::endl;
    }
}

GridTreeDetector *gridTreeDetector = new GridTreeDetector(0.5f,0.2f,10);
GridTreeDetector *gridTreeDetector2 = new GridTreeDetector(0.02f,0.0f,1);
OctoTree *octTree = new OctoTree(-100, -100, -100, 100, 100, 100, 10, 10);
QuadTree *quadTree = new QuadTree(-100, -100, 100, 100, 10, 10);
Point3D *points = new Point3D[5000000];
Point2D *tree_points = new Point2D[50000];
Point2D *tree_points_raw = new Point2D[50000];
CoordFrame *frame;

VertexBuffer *vb;
VertexBuffer *treeVB;
std::vector<float> pointBuffer;
int _pointCount = 0;
int _treeCountTemp = 0;
int _treeCount = 0;

bool updatePoints = false;
bool updateTrees = false;
bool startReceived = false;
bool showTrees = false;
bool updatingPoints = false;
bool requestCloudFileLoad = false;
bool requestTreeFileLoad = false;
bool requestTreeTransformUpdate = false;
int densityScale = 5;

float treeOffsetX = -17.029f;
float treeOffsetY = -12.617f;
float treeRotationDeg = 150.968f;

void ApplyTreeTransform(int treeCount)
{
    const float angleRad = treeRotationDeg * 3.14159265358979323846f / 180.0f;
    const float cosAngle = std::cos(angleRad);
    const float sinAngle = std::sin(angleRad);

    for (int i = 0; i < treeCount; i++)
    {
        const float srcX = tree_points_raw[i].x;
        const float srcY = tree_points_raw[i].y;

        tree_points[i].x = srcX * cosAngle - srcY * sinAngle + treeOffsetX;
        tree_points[i].y = srcX * sinAngle + srcY * cosAngle + treeOffsetY;
    }
}


// Callback function
void ProcessCloudBuffer(std::vector<float> &cloudBuffer)
{
    MinMaxValues values = ReadCloudBufferToPoints(cloudBuffer, points);
    gridTreeDetector->SeparateVegetation(points, values.pointCount);

    quadTree->clear();
    quadTree = new QuadTree(values.minX, values.minY, values.maxX, values.maxY, 20, 9);

    for (size_t i = 0; i < values.pointCount; i++)
    {
        if (points[i].isVegetation)
        {
            quadTree->insert(points[i]);
        }
    }

    quadTree->calculateDensity();
    _pointCount = values.pointCount;
    updatePoints = true;
}

void onMessageReceived(const char *message, size_t size)
{
    if (size <= 0)
    {
        return;
    }

    if (message[0] == '!')
    {
        if (message[1] == 'S')
        {
            pointBuffer.clear();
            startReceived = true;
        }
        else if (message[1] == 'E')
        {
            ProcessCloudBuffer(pointBuffer);
            startReceived = false;
        }
        else if (message[1] == 'T')
        {
            _treeCountTemp = AddTreesToBuffer(message, _treeCountTemp, size, tree_points);
        }
        else if (message[1] == 'D')
        {
            _treeCount = _treeCountTemp;
            _treeCountTemp = 0;

            for (int i = 0; i < _treeCount; i++)
            {
                tree_points_raw[i] = tree_points[i];
            }
            ApplyTreeTransform(_treeCount);

            updateTrees = true;
        }
        return;
    }

    if (startReceived)
    {
        AddCloudToArray(message, size, pointBuffer);
    }
}

int main()
{
    InitializeDefaultFilePaths();

    OpenGLWindow window(800, 600, "OpenGL Window");

    vb = new VertexBuffer(points, 5000000 * sizeof(Point3D));
    VertexBufferLayout layout;
    layout.PushFloat(3);
    layout.PushFloat(1);
    layout.PushFloat(1);
    Vertexarray array = Vertexarray();
    array.AddBuffer(*vb, layout);

    treeVB = new VertexBuffer(tree_points, 50000 * sizeof(Point2D));
    Vertexarray tree_array = Vertexarray();
    VertexBufferLayout tree_layout;
    tree_layout.PushFloat(2u);
    tree_array.AddBuffer(*treeVB, tree_layout);

    Shader shader("../shaders/point.glsl");
    Shader treeShader("../shaders/tree.glsl");
    frame = new CoordFrame();
    checkGLError();

    UDPSocket socket(onMessageReceived);

    ImGuiManager imguiManager(window.GetWindow(), &socket, _pointCount, _treeCount, ipBuffer, portBuffer, cloudFileBuffer, treeFileBuffer, requestCloudFileLoad, requestTreeFileLoad, requestTreeTransformUpdate, treeOffsetX, treeOffsetY, treeRotationDeg, showTrees, densityScale);

    while (true)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        checkGLError();

        if (requestCloudFileLoad)
        {
            requestCloudFileLoad = false;

            std::string errorMessage;
            std::vector<float> fileCloudBuffer;
            if (ReadCloudFileToBuffer(cloudFileBuffer, fileCloudBuffer, errorMessage))
            {
                ProcessCloudBuffer(fileCloudBuffer);
                _treeCount = 0;
                _treeCountTemp = 0;
                std::cout << "Loaded " << _pointCount << " points from file: " << cloudFileBuffer << std::endl;
            }
            else
            {
                std::cerr << "Cloud file load failed: " << errorMessage << std::endl;
            }
        }

        if (requestTreeFileLoad)
        {
            requestTreeFileLoad = false;

            std::string errorMessage;
            int loadedTreeCount = 0;
            if (ReadTreeXmlFileToBuffer(treeFileBuffer, tree_points_raw, 50000, loadedTreeCount, errorMessage))
            {
                _treeCount = loadedTreeCount;
                _treeCountTemp = 0;
                ApplyTreeTransform(_treeCount);
                updateTrees = true;
                std::cout << "Loaded " << _treeCount << " trees from XML file: " << treeFileBuffer << std::endl;
            }
            else
            {
                std::cerr << "Tree XML load failed: " << errorMessage << std::endl;
            }
        }

        if (requestTreeTransformUpdate)
        {
            requestTreeTransformUpdate = false;
            if (_treeCount > 0)
            {
                ApplyTreeTransform(_treeCount);
                updateTrees = true;
            }
        }

        if (updatePoints)
        {
            vb->update(points, _pointCount * sizeof(Point3D));
            updatePoints = false;
        }

        if (_pointCount > 0)
        {
            array.Bind();
            shader.Bind();
            shader.SetUniformMat4f("MVP", window.getMVP());
            shader.SetUniform1i("densityScale", densityScale);
            glDrawArrays(GL_POINTS, 0, _pointCount);
            checkGLError();
        }

        if (updateTrees)
        {
            treeVB->update(tree_points, _treeCount * sizeof(Point2D));
            updateTrees = false;
        }

        if (_treeCount > 0 && showTrees)
        {
            tree_array.Bind();
            treeShader.Bind();
            treeShader.SetUniformMat4f("MVP", window.getMVP());
            glDrawArrays(GL_POINTS, 0, _treeCount);
            checkGLError();
        }

        frame->Render(window.getMVP());

        // Render ImGui frame
        imguiManager.Render();
        if (!window.PreRender())
        {
            socket.~UDPSocket();
            break;
        }
    }

    window.Close();

    // Cleanup
    delete vb;
    delete treeVB;
    delete[] points;
    delete[] tree_points;
    delete[] tree_points_raw;
    delete frame;
    delete octTree;

    return 0;
}
