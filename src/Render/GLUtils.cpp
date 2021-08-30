#include "Render/GLUtils.h"

#include "Log.h"
#include <sstream>

void Utils::DrawQuad()
{
    static u32 quadVAO = 0;
    static u32 quadVBO = 0;
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, void * param)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131154 || id == 1281
        || type == GL_DEBUG_TYPE_PUSH_GROUP || type == GL_DEBUG_TYPE_POP_GROUP) // 1281 - negative number
        return;

    std::stringstream ss;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             ss << "source: API;"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ss << "source: Window System;"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "source: Shader Compiler;"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     ss << "source: Third Party;"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     ss << "source: Application;"; break;
    case GL_DEBUG_SOURCE_OTHER:           ss << "source: Other;"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               ss << " type: Error;"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << " type: Deprecated Behaviour;"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ss << " type: Undefined Behaviour;"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         ss << " type: Portability;"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         ss << " type: Performance;"; break;
    case GL_DEBUG_TYPE_MARKER:              ss << " type: Marker;"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          ss << " type: Push Group;"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           ss << " type: Pop Group;"; break;
    case GL_DEBUG_TYPE_OTHER:               ss << " type: Other;"; break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         ss << " severity: high;"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       ss << " severity: medium;"; break;
    case GL_DEBUG_SEVERITY_LOW:          ss << " severity: low;"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: ss << " severity: notification;"; break;
    }

    ss << " ID:" << id << "; " << message;

    logger->Info("OpenGL message: %s", ss.str().c_str());
}

void Utils::SetupDebugOpenGL()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
