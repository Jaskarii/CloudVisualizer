#include "ImGui/imgui.h"

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

private:
    const char* name;
    bool isClicked;
};