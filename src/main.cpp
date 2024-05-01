#include <OpenGLWindow.h>
#include <UDPSocket.h>
#include <VertexBuffer.h>
#include <Shader.h>
#include <vector>
#include <random>
#include <VertexBufferLayout.h>
#include <Vertexarray.h>
#include <iostream>
#include <ButtonHandler.h>
#include "MessageParser.h"

using namespace MessageParser;

char ipBuffer[256] = ""; // Buffer to hold ip
char portBuffer[256] = ""; // Buffer to hold port

void checkGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error in " << std::endl;
    }
}

Point3D *points = new Point3D[5000000];
Point2D *tree_points = new Point2D[5000];

VertexBuffer *vb;
VertexBuffer *treeVB;
std::vector<float> pointBuffer;
int _pointCount = 0;
int _treeCount = 0;

int tempPointCount = 0;
bool updatePoints = false;
bool updateTrees = false;
bool startReceived = false;

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
            _pointCount = ReadCloudBufferToPoints(pointBuffer, points);
            startReceived = false;
            updatePoints = true;
        }
        else if (message[1] == 'T')
        {
            _treeCount = AddTreesToBuffer(message, size, tree_points);
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
    ButtonHandler::InitImGui(window.GetWindow());
    Shader shader("../shaders/point.glsl");
    Shader treeShader("../shaders/tree.glsl");

    ButtonHandler button1("SensorIntergrator");
    ButtonHandler button2("Graph");
    ButtonHandler button3("TreeDetection");
    ButtonHandler button4("Stop");

    vb = new VertexBuffer(points, 5000000 * sizeof(Point3D));
    VertexBufferLayout layout;
    layout.PushFloat(3);
    Vertexarray array = Vertexarray();
    array.AddBuffer(*vb, layout);
    treeVB = new VertexBuffer(tree_points, 5000 * sizeof(Point2D));
    Vertexarray tree_array = Vertexarray();
    VertexBufferLayout tree_layout;
    tree_layout.PushFloat(2);
    tree_array.AddBuffer(*treeVB, tree_layout);

    UDPSocket socket(onMessageReceived);

    while(true)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        checkGLError();
        array.Bind();
        shader.Bind();
        shader.SetUniformMat4f("MVP", window.getMVP());
        if (updatePoints)
        {
            vb->update(points, _pointCount * sizeof(Point3D));
            updatePoints = false;
        }


        glDrawArrays(GL_POINTS, 0, _pointCount);
        
        if (updateTrees)
        {
            treeVB->update(tree_points, _treeCount * sizeof(Point2D));
            updateTrees = false;
        }
        tree_array.Bind();
        treeShader.Bind();
        treeShader.SetUniformMat4f("MVP", window.getMVP());
        glDrawArrays(GL_POINTS, 0, _treeCount);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render your buttons
        button1.Render();
        button2.Render();
        button3.Render();
        button4.Render();

        // InputText widget to capture text input
        ImGui::InputText("Ip", ipBuffer, IM_ARRAYSIZE(ipBuffer));

        // InputText widget to capture text input
        ImGui::InputText("Port", portBuffer, IM_ARRAYSIZE(portBuffer));

        // Check if buttons were clicked
        if (button1.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            socket.sendMessage(ipString, portString,"SENSOR");
        }
        else if (button2.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            socket.sendMessage(ipString, portString,"GRAPH");
        }
        else if (button3.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            socket.sendMessage(ipString, portString,"TREE");
        }
        else if (button4.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            socket.sendMessage(ipString, portString,"STOP");
        }

        // Render ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!window.PreRender())
        {
            socket.~UDPSocket();
            ButtonHandler::CleanupImGui();
            break;
        }
    }
    window.Close();
    return 0;
}