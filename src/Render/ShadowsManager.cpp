#include "Render/ShadowsManager.h"

#include <array>

ShadowsManager::ShadowsManager()
    : pointLightPositions(pointLightHighShadowCount + pointLightMediumShadowCount + pointLightLowShadowCount)
{
    //TODO: read all variables from config file
    isDirectionalShadowsActive = true;
    isPointShadowsActive = true;
    isSpotShadowsActive = true;
    isShadowsActive = true;
    reserve = false;

    if (!isDirectionalShadowsActive)
        dirLightShadowResolution = 1;
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

    if (!isPointShadowsActive)
        pointLightShadowResolution = 1;
    glGenFramebuffers(3, pointDepthMapFBO);

    glGenTextures(3, pointDepthMaps);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointDepthMaps[0]);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT,
                 pointLightShadowResolution, pointLightShadowResolution, pointLightHighShadowCount * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointDepthMaps[1]);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT,
                 pointLightShadowResolution / 2, pointLightShadowResolution / 2, pointLightMediumShadowCount * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointDepthMaps[2]);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT,
                 pointLightShadowResolution / 4, pointLightShadowResolution / 4, pointLightLowShadowCount * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[0]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMaps[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[1]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMaps[1], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[2]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMaps[2], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowsManager::~ShadowsManager()
{
    std::array textures = { dirDepthMap, pointDepthMaps[0], pointDepthMaps[1], pointDepthMaps[2] };
    glDeleteTextures(textures.size(), textures.data());

    std::array FBOs = { dirDepthMapFBO, pointDepthMapFBO[0], pointDepthMapFBO[1], pointDepthMapFBO[2] };
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

                dirLightVP = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, dirLightDrawDistance);
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

        pointShadowCount = glm::uvec3(0);

        if (isPointShadowsActive && !(*pointLightSources).empty())
        {
            std::sort((*pointLightSources).begin(), (*pointLightSources).end(), [cameraPosition](const PointLight &p1, const PointLight &p2){
                return glm::distance(glm::vec3(p1.positionAndIntensity), cameraPosition) < glm::distance(glm::vec3(p2.positionAndIntensity), cameraPosition);
            });

            Shader &depth = shaders["PointDepth"];
            depth.Use();

            pointLightPositions.Open();
            glm::vec4 *lights = pointLightPositions.GetData();

            for (u32 i = 0; i < (pointLightHighShadowCount + pointLightMediumShadowCount + pointLightLowShadowCount) && i < (*pointLightSources).size(); ++i)
            {
                if (glm::distance(glm::vec3((*pointLightSources)[i].positionAndIntensity), cameraPosition) < shadowDrawDistance)
                {
                    if (i < pointLightHighShadowCount)
                    {
                        ++pointShadowCount.x;
                        depth.SetInt("layer", i);

                        if (i == 0)
                        {
                            glViewport(0, 0, pointLightShadowResolution, pointLightShadowResolution);
                            glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[0]);
                            glClear(GL_DEPTH_BUFFER_BIT);
                        }
                    }
                    else if (i < pointLightMediumShadowCount + pointLightHighShadowCount)
                    {
                        ++pointShadowCount.y;
                        depth.SetInt("layer", i - pointLightHighShadowCount);

                        if (i - pointLightHighShadowCount == 0)
                        {
                            glViewport(0, 0, pointLightShadowResolution / 2, pointLightShadowResolution / 2);
                            glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[1]);
                            glClear(GL_DEPTH_BUFFER_BIT);
                        }
                    }
                    else if (i < pointLightLowShadowCount + pointLightMediumShadowCount + pointLightHighShadowCount)
                    {
                        ++pointShadowCount.z;
                        depth.SetInt("layer", i - pointLightHighShadowCount - pointLightMediumShadowCount);

                        if (i - pointLightHighShadowCount - pointLightMediumShadowCount == 0)
                        {
                            glViewport(0, 0, pointLightShadowResolution / 4, pointLightShadowResolution / 4);
                            glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO[2]);
                            glClear(GL_DEPTH_BUFFER_BIT);
                        }
                    }
                    else
                        break;

                    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, (*pointLightSources)[i].colorAndRadius.w);

                    lights[i] = (*pointLightSources)[i].positionAndIntensity;
                    lights[i].w = (*pointLightSources)[i].colorAndRadius.w;
                    auto lightPos = glm::vec3((*pointLightSources)[i].positionAndIntensity);

                    std::array<glm::mat4, 6> shadowTransforms {
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)),
                    };

                    for (u32 j = 0; j < 6; ++j)
                        depth.SetMat4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
                    depth.SetFloat("far_plane", (*pointLightSources)[i].colorAndRadius.w);
                    depth.SetVec3("lightPos", lightPos);

                    for (const auto &task : renderTasks)
                    {
                        depth.SetMat4("model", task.Transform);
                        glBindVertexArray(task.DrawData.VAO);
                        glDrawElements(Utils::GetDrawModeGL(task.DrawData.Mode), task.DrawData.Count, GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
            pointLightPositions.Close();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
