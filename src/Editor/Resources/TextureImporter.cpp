#include "Editor/Resources/TextureImporter.h"

#include "imgui.h"
#include "Engine.h"
#include "stb_image.h"
#include "Editor/Editor.h"
#include "Render/Texture.h"
#include "Assets/AssetsManager.h"
#include "Editor/ImGui/ImFileDialog.h"

std::string imageFilter = "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.gif){.png,.jpg,.jpeg,.bmp,.tga,.gif},";

void TextureImporter::SetCurrentData(const fs::path &path, int width, int height, int comp)
{
    currentPath = path.string();
    assetName = path.stem().string();
    this->width = width;
    this->height = height;
    components = comp;

    wrapS = WrapType::Repeat;
    wrapT = WrapType::Repeat;
    wrapR = WrapType::Repeat;
    filtering = Filtering::Linear;
    mipmaps = true;
    cube = false;

    paths[0] = currentPath;
    for (u32 i = 1; i < 6; ++i)
        paths[i] = "";
}

void TextureImporter::ModalWindow(const std::string &name)
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Import texture window", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Path: %s", currentPath.c_str());
        ImGui::Text("Width:"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", width); ImGui::SameLine();
        ImGui::Text(", height:"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", height);
        ImGui::Text("Texture type:"); ImGui::SameLine();
        switch (components)
        {
        case 1:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Gray");
            break;
        case 2:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Gray Alpha");
            break;
        case 3:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "RGB");
            break;
        case 4:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "RGBA");
            break;
        }
        ImGui::Separator();

        ImGui::Checkbox("Cube", &cube);

        if (cube)
        {
            if (ImGui::Button("+X"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 0;
            }
            ImGui::SameLine();
            ImGui::Text(paths[0].c_str());

            if (ImGui::Button("-X"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 1;
            }
            ImGui::SameLine();
            ImGui::Text(paths[1].c_str());

            if (ImGui::Button("+Y"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 2;
            }
            ImGui::SameLine();
            ImGui::Text(paths[2].c_str());

            if (ImGui::Button("-Y"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 3;
            }
            ImGui::SameLine();
            ImGui::Text(paths[3].c_str());

            if (ImGui::Button("+Z"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 4;
            }
            ImGui::SameLine();
            ImGui::Text(paths[4].c_str());

            if (ImGui::Button("-Z"))
            {
                ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a texture", imageFilter + ".*");
                currentPos = 5;
            }
            ImGui::SameLine();
            ImGui::Text(paths[5].c_str());

            if (ifd::FileDialog::Instance().IsDone("AssetOpenDialog"))
            {
                if (ifd::FileDialog::Instance().HasResult())
                {
                    fs::path res = ifd::FileDialog::Instance().GetResult();
                    int x, y, comp;
                    if (stbi_info(res.string().c_str(), &x, &y, &comp) && width == x && height == y && components == comp)
                        paths[currentPos] = res.string();
                }
                ifd::FileDialog::Instance().Close();
            }
        }

        const char *wrapStr[] = { "Repeat", "MirroredRepeat", "ClampToEdge", "ClampToBorder", "None" };

        int wrapSCurrent = static_cast<int>(wrapS);
        ImGui::Combo("Wrap S", &wrapSCurrent, wrapStr, IM_ARRAYSIZE(wrapStr));
        wrapS = static_cast<WrapType>(wrapSCurrent);

        if (height != 1)
        {
            int wrapTCurrent = static_cast<int>(wrapT);
            ImGui::Combo("Wrap T", &wrapTCurrent, wrapStr, IM_ARRAYSIZE(wrapStr));
            wrapT = static_cast<WrapType>(wrapTCurrent);
        }

        if (cube)
        {
            int wrapRCurrent = static_cast<int>(wrapT);
            ImGui::Combo("Wrap R", &wrapRCurrent, wrapStr, IM_ARRAYSIZE(wrapStr));
            wrapT = static_cast<WrapType>(wrapRCurrent);
        }

        const char *filteringStr[] = { "Nearest", "Linear", "None" };
        int filteringCurrent = static_cast<int>(filtering);
        ImGui::Combo("Filtering", &filteringCurrent, filteringStr, IM_ARRAYSIZE(filteringStr));
        filtering = static_cast<Filtering>(filteringCurrent);

        ImGui::Checkbox("Mipmaps", &mipmaps);

        std::string label = "Asset name";
        bool isUniqName = GameEngine->GetAssetsManager().GetAsset(assetName) == nullptr;
        if (!isUniqName)
            label += "(Not uniq)";
        else
            label += "(Uniq)";

        char buffer[256] = {0};
        if(assetName.size() < 256)
            std::copy(assetName.begin(), assetName.end(), buffer);
        ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
        assetName = buffer;
        ImGui::Separator();

        if (ImGui::Button("Apply", ImVec2(150, 0)) && isUniqName)
        {
            AssetsHandle texture = std::make_shared<Texture>();
            auto *texturePtr = static_cast<Texture*>(texture.get());
            if (cube)
                texturePtr->SetType(TexType::TextureCube);
            else
                texturePtr->SetType(height == 1? TexType::Texture1D : TexType::Texture2D);
            texturePtr->SetFiltering(filtering);
            if (cube)
                texturePtr->SetWHD({width, height, 6});
            else
                texturePtr->SetWH({width, height});
            texturePtr->SetWrapS(wrapS);
            texturePtr->SetWrapT(wrapT);
            texturePtr->SetWrapR(wrapR);
            texturePtr->SetMipmap(mipmaps);
            switch (components)
            {
            case 1:
                texturePtr->SetTypeData(TexDataType::R);
                break;
            case 2:
                texturePtr->SetTypeData(TexDataType::RG);
                break;
            case 3:
                texturePtr->SetTypeData(TexDataType::RGB);
                break;
            case 4:
                texturePtr->SetTypeData(TexDataType::RGBA);
                break;
            }
            if (cube)
            {
                for (u32 i = 0; i < 6; ++i)
                {
                    int x, y, comp;
                    u8 *src = stbi_load(currentPath.c_str(), &x, &y, &comp, components);
                    texturePtr->SetSrcCubeTexture(src, i);
                }
            }
            else
            {
                int x, y, comp;
                u8 *src = stbi_load(currentPath.c_str(), &x, &y, &comp, components);
                texturePtr->SetSrc(src);
            }
            texturePtr->Apply();
            texturePtr->RenderUpdate();
            texturePtr->IsStatic = true;

            GameEngine->GetAssetsManager().AddAsset(assetName, texture);
            auto deleter = [](const AssetsHandle &handle){
                auto *texture = static_cast<Texture*>(handle.get());
                for (u32 i = 0; i < 6; ++i)
                {
                    if (texture->GetSrcCubeTexture(i) != nullptr)
                    {
                        stbi_image_free(texture->GetSrcCubeTexture(i));
                        texture->SetSrcCubeTexture(nullptr, i);
                    }
                }
            };
            GameEditor->GetAssetsWriter().AddAsset(texture, deleter);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(150, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
