#include "Render/ShadowsManager.h"

#include <array>

ShadowsManager::ShadowsManager()
{
    //TODO: read all variables from config file
    isDirectionalShadowsActive = true;
    isPointShadowsActive = true;
    isSpotShadowsActive = true;
    isShadowsActive = true;

    glGenFramebuffers(1, &dirDepthMapFBO);
    glGenTextures(1, &dirDepthMap);
    glBindTexture(GL_TEXTURE_2D, dirDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dirLightShadowResolution, dirLightShadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, dirDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    isDeleteDirClear = true;
}

ShadowsManager::~ShadowsManager()
{
    std::array textures = { dirDepthMap };
    glDeleteTextures(textures.size(), textures.data());

    std::array FBOs = { dirDepthMapFBO };
    glDeleteFramebuffers(FBOs.size(), FBOs.data());
}

void ShadowsManager::ApplyLightSources(std::optional<DirectionLight> directionLight, std::vector<Utils::PointLight> &pointLightSources, std::vector<Utils::SpotLight> &spotLightSources)
{
    this->directionLight = directionLight;
    this->pointLightSources = &pointLightSources;
    this->spotLightSources = &spotLightSources;
}

void ShadowsManager::Render(glm::vec3 cameraPosition, std::unordered_map<std::string, Shader> &shaders, std::vector<RenderSystem::RenderTask> &renderTasks)
{
    if (isShadowsActive)
    {
        if (isDirectionalShadowsActive)
        {
            if (directionLight)
            {
                isDeleteDirClear = false;

                Shader &depth = shaders["Depth"];
                depth.Use();

                glViewport(0, 0, dirLightShadowResolution, dirLightShadowResolution);

                dirLightVP = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, dirLightDrawDistance);
                glm::vec3 center =  glm::vec3(directionLight.value().position) + glm::vec3(directionLight.value().direction);
                dirLightVP *= glm::lookAt(glm::vec3(directionLight.value().position), center, glm::vec3(0.0, 1.0, 0.0));
                depth.SetMat4("vp", dirLightVP);

                glBindFramebuffer(GL_FRAMEBUFFER, dirDepthMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
                for (const auto &rTask : renderTasks)
                {
                    depth.SetMat4("model", rTask.Transform);
                    glBindVertexArray(rTask.DrawData.VAO);
                    glDrawElements(Utils::GetDrawModeGL(rTask.DrawData.Mode), rTask.DrawData.Count, GL_UNSIGNED_INT, nullptr);
                }
            }
            else if (!isDeleteDirClear)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, dirDepthMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
                isDeleteDirClear = true;
            }
        }
    }
}
