#include "OpenGLWindow.h"

glm::mat4 proj;
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 MVP;
glm::mat4 model = glm::mat4(1.0f); // Initialize to identity matrix



float left = -50.0f;
float right = 50.0f;
float bottom = -50.0f;
float top = 50.0f;
float depth = -100.0f;
float near = 100.0f;
float scale = 1.0f;

bool leftMouseButtonPressed = false;
bool middleMouseButtonPressed = false;
double lastX = 0.0;
double lastY = 0.0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)width / (float)height;
    // Create an orthographic projection that maps the world coordinates to the window dimensions
    proj = glm::ortho(scale*left, scale*right, scale*bottom / aspectRatio, scale*top / aspectRatio, scale*depth, scale*near);
    MVP = proj * view * model;

    glViewport(0, 0, width, height);
}

void initCamera()
{
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 targetPos = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 0.0f, 1.0f);

    view = glm::lookAt(cameraPos, targetPos, upVector);
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (leftMouseButtonPressed)
    {
        // Calculate the change in mouse position
        double deltaX = xpos - lastX;
        double deltaY = ypos - lastY;

        // Update last mouse position
        lastX = xpos;
        lastY = ypos;

        // Set sensitivity for rotation
        float sensitivity = 0.01f;

        // Perform rotation based on mouse movement
        view = glm::rotate(view, (float)deltaX * sensitivity, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around z-axis
        view = glm::rotate(view, (float)deltaY * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis

        // Recalculate MVP matrix
        MVP = proj * view * model;
    }
    else if (middleMouseButtonPressed)
    {
        // Calculate the change in mouse position
        double deltaX = xpos - lastX;
        double deltaY = ypos - lastY;

        // Update last mouse position
        lastX = xpos;
        lastY = ypos;

        // Set sensitivity for panning
        float sensitivity = 0.02f;

        // Perform translation based on mouse movement
        view = glm::translate(view, glm::vec3((float)deltaX * sensitivity, -(float)deltaY * sensitivity, 0.0f));

        // Recalculate MVP matrix
        MVP = proj * view * model;
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            leftMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE)
        {
            leftMouseButtonPressed = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            middleMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE)
        {
            middleMouseButtonPressed = false;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    float zoomSpeed = 0.1f;
    scale += yoffset * zoomSpeed; // Zoom in when scrolling up, zoom out when scrolling down
    scale = glm::clamp(scale, 0.1f, 10.0f); // Limit the scale between 0.1 and 10

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)width / (float)height;

    // Adjust the orthographic projection based on the scale
    proj = glm::ortho(scale*left, scale*right, scale*bottom / aspectRatio, scale*top / aspectRatio, scale*depth, scale*near);
    MVP = proj * view * model;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        bool altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;

        switch (key)
        {
            case GLFW_KEY_SPACE:
                initCamera();
                break;
            case GLFW_KEY_UP:
                if (altPressed) {
                    // Pan camera up
                    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f));
                } else {
                    // Rotate camera up (around the x-axis)
                    view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                break;
            case GLFW_KEY_DOWN:
                if (altPressed) {
                    // Pan camera down
                    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
                } else {
                    // Rotate camera down (around the x-axis)
                    view = glm::rotate(view, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                break;
            case GLFW_KEY_LEFT:
                if (altPressed) {
                    // Pan camera left
                    view = glm::translate(view, glm::vec3(0.0f, -0.1f, 0.0f));
                } else {
                    // Rotate camera left (around the z-axis)
                    view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                break;
            case GLFW_KEY_RIGHT:
                if (altPressed) {
                    // Pan camera right
                    view = glm::translate(view, glm::vec3(0.0f, 0.1f, 0.0f));
                } else {
                    // Rotate camera right (around the z-axis)
                    view = glm::rotate(view, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                break;
            default:
                break;
        }
        MVP = proj * view * model;
    }
}

OpenGLWindow::OpenGLWindow(int width, int height, const char *title)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        // Handle initialization failure
        // You can throw an exception or handle the error in your own way
    }

    // Create a GLFW window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    initCamera();
    // Make the window's context current
    makeContextCurrent();
    gladLoadGL();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursorPositionCallback); // Register cursor position callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
}

OpenGLWindow::~OpenGLWindow()
{
    // Destroy the GLFW window
    glfwDestroyWindow(window);

    // Terminate GLFW
    glfwTerminate();
}

bool OpenGLWindow::PreRender()
{
    if (shouldClose())
    {
        return false;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    return true;
}

void OpenGLWindow::makeContextCurrent()
{
    // Make the window's context current
    glfwMakeContextCurrent(window);
}

bool OpenGLWindow::shouldClose()
{
    // Check if the window should close
    return glfwWindowShouldClose(window);
}

void OpenGLWindow::setShouldClose(bool shouldClose)
{
    // Set whether the window should close
    glfwSetWindowShouldClose(window, shouldClose);
}

void OpenGLWindow::Close()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

glm::mat4 OpenGLWindow::getMVP()
{
    return MVP;
}

GLFWwindow* OpenGLWindow::GetWindow()
{
    return window;
}

int OpenGLWindow::getWidth()
{
    // Get the width of the window
    int width;
    glfwGetWindowSize(window, &width, nullptr);
    return width;
}

int OpenGLWindow::getHeight()
{
    // Get the height of the window
    int height;
    glfwGetWindowSize(window, nullptr, &height);
    return height;
}