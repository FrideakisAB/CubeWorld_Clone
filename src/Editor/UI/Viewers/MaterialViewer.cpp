#include "Editor/UI/Viewers/MaterialViewer.h"

#include "Editor/Editor.h"
#include "Render/Texture.h"
#include "Systems/RenderSystem.h"
#include "Editor/ImGui/ImCustom.h"
#include "Components/MaterialComponent.h"
#include "Editor/Commands/ViewersCommands.h"

void MaterialViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &material = dynamic_cast<MaterialComponent&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Material component", &closed))
    {
        bool update = false;

        std::string context;
        CustomTextState state;

        if (!material.IsValid())
        {
            context = "(no select)";
            state = CustomTextState::None;
        }
        else
        {
            context = material.GetMaterial()->GetName();
            state = CustomTextState::Global;
            if (context.empty())
            {
                context = "(custom)";
                state = CustomTextState::NoGlobal;
            }
        }

        std::string asset;
        auto dragCollector = [&](){
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
            {
                std::string_view str = *static_cast<std::string_view*>(payload->Data);
                if (GameEngine->GetAssetsManager().GetAsset<Material>(str.data()) != nullptr)
                {
                    update = true;
                    asset = str;
                }
                ImGui::EndDragDropTarget();
            }
        };

        if (ImGui::TextHandleButton("Material", context, "Material", state, 16, dragCollector))
            ImGui::OpenPopup("MaterialSelector");

        auto isMaterialFunction = [](const AssetsHandle &handle) {
            return dynamic_cast<Material*>(handle.get()) != nullptr;
        };
        if (ImGui::AssetSelectorPopup("MaterialSelector", context, "Material", state, asset, isMaterialFunction))
            update = true;

        if (update)
        {
            GameEditor->CommandList.AddCommand<SetMaterial>(&go, asset);
            GameEditor->CommandList.Redo();
        }

        bool makeCustom = false;
        if (state != CustomTextState::None && state != CustomTextState::NoGlobal)
        {
            ImGui::Text("To edit the material, you need a local copy");
            if (ImGui::Button("Make as custom"))
                makeCustom = true;
        }
        else if (state == CustomTextState::None)
            if (ImGui::Button("Make custom"))
                makeCustom = true;

        if (makeCustom)
        {
            if (state != CustomTextState::None)
            {
                GameEditor->CommandList.AddCommand<SetRawMaterial>(&go, static_cast<Material*>(material.GetMaterial()->Clone()));
                GameEditor->CommandList.Redo();
            }
            else
            {
                GameEditor->CommandList.AddCommand<SetRawMaterial>(&go, new Material());
                GameEditor->CommandList.Redo();
            }
        }

        update = false;
        std::string shaderStr;
        std::string paramName;
        std::string textureAsset;
        Utils::ShaderParamValue paramValue;
        if (state != CustomTextState::None && state == CustomTextState::NoGlobal)
        {
            if (ImGui::TreeNode("Material editor"))
            {
                std::vector<const char*> shadersName;
                shadersName.reserve(GameEngine->GetRenderSystem().GetShaders().size());
                int itemCurrent;
                for (const auto &value : GameEngine->GetRenderSystem().GetShaders())
                {
                    if (material.IsValid() && value.first == material.GetMaterial()->Shader)
                        itemCurrent = shadersName.size();
                    shadersName.push_back(value.first.c_str());
                }

                ImGui::Combo("Shader", &itemCurrent, &shadersName[0], shadersName.size());
                if (material.GetMaterial()->Shader != shadersName[itemCurrent])
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                It != material.GetMaterial()->Uniforms.end() &&
                                It->second.valueType == parameter.valueType)
                            {
                                value = std::get<int>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                It != material.GetMaterial()->Uniforms.end() &&
                                It->second.valueType == parameter.valueType)
                            {
                                value = std::get<u32>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                It != material.GetMaterial()->Uniforms.end() &&
                                It->second.valueType == parameter.valueType)
                            {
                                value = std::get<f32>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                    It != material.GetMaterial()->Uniforms.end() &&
                                    It->second.valueType == parameter.valueType)
                            {
                                value = std::get<f64>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                    It != material.GetMaterial()->Uniforms.end() &&
                                    It->second.valueType == parameter.valueType)
                            {
                                value = std::get<glm::vec2>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                    It != material.GetMaterial()->Uniforms.end() &&
                                    It->second.valueType == parameter.valueType)
                            {
                                value = std::get<glm::vec3>(It->second.value);
                                oldValue = value;
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
                            if (auto It = material.GetMaterial()->Uniforms.find(parameter.name);
                                    It != material.GetMaterial()->Uniforms.end() &&
                                    It->second.valueType == parameter.valueType)
                            {
                                value = std::get<glm::vec4>(It->second.value);
                                oldValue = value;
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

                            if (auto It = material.GetMaterial()->Samplers.find(parameter.name);
                                    It != material.GetMaterial()->Samplers.end() &&
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

                            if (auto It = material.GetMaterial()->Samplers.find(parameter.name);
                                    It != material.GetMaterial()->Samplers.end() &&
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

                            if (auto It = material.GetMaterial()->Samplers.find(parameter.name);
                                    It != material.GetMaterial()->Samplers.end() &&
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

                ImGui::TreePop();
            }
        }

        if (update)
        {
            if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<MaterialComponent>>(&go);

            if (!shaderStr.empty())
            {
                material.GetMaterial()->Shader = shaderStr;
                material.GetMaterial()->Uniforms.clear();
                material.GetMaterial()->Samplers.clear();
            }

            if (!paramName.empty())
            {
                if (!Utils::IsSampler(paramValue.valueType))
                    material.GetMaterial()->Uniforms[paramName] = paramValue;
                else
                {
                    if (!textureAsset.empty())
                        material.GetMaterial()->Samplers[paramName] = GameEngine->GetAssetsManager().GetAsset(textureAsset);
                    else if (material.GetMaterial()->Samplers.find(paramName) != material.GetMaterial()->Samplers.end())
                        material.GetMaterial()->Samplers.erase(paramName);
                }
            }
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<MaterialComponent>>(&go);
        GameEditor->CommandList.Redo();
    }
}
