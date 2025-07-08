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

#define FIRST_IMGUI_TAB  ImGui::SetCursorPosX(30)
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
        ShowTransform();
        ShowStaticMesh();
        ShowLight();

    }

    ImGui::PopItemWidth();

    ImGui::EndChild();

    ImGui::End();
}

void KMGDetailWindow::ShowName()
{
    VERTICAL_SPACE(5);
    FIRST_IMGUI_TAB;

    std::string actorName = "NONE";

    if (currenFocusActor)
    {
        actorName = KMGUtility::WStringToString(currenFocusActor->GetWstrName());
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
        FIRST_IMGUI_TAB;
        ImGui::Text("Position");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.1f, 1.0f));
        ImGui::DragFloat3("##Position", position, 1.0f, 0.0f, 0.0f, "%.1f");
        ImGui::PopStyleColor();

        // Rotation
        FIRST_IMGUI_TAB;
        ImGui::Text("Rotation");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.1f, 1.0f));
        ImGui::DragFloat3("##Rotation", rotation, 1.0f, 0.0f, 360.0f, "%.1f");
        ImGui::PopStyleColor();

        // Scale
        FIRST_IMGUI_TAB;
        ImGui::Text("Scale");
        ImGui::SameLine(controlOffsetX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.2f, 1.0f));
        ImGui::DragFloat3("##Scale", scale, 0.1f, 0.1f, 10.0f, "%.1f");
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
        FIRST_IMGUI_TAB;

        static bool enabled = false;

        StaticMeshComponent* staticComp = nullptr;
        if (currenFocusActor)
        {
            staticComp = currenFocusActor->GetComponent<StaticMeshComponent>(EComponentType::ECT_STATICMESH);
        }

        if (staticComp == nullptr)
        {
            enabled = false;
        }
        else
        {
            enabled = true;
        }

        bool beforeEnable = enabled;
        ImGui::Checkbox("Enable StaticMesh Component", &enabled);
        if (beforeEnable != enabled) // 이러면 버튼을 눌러서 변한거다
        {
            if (enabled)
            {
                // 이거면 static mesh를 추가하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::AddStaticMeshComponent(currenFocusActor->GetWstrName()));
            }
            else
            {
                // 이거면 static mesh를 제거 하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::RemoveStaticMeshComponent(currenFocusActor->GetWstrName()));
            }
        }

        VERTICAL_SPACE(10);

        if (!enabled) return;

        static std::string meshPath = "mesh Path";
        static std::string texturePath = "texture Path";
        static std::string normalMapPath = "normalMap Path";

        float buttonOffsetY = (ImGui::GetTextLineHeight() - ImGui::GetFrameHeight()) * 0.5f;

        FIRST_IMGUI_TAB;
        ImGui::Text("Static Mesh :");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(meshPath, 180.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(345);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

        if (ImGui::Button("...##Mesh"))
        {
            wstring fileName = KMGUtility::OpenFileDialog();
            wcout << fileName << "\n";
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        ImGui::Text("Texture:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(texturePath, 180.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(345);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

        if (ImGui::Button("...##Texture"))
        {
            wstring fileName = KMGUtility::OpenFileDialog();
            
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        ImGui::Text("NormalMap:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(normalMapPath, 180.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(345);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);
        if (ImGui::Button("...##NormalMap"))
        {
            wstring fileName = KMGUtility::OpenFileDialog();

        }

        VERTICAL_SPACE(10);

    }
}

void KMGDetailWindow::ShowLight()
{
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (currenFocusActor == nullptr) return;

        VERTICAL_SPACE(10);
        FIRST_IMGUI_TAB;

        static bool enabled = false;

        LightComponent* lightComp = nullptr;
        if (currenFocusActor)
        {
            lightComp = currenFocusActor->GetComponent<LightComponent>(EComponentType::ECT_LIGHT);
        }
           
        if (lightComp == nullptr)
        {
            enabled = false;
        }
        else
        {
            enabled = true;
        }
        
        bool beforeEnable = enabled;
        ImGui::Checkbox("Enable Light Component", &enabled);
        if (beforeEnable != enabled) // 이러면 버튼을 눌러서 변한거다
        {
            if (enabled)
            {
                // 이거면 light mesh를 추가하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::AddLightComponent(currenFocusActor->GetWstrName()));
            }
            else
            {
                // 이거면 light mesh를 제거 하라는 것
                if (schedular) schedular->PushCommand(KMGCommand::RemoveLightComponent(currenFocusActor->GetWstrName()));
            }
        }

        VERTICAL_SPACE(10);

        if (!enabled) return;

        FIRST_IMGUI_TAB;
        static int type = 0; // 0 = directional, 1 = point, 2 = spot
        ImGui::Text("Light Type");
        ImGui::SameLine(150);
        ImGui::Combo("##LightType", &type, "Directional\0Point\0Spot\0");

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        static float range = 100.0f;
        ImGui::Text("Range");
        ImGui::SameLine(150);
        ImGui::DragFloat("##LightRange", &range, 1.0f, 0.0f, 1000.0f);

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        static float intensity = 1.0f;
        ImGui::Text("Intensity");
        ImGui::SameLine(150);
        ImGui::DragFloat("##LightIntensity", &intensity, 0.01f, 0.0f, 100.0f);

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        static DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 0.7f, 1.0f };
        float colorArray[4] = { color.x, color.y, color.z, color.w };

        ImGui::Text("Color (RGBA)");
        ImGui::SameLine(150);
        if (ImGui::ColorEdit4("##LightColor", colorArray))
        {
            color = { colorArray[0], colorArray[1], colorArray[2], colorArray[3] };
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

