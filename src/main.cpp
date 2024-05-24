#include <OpenGLWindow.h>
#include <UDPSocket.h>
#include <VertexBuffer.h>
#include <Shader.h>
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

void checkGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << std::hex << err << std::endl;
    }
}

GridTreeDetector *gridTreeDetector = new GridTreeDetector();
OctoTree *octTree = new OctoTree(-100, -100, -100, 100, 100, 100, 10, 10);
QuadTree *quadTree = new QuadTree(-100, -100, 100, 100, 10, 10);
Point3D *points = new Point3D[5000000];
Point2D *tree_points = new Point2D[50000];
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
int densityScale = 5;


// Callback function
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
            MinMaxValues values = ReadCloudBufferToPoints(pointBuffer, points);
            gridTreeDetector->splitIntoGrids(points, values.pointCount);
            //QuadTree
            //OctoTree
            quadTree->clear();
            quadTree = new QuadTree(values.minX, values.minY, values.maxX, values.maxY, 10, 80);

            // octTree->clear();
            // octTree = new OctoTree(values.minX, values.minY, values.minZ, values.maxX, values.maxY, values.maxZ, 10, 20);

            for (size_t i = 0; i < values.pointCount; i++)
            {
                if (points[i].isVegetation)
                {
                    quadTree->insert(points[i]);
                }
            }
            quadTree->calculateDensity();
            //quadTree->calculateDensityGradient();

            _pointCount = values.pointCount;
            startReceived = false;
            updatePoints = true;
        }
        else if (message[1] == 'T')
        {
            _treeCountTemp = AddTreesToBuffer(message, _treeCountTemp, size, tree_points);
        }
        else if (message[1] == 'D')
        {
            _treeCount = _treeCountTemp;
            _treeCountTemp = 0;
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
    OpenGLWindow window(800, 600, "OpenGL Window");

    vb = new VertexBuffer(points, 5000000 * sizeof(Point3D));
    VertexBufferLayout layout;
    layout.PushFloat(3);
    layout.PushFloat(1);
    layout.PushFloat(1);
    Vertexarray array = Vertexarray();
    array.AddBuffer(*vb, layout);

    treeVB = new VertexBuffer(tree_points, 5000 * sizeof(Point2D));
    Vertexarray tree_array = Vertexarray();
    VertexBufferLayout tree_layout;
    tree_layout.PushFloat(2u);
    tree_array.AddBuffer(*treeVB, tree_layout);

    Shader shader("../shaders/point.glsl");
    Shader treeShader("../shaders/tree.glsl");
    frame = new CoordFrame();
    checkGLError();

    UDPSocket socket(onMessageReceived);

    ImGuiManager imguiManager(window.GetWindow(), &socket, _pointCount, _treeCount, ipBuffer, portBuffer, showTrees, densityScale);

    while (true)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        checkGLError();

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
    delete frame;
    delete octTree;

    return 0;
}
