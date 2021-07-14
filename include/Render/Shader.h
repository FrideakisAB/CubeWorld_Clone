#ifndef SHADER_H
#define SHADER_H

#include "Log.h"
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include "Render/GLUtils.h"
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
    u32 ID;
    u32 attachments[6]{};

public:
    Shader()
    {
        ID = glCreateProgram();
        attachments[5] = 0;
    }

    void AddShader(GLenum type, const char* source, const std::string& types)
    {
        u32 shad = glCreateShader(type);
        glShaderSource(shad, 1, &source, nullptr);
        glCompileShader(shad);
        checkCompileErrors(shad, types);
        glAttachShader(ID, shad);
        attachments[attachments[5]] = shad;
        ++attachments[5];
    }

    void Build()
    {
        glLinkProgram(ID);
        checkCompileErrors(ID, "Program");

        for (u32 i = 0; i < attachments[5]; ++i)
            glDeleteShader(attachments[i]);
    }

    void Use() const
    {
        glUseProgram(ID);
    }

    void SetTexture2D(const std::string &name, uint32_t id, uint32_t num = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + num);
        glBindTexture(GL_TEXTURE_2D, id);
        SetUInt(name, num);
    }

    void SetBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void SetInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetUInt(const std::string &name, u32 value) const
    {
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetTransformFeedbackVaryings(char** names, u32 count, GLenum type=GL_INTERLEAVED_ATTRIBS) const
    {
        glTransformFeedbackVaryings(ID, count, names, type);
        glLinkProgram(ID);
        checkCompileErrors(ID, "Program");
    }

private:
    static void checkCompileErrors(GLuint shader, const std::string& type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "Program")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                logger->Error(std::string("Shader error: " + type));
                logger->Error(infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                logger->Error(std::string("Shader program linking error"));
                logger->Error(infoLog);
            }
        }
    }
};

#endif
