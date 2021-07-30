#include "Render/Shader.h"

Shader::Shader()
{
    ID = glCreateProgram();
}

void Shader::AddShader(GLenum type, const char* source, const std::string& types)
{
    u32 shad = glCreateShader(type);
    glShaderSource(shad, 1, &source, nullptr);
    glCompileShader(shad);
    checkCompileErrors(shad, types);
    glAttachShader(ID, shad);
    attachments[attachmentCount] = shad;
    ++attachmentCount;
}

void Shader::Build()
{
    glLinkProgram(ID);
    checkCompileErrors(ID, "Program");

    for (u32 i = 0; i < attachmentCount; ++i)
        glDeleteShader(attachments[i]);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "Program")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            logger->Error(std::string("Shader error: " + type));
            logger->Error(infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            logger->Error(std::string("Shader program linking error"));
            logger->Error(infoLog);
        }
    }
}

json Shader::SerializeObj()
{
    return ISerialize::SerializeObj();
}

void Shader::UnSerializeObj(const json &j)
{
    ISerialize::UnSerializeObj(j);
}
