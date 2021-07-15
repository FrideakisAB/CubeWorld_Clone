#include "Log.h"
#include <exception>
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void window_focus_callback(GLFWwindow *window, int focused);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xPos, double yPos);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
    logger = new Log();

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *glfwWindow = glfwCreateWindow(1280, 720, "Cube World", nullptr, nullptr);
    if (glfwWindow == nullptr)
        return -2;

    glfwMakeContextCurrent(glfwWindow);

    glfwSetWindowSizeLimits(glfwWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);
    glfwSetWindowFocusCallback(glfwWindow, window_focus_callback);

    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();

        return -3;
    }

    if (!GLEW_VERSION_4_4)
    {
        glfwTerminate();

        return -4;
    }

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        glfwSwapBuffers(glfwWindow);
    }

    glfwTerminate();
    delete logger;

    return 0;
}

void window_focus_callback(GLFWwindow *window, int focused)
{

}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{

}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{

}

void cursor_position_callback(GLFWwindow *window, double xPos, double yPos)
{

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
