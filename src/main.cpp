#include "Log.h"
#include "Engine.h"
#include <exception>
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Systems/RenderSystem.h"

void error_callback(int error, const char* description);
void window_focus_callback(GLFWwindow *window, int focused);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xPos, double yPos);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
    logger = new Log();

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        logger->Error("Error, GLFW initialisation failed");
        delete logger;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *glfwWindow = glfwCreateWindow(1024, 768, "Cube World", nullptr, nullptr);
    if (glfwWindow == nullptr)
    {
        logger->Error("Error, window creation failed");
        glfwTerminate();
        delete logger;

        return -2;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetWindowSizeLimits(glfwWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);
    glfwSetWindowFocusCallback(glfwWindow, window_focus_callback);

    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    if (GLenum err = glewInit(); err != GLEW_OK)
    {
        logger->Error("Error initialize GLEW, may be OpenGL 4.4 not supported. %s", glewGetErrorString(err));
        glfwTerminate();
        delete logger;

        return -3;
    }

    if (!GLEW_VERSION_4_4)
    {
        logger->Error("Error OpenGL 4.4 not supported");
        glfwTerminate();
        delete logger;

        return -4;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    GameEngine = new Engine();

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        GameEngine->GetRenderSystem().Resize(0, 0, width, height);
        GameEngine->GetRenderSystem().PreUpdate();
        //Render only, remove it
        GameEngine->Update();
        GameEngine->GetRenderSystem().Update();

        glfwSwapBuffers(glfwWindow);

        GameEngine->GetRenderSystem().PostUpdate();
    }

    delete GameEngine;
    glfwTerminate();
    delete logger;

    return 0;
}

void error_callback(int error, const char* description)
{
    logger->Error("GLFW caused error: %s", description);
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
