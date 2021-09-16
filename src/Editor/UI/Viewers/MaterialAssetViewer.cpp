#include "Editor/UI/Viewers/MaterialAssetViewer.h"

#include "Engine.h"
#include "Editor/Editor.h"
#include "Systems/RenderSystem.h"
#include "Editor/ImGui/ImCustom.h"
#include "Components/MaterialComponent.h"
#include "Editor/Commands/ViewersCommands.h"

void MaterialAssetViewer::OnEditorUI(IAsset &asset)
{
    auto &material = dynamic_cast<Material&>(asset);

    if (MaterialEditor(material))
    {
        //TODO: save in history
        /*if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
            lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<MaterialComponent>>(&go);*/

        ApplyChanges(material);
    }
}

bool MaterialAssetViewer::MaterialEditor(Material &material)
{
    bool update = false;

    shaderStr = "";
    paramName = "";
    textureAsset = "";
    paramValue = {};

    std::vector<const char*> shadersName;
    shadersName.reserve(GameEngine->GetRenderSystem().GetShaders().size());
    int itemCurrent = -1;
    for (const auto &value : GameEngine->GetRenderSystem().GetShaders())
    {
        if (value.first == material.Shader)
            itemCurrent = shadersName.size();
        shadersName.push_back(value.first.c_str());
    }

    if (itemCurrent == -1)
    {
        update = true;
        itemCurrent = 0;
    }

    ImGui::Combo("Shader", &itemCurrent, &shadersName[0], shadersName.size());
    if (material.Shader != shadersName[itemCurrent])
    {
        shaderStr = shadersName[itemCurrent];
        update = true;
    }

    const Shader &shader = GameEngine->GetRenderSystem().GetShaders().find(shadersName[itemCurrent])->second;
    if (!shader.GetParameters().empty())
    {
        for (const auto &parameter : shader.GetParameters())
        {
            switch (parameter.valueType)
            {
            case Utils::ShaderValue::Int:
            {
                int value = 0, oldValue = 0;
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<int>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                ImGui::InputInt(Utils::ParseUniformName(parameter.name).c_str(), &value);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::UnsignedInt:
            {
                u32 value = 0, oldValue = 0;
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<u32>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                ImGui::InputScalar(Utils::ParseUniformName(parameter.name).c_str(), ImGuiDataType_U32, &value);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Float:
            {
                f32 value = 0, oldValue = 0;
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<f32>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                ImGui::InputFloat(Utils::ParseUniformName(parameter.name).c_str(), &value);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Double:
            {
                f64 value = 0, oldValue = 0;
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<f64>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                ImGui::InputDouble(Utils::ParseUniformName(parameter.name).c_str(), &value);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Vector2:
            {
                glm::vec2 value = {}, oldValue = {};
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<glm::vec2>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                ImGui::InputFloat2(Utils::ParseUniformName(parameter.name).c_str(), &value[0]);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Vector3:
            {
                glm::vec3 value = {}, oldValue = {};
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<glm::vec3>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                if (auto pos = parameter.name.find("color_"); pos != std::string::npos)
                    ImGui::ColorEdit3(Utils::ParseUniformName(parameter.name.substr(pos + 6)).c_str(), &value[0]);
                else
                    ImGui::InputFloat3(Utils::ParseUniformName(parameter.name).c_str(), &value[0]);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Vector4:
            {
                glm::vec4 value = {}, oldValue = {};
                if (auto It = material.Uniforms.find(parameter.name);
                        It != material.Uniforms.end() &&
                        It->second.valueType == parameter.valueType)
                {
                    value = std::get<glm::vec4>(It->second.value);
                    oldValue = value;
                }
                else
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }

                if (auto pos = parameter.name.find("color_"); pos != std::string::npos)
                    ImGui::ColorEdit4(Utils::ParseUniformName(parameter.name.substr(pos + 6)).c_str(), &value[0]);
                else
                    ImGui::InputFloat4(Utils::ParseUniformName(parameter.name).c_str(), &value[0]);
                if (value != oldValue)
                {
                    paramName = parameter.name;
                    paramValue.value = value;
                    paramValue.valueType = parameter.valueType;
                    update = true;
                }
            }
                break;

            case Utils::ShaderValue::Mat2:
                ImGui::Text("Mat2 %s", parameter.name.c_str());
                break;

            case Utils::ShaderValue::Mat3:
                ImGui::Text("Mat3 %s", parameter.name.c_str());
                break;

            case Utils::ShaderValue::Mat4:
                ImGui::Text("Mat4 %s", parameter.name.c_str());
                break;

            case Utils::ShaderValue::Sampler1D:
            {
                CustomTextState texState = CustomTextState::None;
                std::string texContext = "None";

                if (auto It = material.Samplers.find(parameter.name);
                        It != material.Samplers.end() &&
                        It->second)
                {
                    texContext = It->second->GetName();
                    texState = texContext.empty()? CustomTextState::NoGlobal : CustomTextState::Global;
                }

                auto dragCollectorSampler = [&](){
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
                    {
                        std::string_view str = *static_cast<std::string_view*>(payload->Data);
                        if (auto *tex = GameEngine->GetAssetsManager().GetAsset<Texture>(str.data()); tex != nullptr)
                        {
                            if (tex->GetType() == TexType::Texture1D)
                            {
                                update = true;
                                textureAsset = str;
                                paramValue.valueType = Utils::ShaderValue::Sampler1D;
                                paramName = parameter.name;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                };

                if (ImGui::TextHandleButton(Utils::ParseUniformName(parameter.name).c_str(), texContext, "Sampler1D", texState, 16, dragCollectorSampler))
                    ImGui::OpenPopup("Sampler1DSelector");

                auto isSampler1DFunction = [](const AssetsHandle &handle) {
                    auto *tex = dynamic_cast<Texture*>(handle.get());
                    return tex != nullptr && tex->GetType() == TexType::Texture1D;
                };
                if (ImGui::AssetSelectorPopup("Sampler1DSelector", texContext, "Sampler1D", texState, textureAsset, isSampler1DFunction))
                {
                    update = true;
                    paramValue.valueType = Utils::ShaderValue::Sampler1D;
                    paramName = parameter.name;
                }
            }
                break;

            case Utils::ShaderValue::Sampler2D:
            {
                CustomTextState texState = CustomTextState::None;
                std::string texContext = "None";

                if (auto It = material.Samplers.find(parameter.name);
                        It != material.Samplers.end() &&
                        It->second)
                {
                    texContext = It->second->GetName();
                    texState = texContext.empty()? CustomTextState::NoGlobal : CustomTextState::Global;
                }

                auto dragCollectorSampler = [&](){
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
                    {
                        std::string_view str = *static_cast<std::string_view*>(payload->Data);
                        if (auto *tex = GameEngine->GetAssetsManager().GetAsset<Texture>(str.data()); tex != nullptr)
                        {
                            if (tex->GetType() == TexType::Texture2D)
                            {
                                update = true;
                                textureAsset = str;
                                paramValue.valueType = Utils::ShaderValue::Sampler2D;
                                paramName = parameter.name;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                };

                if (ImGui::TextHandleButton(Utils::ParseUniformName(parameter.name).c_str(), texContext, "Sampler2D", texState, 16, dragCollectorSampler))
                    ImGui::OpenPopup("Sampler2DSelector");

                auto isSampler2DFunction = [](const AssetsHandle &handle) {
                    auto *tex = dynamic_cast<Texture*>(handle.get());
                    return tex != nullptr && tex->GetType() == TexType::Texture2D;
                };
                if (ImGui::AssetSelectorPopup("Sampler2DSelector", texContext, "Sampler2D", texState, textureAsset, isSampler2DFunction))
                {
                    update = true;
                    paramValue.valueType = Utils::ShaderValue::Sampler2D;
                    paramName = parameter.name;
                }
            }
                break;

            case Utils::ShaderValue::SamplerCube:
            {
                CustomTextState texState = CustomTextState::None;
                std::string texContext = "None";

                if (auto It = material.Samplers.find(parameter.name);
                        It != material.Samplers.end() &&
                        It->second)
                {
                    texContext = It->second->GetName();
                    texState = texContext.empty()? CustomTextState::NoGlobal : CustomTextState::Global;
                }

                auto dragCollectorSampler = [&](){
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
                    {
                        std::string_view str = *static_cast<std::string_view*>(payload->Data);
                        if (auto *tex = GameEngine->GetAssetsManager().GetAsset<Texture>(str.data()); tex != nullptr)
                        {
                            if (tex->GetType() == TexType::TextureCube)
                            {
                                update = true;
                                textureAsset = str;
                                paramValue.valueType = Utils::ShaderValue::SamplerCube;
                                paramName = parameter.name;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                };

                if (ImGui::TextHandleButton(Utils::ParseUniformName(parameter.name).c_str(), texContext, "SamplerCube", texState, 16, dragCollectorSampler))
                    ImGui::OpenPopup("SamplerCubeSelector");

                auto isSamplerCubeFunction = [](const AssetsHandle &handle) {
                    auto *tex = dynamic_cast<Texture*>(handle.get());
                    return tex != nullptr && tex->GetType() == TexType::TextureCube;
                };
                if (ImGui::AssetSelectorPopup("SamplerCubeSelector", texContext, "SamplerCube", texState, textureAsset, isSamplerCubeFunction))
                {
                    update = true;
                    paramValue.valueType = Utils::ShaderValue::SamplerCube;
                    paramName = parameter.name;
                }
            }
                break;
            }
        }
    }
    else
        ImGui::Text("Warning! Shader not have parameters, maybe they are not used for objects");

    return update;
}

void MaterialAssetViewer::ApplyChanges(Material &material)
{
    if (!shaderStr.empty())
    {
        material.Shader = shaderStr;
        material.Uniforms.clear();
        material.Samplers.clear();
    }

    if (!paramName.empty())
    {
        if (!Utils::IsSampler(paramValue.valueType))
            material.Uniforms[paramName] = paramValue;
        else
        {
            if (!textureAsset.empty())
                material.Samplers[paramName] = GameEngine->GetAssetsManager().GetAsset(textureAsset);
            else if (material.Samplers.find(paramName) != material.Samplers.end())
                material.Samplers.erase(paramName);
        }
    }
}
