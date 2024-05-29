#include "ButtonHandler.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "UDPSocket.h"

class ImGuiManager 
{
public:
    ImGuiManager(GLFWwindow* window, UDPSocket* socket, int& pointCount, int& treeCount, char* ipBuf, char* portBuf, bool& treesVisible, int& sliderValue)
        : window_(window), _socket(socket), _pointCount(pointCount), _treeCount(treeCount), ipBuffer(ipBuf), portBuffer(portBuf), _checkBoxState(treesVisible), _sliderValue(sliderValue) {
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
    UDPSocket* _socket;
    int& _treeCount;
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
        button1.Render();
        button2.Render();
        button3.Render();
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

    void RenderSlider()
    {
        ImGui::SliderInt("Slider", &_sliderValue, 1, 500);
    }
};
