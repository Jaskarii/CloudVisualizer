#include "ButtonHandler.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "UDPSocket.h"

class ImGuiManager 
{
public:
    ImGuiManager(GLFWwindow* window, UDPSocket* socket, int& pointCount, int& treeCount, char* ipBuf, char* portBuf, char* cloudFileBuf, char* treeFileBuf, bool& requestFileLoad, bool& requestTreeFileLoad, bool& requestTreeTransformUpdate, float& treeOffsetX, float& treeOffsetY, float& treeRotationDeg, bool& treesVisible, int& sliderValue)
        : window_(window), _socket(socket), _pointCount(pointCount), _treeCount(treeCount), ipBuffer(ipBuf), portBuffer(portBuf), cloudFileBuffer(cloudFileBuf), treeFileBuffer(treeFileBuf), _requestFileLoad(requestFileLoad), _requestTreeFileLoad(requestTreeFileLoad), _requestTreeTransformUpdate(requestTreeTransformUpdate), _treeOffsetX(treeOffsetX), _treeOffsetY(treeOffsetY), _treeRotationDeg(treeRotationDeg), _checkBoxState(treesVisible), _sliderValue(sliderValue) {
        InitImGui();
    }

    ~ImGuiManager()
    {
        CleanupImGui();
    }

    void Render()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderButtons();
        RenderTextInputs();
        RenderFileLoader();
        ImGui::Checkbox("Trees", &_checkBoxState);
        RenderSlider();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    GLFWwindow* window_;
    int& _pointCount;
    char* ipBuffer;
    char* portBuffer;
    char* cloudFileBuffer;
    char* treeFileBuffer;
    UDPSocket* _socket;
    int& _treeCount;
    bool& _requestFileLoad;
    bool& _requestTreeFileLoad;
    bool& _requestTreeTransformUpdate;
    float& _treeOffsetX;
    float& _treeOffsetY;
    float& _treeRotationDeg;
    bool& _checkBoxState;
    int& _sliderValue; // New member variable for slider value

    ButtonHandler button0{"Pre-Sensor"};
    ButtonHandler button1{"Sensor"};
    ButtonHandler button2{"Graph"};
    ButtonHandler button3{"TreeDetection"};
    ButtonHandler button4{"Stop"};

    void InitImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void CleanupImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void RenderButtons() {
        button0.Render();
        ImGui::SameLine();
        button1.Render();
        ImGui::SameLine();
        button2.Render();
        ImGui::SameLine();
        button3.Render();
        ImGui::SameLine();
        button4.Render();

        if (button1.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            _socket->sendMessage(ipString, portString, "SENSOR");
            _treeCount = 0;
        }
        else if (button0.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            _socket->sendMessage(ipString, portString, "PRESENSOR");
            _treeCount = 0;
        }
        else if (button2.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            _socket->sendMessage(ipString, portString, "GRAPH");
            _treeCount = 0;
        }
        else if (button3.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            _socket->sendMessage(ipString, portString, "TREE");
        }
        else if (button4.WasClicked())
        {
            std::string ipString(ipBuffer);
            std::string portString(portBuffer);
            _socket->sendMessage(ipString, portString, "STOP");
        }
    }

    void RenderTextInputs()
    {
        ImGui::Text("Point Count: %d", _pointCount);
        ImGui::InputText("Ip", ipBuffer, 32);
        ImGui::InputText("Port", portBuffer, 8);
    }

    void RenderFileLoader()
    {
        ImGui::InputText("Cloud File", cloudFileBuffer, 1024);
        if (ImGui::Button("Load Cloud"))
        {
            _requestFileLoad = true;
            _treeCount = 0;
        }

        ImGui::InputText("Tree XML", treeFileBuffer, 1024);
        if (ImGui::Button("Load Trees"))
        {
            _requestTreeFileLoad = true;
        }

        bool transformChanged = false;
        transformChanged |= ImGui::SliderFloat("Tree Offset X", &_treeOffsetX, -500.0f, 500.0f, "%.3f");
        transformChanged |= ImGui::SliderFloat("Tree Offset Y", &_treeOffsetY, -500.0f, 500.0f, "%.3f");
        transformChanged |= ImGui::SliderFloat("Tree Rotation (deg)", &_treeRotationDeg, -180.0f, 180.0f, "%.3f");

        ImGui::Text("Fine tune (0.01 step)");
        transformChanged |= ImGui::InputFloat("Offset X precise", &_treeOffsetX, 0.01f, 0.1f, "%.2f");
        transformChanged |= ImGui::InputFloat("Offset Y precise", &_treeOffsetY, 0.01f, 0.1f, "%.2f");
        transformChanged |= ImGui::InputFloat("Rotation precise", &_treeRotationDeg, 0.01f, 0.1f, "%.2f");

        if (ImGui::Button("Reset Tree Transform"))
        {
            _treeOffsetX = 0.0f;
            _treeOffsetY = 0.0f;
            _treeRotationDeg = 0.0f;
            transformChanged = true;
        }

        if (transformChanged)
        {
            _requestTreeTransformUpdate = true;
        }
    }

    void RenderSlider()
    {
        ImGui::SliderInt("Slider", &_sliderValue, 1, 500);
    }
};
