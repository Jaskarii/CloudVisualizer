#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OpenGLWindow 
{
public:
    OpenGLWindow(int width, int height, const char* title);
    ~OpenGLWindow();

    bool PreRender();

    void makeContextCurrent();
    void swapBuffers();
    void pollEvents();
    void setShouldClose(bool shouldClose);
    void Close();
    glm::mat4 getMVP();


    int getWidth();
    int getHeight();

private:
    bool shouldClose();
    GLFWwindow *window;
};