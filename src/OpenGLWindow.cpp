#include "OpenGLWindow.h"

glm::mat4 proj;
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 MVP;
glm::mat4 model = glm::mat4(1.0f); // Initialize to identity matrix


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glfwGetFramebufferSize(window, &width, &height);

    float fov = 45.0f;
    float aspectRatio = (float)width / (float)height;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
    proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    MVP = proj * view * model;

	glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float fwidth = (float)width;
		float fheight = (float)height;
		ypos = height - ypos;
		// Convert cursor position to OpenGL normalized coordinates
		// road->SetNewPath(Vector2(xpos, ypos));
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float fov = 45.0f; // This should be a global variable or a member of your class
    fov -= yoffset; // Zoom out when scrolling up, zoom in when scrolling down
    fov = glm::clamp(fov, 1.0f, 45.0f); // Limit the fov between 1 and 45 degrees

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)width / (float)height;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
    proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    MVP = proj * view * model;
}

OpenGLWindow::OpenGLWindow(int width, int height, const char* title)
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

    float fov = 45.0f;
    float aspectRatio = (float)width / (float)height;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
    proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(cameraPos, cameraTarget, upVector);

    MVP = proj * view * model;

    // Make the window's context current
    makeContextCurrent();
    gladLoadGL();
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scroll_callback);
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