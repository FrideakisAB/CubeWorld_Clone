#include "Render/Shader.h"

Shader::Shader()
{
    ID = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

bool Shader::AddShader(GLenum type, const char *source, const std::string &types)
{
    if (attachmentCount < 5)
    {
        u32 shad = glCreateShader(type);
        glShaderSource(shad, 1, &source, nullptr);
        glCompileShader(shad);
        bool isCompile = checkCompileErrors(shad, types);
        glAttachShader(ID, shad);
        attachments[attachmentCount++] = shad;

        return isCompile;
    }

    return false;
}

bool Shader::Build()
{
    glLinkProgram(ID);
    bool result = checkCompileErrors(ID, "Program");

    for (u32 i = 0; i < attachmentCount; ++i)
        glDeleteShader(attachments[i]);

    return result;
}

void Shader::Use() const
{
    glUseProgram(ID);
}

bool Shader::checkCompileErrors(GLuint shader, const std::string &type)
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

            return false;
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

            return false;
        }
    }

    return true;
}

json Shader::SerializeObj()
{
    return {};
}

void Shader::UnSerializeObj(const json &j)
{
    std::string name = j["shaderName"];
    if (j.contains("vertex"))
        AddShader(GL_VERTEX_SHADER, j["vertex"].get<std::string>().c_str(), name + " vertex");
    if (j.contains("fragment"))
        AddShader(GL_FRAGMENT_SHADER, j["fragment"].get<std::string>().c_str(), name + " fragment");
    if (j.contains("geometry"))
        AddShader(GL_GEOMETRY_SHADER, j["geometry"].get<std::string>().c_str(), name + " geometry");
    if (j.contains("tessEval"))
        AddShader(GL_TESS_EVALUATION_SHADER, j["tessEval"].get<std::string>().c_str(), name + " tess eval");
    if (j.contains("tessControl"))
        AddShader(GL_TESS_CONTROL_SHADER, j["tessControl"].get<std::string>().c_str(), name + " tess control");
    if (j.contains("compute"))
        AddShader(GL_COMPUTE_SHADER, j["compute"].get<std::string>().c_str(), name + " compute");

    Build();
}
