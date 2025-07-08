#include "KMGSceneHierarchyWindow.h"
#include <EngineData.h>
#include <iostream>
#include <KMGScene.h>
#include <KMGUtility.h>
#include <CommandSchedular.h>

#include <KMGActor.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "KMGDetailWindow.h"

using namespace DirectX;
using namespace std;

#define FIRST_IMGUI_TAB(x)  ImGui::SetCursorPosX(x)
#define VERTICAL_SPACE(x)  ImGui::Dummy(ImVec2(0, x))

extern std::atomic<bool> bDetailFocused;
extern CommandSchedular* schedular;

void DrawClippedPathText(const std::string& path, float maxWidth);

void KMGDetailWindow::DrawDetailWindow(
    KMGActor* focusActor,
    int mainWindowWidth, int mainWindowHeight
)
{
    currenFocusActor = focusActor;

    ImGuiID id = ImGui::GetID(DETAIL_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = mainWindowHeight * HIERARCHY_DETAIL_HEIGHT_RATIO;

        int WindowWidth = mainWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = mainWindowHeight * (1 - HIERARCHY_DETAIL_HEIGHT_RATIO);

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(DETAIL_WINDOW_NAME,
        0,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize
        );

    bDetailFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    if (ImGui::BeginChild("DetailWindowRegion", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemInnerSpacing.x = 15.0f;

        ImGui::PushItemWidth(200);

        
        ShowName();
        VERTICAL_SPACE(5);
        ShowTransform();
        VERTICAL_SPACE(5);

        ShowStaticMesh();
        VERTICAL_SPACE(5);

        ShowLight();

    }

    ImGui::PopItemWidth();

    ImGui::EndChild();

    ImGui::End();
}

void KMGDetailWindow::ShowName()
{
    VERTICAL_SPACE(5);
    FIRST_IMGUI_TAB(30);

    std::string actorName = "NONE";

    if (currenFocusActor)
    {
        actorName = KMGUtility::WStringToString(currenFocusActor->GetName());
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 textSize = ImGui::CalcTextSize(actorName.c_str());
    ImVec2 padding = ImVec2(6.0f, 4.0f);

    ImGui::GetWindowDrawList()->AddRectFilled(
        pos,
        ImVec2(pos.x + textSize.x + padding.x * 2, pos.y + textSize.y + padding.y * 2),
        IM_COL32(40, 100, 160, 200), // 파란 배경
        4.0f
    );

    float fontHeight = ImGui::GetFontSize();
    float verticalFix = (textSize.y - fontHeight) * 0.5f;
    ImGui::SetCursorScreenPos(ImVec2(pos.x + padding.x, pos.y + padding.y - verticalFix));

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.9f, 1.0f), actorName.c_str());
    VERTICAL_SPACE(10);
}

void KMGDetailWindow::ShowTransform()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float controlOffsetX = 140.0f;

        static float position[3] = { 0.0f, 0.0f, 0.0f };
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        static float scale[3] = { 1.0f, 1.0f, 1.0f };


        if (currenFocusActor == nullptr) return;

        XMVECTOR positionVec = currenFocusActor->GetPosition();
        XMVECTOR rotationVec = currenFocusActor->GetRotation();
        XMVECTOR scaleVec = currenFocusActor->GetScale();

        position[0] = XMVectorGetX(positionVec);
        position[1] = XMVectorGetY(positionVec);
        position[2] = XMVectorGetZ(positionVec);

        rotation[0] = XMConvertToDegrees(XMVectorGetX(rotationVec));
        rotation[1] = XMConvertToDegrees(XMVectorGetY(rotationVec));
        rotation[2] = XMConvertToDegrees(XMVectorGetZ(rotationVec));

        scale[0] = XMVectorGetX(scaleVec);
        scale[1] = XMVectorGetY(scaleVec);
        scale[2] = XMVectorGetZ(scaleVec);


        VERTICAL_SPACE(3);
        ImGui::SetCursorPosX(165);
        ImGui::Text("x");

        ImGui::SameLine(235);
        ImGui::Text("y");

        ImGui::SameLine(305);
        ImGui::Text("z");
        VERTICAL_SPACE(3);

        // Position
        FIRST_IMGUI_TAB(30);
        ImGui::Text("Position");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.1f, 1.0f));
        if (ImGui::DragFloat3("##Position", position, 0.2f, 0.0f, 0.0f, "%.1f"))
        {
            XMVECTOR changedPosition = XMVectorSet(position[0], position[1], position[2], 1);
            if (schedular) schedular->PushCommand(KMGCommand::UpdateActorPosition(currenFocusActor->GetName(), changedPosition));
        }
        ImGui::PopStyleColor();

        // Rotation
        FIRST_IMGUI_TAB(30);
        ImGui::Text("Rotation");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.1f, 1.0f));
        
        if (ImGui::DragFloat3("##Rotation", rotation, 1.0f, 0.0f, 360.0f, "%.1f"))
        {
            XMVECTOR changedRotation = XMVectorSet(rotation[0], rotation[1], rotation[2], 0);
            if (schedular) schedular->PushCommand(KMGCommand::UpdateActorPosition(currenFocusActor->GetName(), changedRotation));
        }

        ImGui::PopStyleColor();

        // Scale
        FIRST_IMGUI_TAB(30);
        ImGui::Text("Scale");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.2f, 1.0f));
        if (ImGui::DragFloat3("##Scale", scale, 0.1f, 0.1f, 10.0f, "%.1f"))
        {
            XMVECTOR changedScale = XMVectorSet(scale[0], scale[1], scale[2], 0);
            if (schedular) schedular->PushCommand(KMGCommand::UpdateActorScale(currenFocusActor->GetName(), changedScale));

        }
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, 10)); // 간격


    }
}

void KMGDetailWindow::ShowStaticMesh()
{
    if (ImGui::CollapsingHeader("Static Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (currenFocusActor == nullptr) return;

        VERTICAL_SPACE(10);
        FIRST_IMGUI_TAB(30);

        static bool enabled = false;

        StaticMeshComponent* staticComp = nullptr;
        if (currenFocusActor)
        {
            staticComp = currenFocusActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        }

        enabled = staticComp != nullptr;

        bool beforeEnable = enabled;
        ImGui::Checkbox("Enable StaticMesh Component", &enabled);
        if (beforeEnable != enabled) // 이러면 버튼을 눌러서 변한거다
        {
            if (enabled)
            {
                // 이거면 static mesh를 추가하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::AddStaticMeshComponent(currenFocusActor->GetName()));
            }
            else
            {
                // 이거면 static mesh를 제거 하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::RemoveStaticMeshComponent(currenFocusActor->GetName()));
            }
        }

        VERTICAL_SPACE(20);

        if (!enabled) return;

        staticComp = currenFocusActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        if (staticComp == nullptr) return;

        std::vector<KMGStaticMesh>* meshes = staticComp->GetMeshes();

        static std::string meshPath = "mesh Path";
        float buttonOffsetY = (ImGui::GetTextLineHeight() - ImGui::GetFrameHeight()) * 0.5f;

        FIRST_IMGUI_TAB(30);
        ImGui::Text("Static Mesh :");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);

        meshPath = staticComp->meshFileName;
        DrawClippedPathText(meshPath, 220.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(385);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

        if (ImGui::Button("...##Mesh"))
        {
            wstring fileName = KMGUtility::OpenFileDialog();
            wcout << fileName << L"\n";

        }
        VERTICAL_SPACE(5);


        ImGui::Indent(15.0f);

        ShowTexture_Normal(meshes);

        ImGui::Indent(-15.0f);


        VERTICAL_SPACE(10);

    }
}

void KMGDetailWindow::ShowTexture_Normal(std::vector<KMGStaticMesh>* meshes)
{
    float buttonOffsetY = (ImGui::GetTextLineHeight() - ImGui::GetFrameHeight()) * 0.5f;

    unordered_set<wstring> textureVecs;
    unordered_set<wstring> normalVecs;

    for (const KMGStaticMesh& mesh : *meshes)
    {
        textureVecs.insert(mesh.textureFilePath);
        normalVecs.insert(mesh.normalMapFilePath);
    }

    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        int i = 1;
        for (const wstring& data : textureVecs)
        {
            VERTICAL_SPACE(5);
            FIRST_IMGUI_TAB(45);
            string textureName = "Texture " + to_string(i) + " :";
            ImGui::Text(textureName.c_str());
            ImGui::SameLine();
            ImGui::SetCursorPosX(150);
            std::string texturePath = KMGUtility::WStringToString(data);
            DrawClippedPathText(texturePath, 220.0f);
            ImGui::SameLine();
            ImGui::SetCursorPosX(385);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

            if (ImGui::Button("...##Texture"))
            {
                wstring fileName = GetTextureFromFileExplorer();
                if (fileName != L"NONE")
                {
                    wchar_t fullPath[MAX_PATH];
                    DWORD len = GetFullPathNameW(fileName.c_str(), MAX_PATH, fullPath, nullptr);
                    if (len > 0) {
                        std::wcout << L"[Debug] Input Path name: " << fileName << "\n";
                        std::wcout << L"[Debug] Full Path: " << fullPath << std::endl;
                    }

                    if (schedular) schedular->PushCommand(KMGCommand::UpdateTexture(currenFocusActor->GetName(), data, fileName));

                }

            }
            VERTICAL_SPACE(5);

            ++i;
        }
    }

    if (ImGui::CollapsingHeader("NormalMaps", ImGuiTreeNodeFlags_DefaultOpen))
    {
        int i = 1;
        for (const wstring& data : normalVecs)
        {
            VERTICAL_SPACE(5);
            FIRST_IMGUI_TAB(45);
            string textureName = "NormalMap " + to_string(i) + " :";

            ImGui::Text(textureName.c_str());
            ImGui::SameLine();
            ImGui::SetCursorPosX(150);
            std::string texturePath = KMGUtility::WStringToString(data);
            DrawClippedPathText(texturePath, 220.0f);
            ImGui::SameLine();
            ImGui::SetCursorPosX(385);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

            if (ImGui::Button("...##NormalMapj"))
            {
                wstring fileName = GetTextureFromFileExplorer();
                if (fileName != L"NONE")
                {
                    if (schedular) schedular->PushCommand(KMGCommand::UpdateNormalMap(currenFocusActor->GetName(), data, fileName));

                }

            }
            VERTICAL_SPACE(5);

            ++i;
        }

    }
}

wstring KMGDetailWindow::GetTextureFromFileExplorer()
{
    wstring fileName = KMGUtility::OpenFileDialog();

    const std::vector<std::wstring> validExtensions = {
        L".png", L".jpg", L".jpeg", L".dds", L".tga", L".bmp", L".gif"
    };

    size_t dotPos = fileName.find_last_of(L'.');
    if (dotPos == std::wstring::npos)
        return L"NONE";

    std::wstring ext = fileName.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

    if (std::find(validExtensions.begin(), validExtensions.end(), ext) == validExtensions.end())
        return L"NONE";

    return fileName;
}


void KMGDetailWindow::ShowLight()
{
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (currenFocusActor == nullptr) return;

        VERTICAL_SPACE(10);
        FIRST_IMGUI_TAB(30);

        static bool enabled = false;

        LightComponent* lightComp = nullptr;
        if (currenFocusActor)
        {
            lightComp = currenFocusActor->GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
        }
         
        enabled = lightComp != nullptr;
        
        bool beforeEnable = enabled;
        ImGui::Checkbox("Enable Light Component", &enabled);
        if (beforeEnable != enabled) // 이러면 버튼을 눌러서 변한거다
        {
            if (enabled)
            {
                // 이거면 light mesh를 추가하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::AddLightComponent(currenFocusActor->GetName()));
            }
            else
            {
                // 이거면 light mesh를 제거 하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::RemoveLightComponent(currenFocusActor->GetName()));
            }
        }

        VERTICAL_SPACE(20);

        if (!enabled) return;

        lightComp = currenFocusActor->GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
        if (lightComp == nullptr) return;

        Light& light = lightComp->GetLight();

        FIRST_IMGUI_TAB(30);
        int type = light.type;
        ImGui::Text("Light Type");
        ImGui::SameLine(150);
        if (ImGui::Combo("##LightType", &type, "Directional\0Point\0Spot\0"))
        {
            if (schedular) schedular->PushCommand(KMGCommand::UpdateLightComponent_Type(currenFocusActor->GetName(), type));
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB(30);
        float range = light.range;
        ImGui::Text("Range");
        ImGui::SameLine(150);
        if (ImGui::DragFloat("##LightRange", &range, 1.0f, 0.0f, 1000.0f))
        {
            if (schedular) schedular->PushCommand(KMGCommand::UpdateLightComponent_Range(currenFocusActor->GetName(), range));
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB(30);
        float intensity = light.intensity;
        ImGui::Text("Intensity");
        ImGui::SameLine(150);
        if (ImGui::DragFloat("##LightIntensity", &intensity, 0.01f, 0.0f, 100.0f))
        {
            if (schedular) schedular->PushCommand(KMGCommand::UpdateLightComponent_Intensity(currenFocusActor->GetName(), intensity));
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB(30);
        DirectX::XMFLOAT4 color = light.color;
        float colorArray[4] = { color.x, color.y, color.z, color.w };

        ImGui::Text("Color (RGBA)");
        ImGui::SameLine(150);
        if (ImGui::ColorEdit4("##LightColor", colorArray))
        {
            if (schedular) schedular->PushCommand(KMGCommand::UpdateLightComponent_Color(currenFocusActor->GetName(), { colorArray[0], colorArray[1], colorArray[2], colorArray[3] }));
        }

        VERTICAL_SPACE(10);

    }
}

void DrawClippedPathText(const std::string& path, float maxWidth)
{
    const float paddingX = 6.0f;
    const float paddingY = 4.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 bgMin = ImVec2(cursorPos.x - paddingX, cursorPos.y - paddingY);
    ImVec2 bgMax = ImVec2(cursorPos.x + maxWidth + paddingX, cursorPos.y + ImGui::GetTextLineHeight() + paddingY);

    ImU32 bgColor = IM_COL32(0, 102, 204, 100);
    ImGui::GetWindowDrawList()->AddRectFilled(bgMin, bgMax, bgColor, 4.0f);

    ImVec2 clipMin = cursorPos;
    ImVec2 clipMax = ImVec2(cursorPos.x + maxWidth, cursorPos.y + ImGui::GetTextLineHeight());
    ImGui::PushClipRect(clipMin, clipMax, true);

    std::string displayedPath = path;
    if (ImGui::CalcTextSize(displayedPath.c_str()).x > maxWidth) {
        while (!displayedPath.empty() &&
            ImGui::CalcTextSize((displayedPath + "...").c_str()).x > maxWidth) {
            displayedPath.pop_back();
        }
        displayedPath += "...";
    }

    ImGui::GetWindowDrawList()->AddText(cursorPos, IM_COL32_WHITE, displayedPath.c_str());

    ImGui::PopClipRect();

}

