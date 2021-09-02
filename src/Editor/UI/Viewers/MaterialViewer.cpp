#include "Editor/UI/Viewers/MaterialViewer.h"

#include "Editor/Editor.h"
#include "Systems/RenderSystem.h"
#include "Editor/ImGui/imgui_custom.h"
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

        if (ImGui::TextHandleButton("Material", context, "Material", state, 16))
            ImGui::OpenPopup("AssetSelector");

        std::string asset;
        auto isMaterialFunction = [](const AssetsHandle &handle) {
            return dynamic_cast<Material*>(handle.get()) != nullptr;
        };
        update = ImGui::AssetSelectorPopup("AssetSelector", context, "Material", state, asset, isMaterialFunction);

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

                            break;

                        case Utils::ShaderValue::UnsignedInt:

                            break;

                        case Utils::ShaderValue::Float:

                            break;

                        case Utils::ShaderValue::Double:

                            break;

                        case Utils::ShaderValue::Vector2:

                            break;

                        case Utils::ShaderValue::Vector3:

                            break;

                        case Utils::ShaderValue::Vector4:

                            break;

                        case Utils::ShaderValue::Mat2:

                            break;

                        case Utils::ShaderValue::Mat3:

                            break;

                        case Utils::ShaderValue::Mat4:

                            break;

                        case Utils::ShaderValue::Sampler1D:

                            break;

                        case Utils::ShaderValue::Sampler2D:

                            break;

                        case Utils::ShaderValue::Sampler3D:

                            break;

                        case Utils::ShaderValue::SamplerCube:

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
                //TODO: set uniform
            }
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<MaterialComponent>>(&go);
        GameEditor->CommandList.Redo();
    }
}
