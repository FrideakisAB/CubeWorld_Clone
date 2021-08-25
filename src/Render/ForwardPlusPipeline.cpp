#include "Render/ForwardPlusPipeline.h"

#include <array>
#include "Engine.h"
#include "Components/Camera.h"
#include "Components/Transform.h"

ForwardPlusPipeline::ForwardPlusPipeline()
    : pointIndices(1024 * 64 * 48), spotIndices(1024 * 64 * 48)
{
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, pingPongFBO);
    glGenTextures(2, pingPongBuffers);
    for (u32 i = 0; i < 2; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingPongBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 768, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongBuffers[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &hdrFBO);
    glGenTextures(2, colorBuffers);
    for (u32 colorBuffer : colorBuffers)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 768, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    for (u32 i = 0; i < 2; ++i)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    u32 attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

ForwardPlusPipeline::~ForwardPlusPipeline()
{
    glDeleteBuffers(1, &skyboxVBO);

    glDeleteVertexArrays(1, &skyboxVBO);

    std::array textures = { depthMap, colorBuffers[0], colorBuffers[1] };
    glDeleteTextures(textures.size(), textures.data());

    std::array FBOs = { depthMapFBO, hdrFBO };
    glDeleteFramebuffers(FBOs.size(), FBOs.data());
}

void ForwardPlusPipeline::ApplyTasks(std::map<std::string, RenderSystem::MaterialMap> &renderObjects, std::vector<RenderSystem::RenderTask> &renderTasks)
{
    this->renderObjects = &renderObjects;
    this->renderTasks = &renderTasks;
}

void ForwardPlusPipeline::ApplyMaterials(std::unordered_map<Material*, RenderSystem::MaterialSet> &materialTranslation)
{
    this->materialTranslation = &materialTranslation;
}

void ForwardPlusPipeline::ApplyShaders(std::unordered_map<std::string, Shader> &shaders)
{
    this->shaders = &shaders;
}

void ForwardPlusPipeline::ApplyLights(std::optional<DirectionLight> directionLight, SSBO<PointLight> &pointLights, u16 pointLightPos, SSBO<SpotLight> &spotLights, u16 spotLightPos)
{
    this->directionLight = directionLight;
    this->pointLights = &pointLights;
    this->pointLightPos = pointLightPos;
    this->spotLights = &spotLights;
    this->spotLightPos = spotLightPos;
}

void ForwardPlusPipeline::ApplyLightSources(std::vector<Utils::PointLight> &pointLightSources, std::vector<Utils::SpotLight> &spotLightSources)
{
    this->pointLightSources = &pointLightSources;
    this->spotLightSources = &spotLightSources;
}

void ForwardPlusPipeline::Resize(u16 offsetX, u16 offsetY, u16 width, u16 height)
{
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    for (u32 colorBuffer : colorBuffers)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    for (u32 pingPongBuffer : pingPongBuffers)
    {
        glBindTexture(GL_TEXTURE_2D, pingPongBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    pointIndices.UploadData(1024 * ((width + width % 16) / 16) * ((height + height % 16) / 16), nullptr, GL_SHADER_STORAGE_BUFFER, GL_STREAM_DRAW);
    spotIndices.UploadData(1024 * ((width + width % 16) / 16) * ((height + height % 16) / 16), nullptr, GL_SHADER_STORAGE_BUFFER, GL_STREAM_DRAW);

    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->width = width;
    this->height = height;

    glViewport(offsetX, offsetY, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ForwardPlusPipeline::Render()
{
    glm::vec3 cameraPosition = ECS::ECS_Engine->GetEntityManager()->GetEntity(Camera::Main->GetOwner())->GetComponent<Transform>()->GetGlobalPos().position;
    glm::mat4 mainVP = Camera::Main->GetVPMatrix(width, height);

    // Shadows stage
    shadowsManager.ApplyLightSources(directionLight, *pointLightSources, *spotLightSources);
    shadowsManager.Render(cameraPosition, *shaders, *renderTasks);

    glViewport(offsetX, offsetY, width, height);

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Depth stage");
    Shader &depth = (*shaders)["Depth"];
    depth.Use();
    depth.SetMat4("vp", mainVP);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto &rTask : *renderTasks)
    {
        if (rTask.Mask.NoShadows)
            continue;

        depth.SetMat4("model", rTask.Transform);
        glBindVertexArray(rTask.DrawData.VAO);
        if (!rTask.DrawData.Arrays)
            glDrawElements(Utils::GetDrawModeGL(rTask.DrawData.Mode), rTask.DrawData.Count, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(Utils::GetDrawModeGL(rTask.DrawData.Mode), 0, rTask.DrawData.Count);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Light culling stage");
    Shader &pointLightCulling = (*shaders)["PointLightCulling"];
    pointLightCulling.Use();
    pointLightCulling.SetIVec2("screenSize", glm::ivec2(width, height));
    pointLightCulling.SetInt("lightCount", pointLightPos);
    pointLightCulling.SetMat4("projection", Camera::Main->GetProjMatrix(width, height));
    pointLightCulling.SetMat4("view", Camera::Main->GetViewMatrix());

    glActiveTexture(GL_TEXTURE4);
    pointLightCulling.SetInt("depthMap", 4);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    pointLights->Bind(0);
    pointIndices.Bind(1);

    glDispatchCompute((width + width % 16) / 16, (height + height % 16) / 16, 1);

    Shader &spotLightCulling = (*shaders)["SpotLightCulling"];
    spotLightCulling.Use();
    pointLightCulling.SetIVec2("screenSize", glm::ivec2(width, height));
    pointLightCulling.SetInt("lightCount", spotLightPos);
    spotLightCulling.SetMat4("projection", Camera::Main->GetProjMatrix(width, height));
    spotLightCulling.SetMat4("view", Camera::Main->GetViewMatrix());

    glActiveTexture(GL_TEXTURE4);
    spotLightCulling.SetInt("depthMap", 4);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    spotLights->Bind(0);
    spotIndices.Bind(1);

    glDispatchCompute((width + width % 16) / 16, (height + height % 16) / 16, 1);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Render opaque stage");
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pointLights->Bind(0);
    spotLights->Bind(1);

    pointIndices.Bind(2);
    spotIndices.Bind(3);

    u8 lastTexture = shadowsManager.AttachShadowsData();

    for (const auto &[name, materialMap] : *renderObjects)
    {
        Shader &shader = (*shaders)[name];
        shader.Use();

        shadowsManager.SetUniforms(shader);

        shader.SetFloat("ambient", GameEngine->GetLighting().Ambient);
        shader.SetFloat("wh", (f32)width / (f32)height);

        shader.SetMat4("vp", mainVP);
        shader.SetInt("numberOfTilesX", (width + width % 16) / 16);
        shader.SetVec3("viewPos", cameraPosition);

        if (directionLight)
        {
            shader.SetVec4("dirLight.position", directionLight.value().position);
            shader.SetVec4("dirLight.colorAndIntensity", directionLight.value().colorAndIntensity);
            shader.SetVec4("dirLight.direction", directionLight.value().direction);
        }

        for (const auto &[materialPtr, taskIDs] : materialMap)
        {
            setupMaterial(shader, materialPtr, lastTexture);
            for (const auto id : taskIDs)
            {
                RenderSystem::RenderTask &renderTask = (*renderTasks)[id];

                shader.SetMat4("model", renderTask.Transform);
                glBindVertexArray(renderTask.DrawData.VAO);
                if (!renderTask.DrawData.Arrays)
                    glDrawElements(Utils::GetDrawModeGL(renderTask.DrawData.Mode), renderTask.DrawData.Count, GL_UNSIGNED_INT, nullptr);
                else
                    glDrawArrays(Utils::GetDrawModeGL(renderTask.DrawData.Mode), 0, renderTask.DrawData.Count);
            }
        }
    }
    glPopDebugGroup();

    if (Camera::Main->IsValidSkybox() && Camera::Main->GetSkybox().GetDrawData().TextureType == TexType::TextureCube)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Render skybox stage");
        glDepthFunc(GL_LEQUAL);
        Shader &skyboxShader = (*shaders)["Skybox"];
        skyboxShader.Use();
        skyboxShader.SetInt("skybox", 2);
        glm::mat4 view = glm::mat4(glm::mat3(Camera::Main->GetViewMatrix()));
        skyboxShader.SetMat4("vp", Camera::Main->GetProjMatrix(width, height) * view);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Camera::Main->GetSkybox().GetDrawData().Handle);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        glPopDebugGroup();
    }

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Bloom blur stage");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    bool horizontal = true, first_iteration = true;
    Shader &blur = (*shaders)["Blur"];
    blur.Use();
    for (u32 i = 0; i < blurAmount; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
        blur.SetInt("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingPongBuffers[!horizontal]);
        Utils::DrawQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glPopDebugGroup();

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "HDR stage");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Shader &hdr = (*shaders)["HDR"];
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
    hdr.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    hdr.SetInt("hdrBuffer", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongBuffers[!horizontal]);
    hdr.SetInt("bloomBlur", 1);
    hdr.SetFloat("exposure", GameEngine->GetLighting().Exposure);
    Utils::DrawQuad();
    glPopDebugGroup();
}

void ForwardPlusPipeline::Release()
{

}

void ForwardPlusPipeline::setupMaterial(Shader &shader, Material *material, u8 lastTexture)
{
    RenderSystem::MaterialSet &materialSet = (*materialTranslation)[material];

    for (const auto &uniform : materialSet.Uniforms)
    {
        switch (uniform.second.valueType)
        {
        case Utils::ShaderValue::Int:
            shader.SetInt(uniform.first, std::get<i32>(uniform.second.value));
            break;

        case Utils::ShaderValue::UnsignedInt:
            shader.SetUInt(uniform.first, std::get<u32>(uniform.second.value));
            break;

        case Utils::ShaderValue::Float:
            shader.SetFloat(uniform.first, std::get<f32>(uniform.second.value));
            break;

        case Utils::ShaderValue::Double:
            shader.SetDouble(uniform.first, std::get<f64>(uniform.second.value));
            break;

        case Utils::ShaderValue::Vector2:
            shader.SetVec2(uniform.first, std::get<glm::vec2>(uniform.second.value));
            break;

        case Utils::ShaderValue::Vector3:
            shader.SetVec3(uniform.first, std::get<glm::vec3>(uniform.second.value));
            break;

        case Utils::ShaderValue::Vector4:
            shader.SetVec4(uniform.first, std::get<glm::vec4>(uniform.second.value));
            break;

        case Utils::ShaderValue::Mat2:
            shader.SetMat2(uniform.first, std::get<glm::mat2>(uniform.second.value));
            break;

        case Utils::ShaderValue::Mat3:
            shader.SetMat3(uniform.first, std::get<glm::mat3>(uniform.second.value));
            break;

        case Utils::ShaderValue::Mat4:
            shader.SetMat4(uniform.first, std::get<glm::mat4>(uniform.second.value));
            break;
        }
    }

    for (const auto &sampler : materialSet.Samplers)
        shader.SetTexture2D(sampler.first, sampler.second.Handle, ++lastTexture);
}
