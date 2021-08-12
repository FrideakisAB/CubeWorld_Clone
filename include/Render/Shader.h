#ifndef SHADER_H
#define SHADER_H

#include "Log.h"
#include <string>
#include <glm/glm.hpp>
#include "Render/GLUtils.h"
#include "Assets/ISerialize.h"
#include <glm/gtc/type_ptr.hpp>

class Shader : public ISerialize {
private:
    u32 ID;
    u32 attachments[5]{};
    u32 attachmentCount = 0;

    static bool checkCompileErrors(GLuint shader, const std::string &type);

public:
    Shader();
    ~Shader();

    bool AddShader(GLenum type, const char *source, const std::string &types);
    bool Build();
    void Use() const;

    void SetTexture2D(const std::string &name, uint32_t id, uint32_t num = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + num);
        glBindTexture(GL_TEXTURE_2D, id);
        SetUInt(name, num);
    }

    void SetTextureHandle(const std::string &name, u64 handle) const
    {
        glUniform2uiv(glGetUniformLocation(ID, name.c_str()), 8*2, (const GLuint *)handle);
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

    void SetIVec2(const std::string &name, const glm::ivec2 &value) const
    {
        glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetUVec3(const std::string &name, const glm::uvec3 &value) const
    {
        glUniform3uiv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
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

    void SetTransformFeedbackVaryings(char **names, u32 count, GLenum type = GL_INTERLEAVED_ATTRIBS) const
    {
        glTransformFeedbackVaryings(ID, count, names, type);
        glLinkProgram(ID);
        checkCompileErrors(ID, "Program");
    }

    json SerializeObj() override;
    void UnSerializeObj(const json &j) override;
};

#endif
