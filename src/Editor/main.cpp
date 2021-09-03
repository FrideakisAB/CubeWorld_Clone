#include "Log.h"
#include "Engine.h"
#include <exception>
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Systems/RenderSystem.h"
#include "ECS/util/Timer.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "Editor/ImGui/imgui_impl_glfw.h"
#include "Editor/ImGui/imgui_impl_opengl3.h"

#include "Editor/ImGui/imgui_dock.h"
#include "Editor/UI/SceneViewer.h"
#include "Editor/Editor.h"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    ImGui::InitDock();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    GameEngine = new Engine();
    GameEditor = new Editor();

    f32 accumulateTime = 0;

    ImGuiContext *g = ImGui::GetCurrentContext();

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        float offsetMBar = g->NextWindowData.MenuBarOffsetMinVal.y + g->FontBaseSize + g->Style.FramePadding.y + 2.0f;
        GameEditor->Menu.Draw();
        ImGui::SetNextWindowPos(ImVec2(0, offsetMBar));
        ImGui::SetNextWindowSize(ImVec2(width, height - offsetMBar));
        ImGui::Begin("Docks", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
        ImGui::BeginDockspace();
        GameEditor->DrawWindows();
        ImGui::EndDockspace();
        ImGui::End();

        accumulateTime += ECS::ECS_Engine->GetTimer()->GetDeltaTime();

        if (accumulateTime >= 0.75f)
        {
            std::string title = std::string("Cube World - ") + std::to_string((int)(1.0f / ECS::ECS_Engine->GetTimer()->GetDeltaTime())) + " FPS";
            glfwSetWindowTitle(glfwWindow, title.c_str());
            accumulateTime = 0.0f;
        }

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Render ImGui stage");
        ImGui::Render();
        glViewport(0, 0, width, height);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glPopDebugGroup();

        glfwSwapBuffers(glfwWindow);
    }

    ImGui::ShutdownDock();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete GameEngine;
    delete GameEditor;
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

    //TODO: remove on input system implement
    if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL))
    {
        if(key == GLFW_KEY_Z && action == GLFW_PRESS)
            GameEditor->CommandList.Undo();
        else if(key == GLFW_KEY_Y && action == GLFW_PRESS)
            GameEditor->CommandList.Redo();
    }
}
