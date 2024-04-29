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

#define PORT 8080

struct Point3D
{
    float x, y, z;
};

void checkGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error in " << std::endl;
    }
}

Point3D *points = new Point3D[200000];
VertexBuffer *vb;
std::vector<float> buffer;
int _pointCount = 0;
int tempPointCount = 0;
bool updatePoints = false;
bool startReceived = false;

// Callback function
void onMessageReceived(const char *message, size_t size)
{
    std::string receivedMessage(message, size);

    if (receivedMessage == "START_CLOUD")
    {
        buffer.clear();
        startReceived = true;
        return;
    }
    else if (receivedMessage == "END_CLOUD")
    {
        tempPointCount = 0;
        // If all packages have been received, update the points array and the vertex buffer
        for (int i = 0; i < buffer.size() / 3; i++)
        {
            points[i].x = buffer[i * 3];
            points[i].y = buffer[i * 3 + 1];
            points[i].z = buffer[i * 3 + 2];

            tempPointCount++;
        }
        // Update the vertex buffer
        _pointCount = tempPointCount;
        updatePoints = true;
        startReceived = false;
        return;
    }
    else if (!startReceived)
    {
        return;
    }
    // Interpret the message as a series of uint32_t
    const uint32_t *uintArray = reinterpret_cast<const uint32_t *>(message);
    int count = 0;

    // Convert the data from network byte order to host byte order and add the points to the buffer
    for (int i = 0; i < size / 4; i++)
    {
        uint32_t hostByteOrderData = ntohl(uintArray[i]);
        float hostByteOrderFloat = *reinterpret_cast<float *>(&hostByteOrderData);

        buffer.push_back(hostByteOrderFloat);
        count++;
    }
}

int main()
{
    OpenGLWindow window(800, 600, "OpenGL Window");
    ButtonHandler::InitImGui(window.GetWindow());
    Shader shader("../shaders/point.glsl");

    ButtonHandler button1("SensorIntergrator");
    ButtonHandler button2("Graph");
    ButtonHandler button3("TreeDetection");
    vb = new VertexBuffer(points, 100000 * sizeof(Point3D));
    VertexBufferLayout layout;
    layout.PushFloat(3);
    Vertexarray array = Vertexarray();
    array.AddBuffer(*vb, layout);
    UDPSocket socket(onMessageReceived);

    for (size_t i = 0; i < 20000; i++)
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
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render your buttons
        button1.Render();
        button2.Render();

        // Check if buttons were clicked
        if (button1.WasClicked())
        {
            socket.sendMessage("START_SENSOR");
        }
        if (button2.WasClicked())
        {
            int b = 0;
        }
        if (button3.WasClicked())
        {
            int b = 0;
        }

        // Render ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.PreRender();

    }
    window.Close();
    return 0;
}