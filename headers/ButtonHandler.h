#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

class ButtonHandler {
public:
    ButtonHandler(const char* buttonName) : name(buttonName), isClicked(false) {}

    void Render() {
        if (ImGui::Button(name)) {
            isClicked = true;
        } else {
            isClicked = false;
        }
    }

    bool WasClicked() const {
        return isClicked;
    }

    static void InitImGui(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    static void CleanupImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

private:
    const char* name;
    bool isClicked;
};