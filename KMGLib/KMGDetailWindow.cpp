#include "KMGSceneHierarchyWindow.h"
#include <EngineData.h>
#include <iostream>
#include <KMGScene.h>
#include <KMGUtility.h>
#include <CommandSchedular.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "KMGDetailWindow.h"

using namespace DirectX;
using namespace std;

#define FIRST_IMGUI_TAB  ImGui::SetCursorPosX(25)
#define VERTICAL_SPACE(x)  ImGui::Dummy(ImVec2(0, x))

extern std::atomic<bool> bDetailFocused;

void DrawClippedPathText(const std::string& path, float maxWidth);


void KMGDetailWindow::DrawDetailWindow(
    int mainWindowWidth, int mainWindowHeight

)
{
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

    //////////////////

    if (ImGui::BeginChild("DetailWindowRegion", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemInnerSpacing.x = 15.0f;

        ImGui::PushItemWidth(200);

        ShowTransform();
        ShowStaticMesh();
        

    }

    ImGui::PopItemWidth();

    ImGui::EndChild();
    ///////////////



    ImGui::End();
}

void KMGDetailWindow::ShowTransform()
{
    static float position[3] = { 0.0f, 0.0f, 0.0f };
    static float rotation[3] = { 0.0f, 0.0f, 0.0f };
    static float scale[3] = { 1.0f, 1.0f, 1.0f };

    float controlOffsetX = 140.0f;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
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

        ImGui::Dummy(ImVec2(0, 10)); // АЃАн


    }
}

void KMGDetailWindow::ShowStaticMesh()
{
    if (ImGui::CollapsingHeader("Static Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static std::string meshPath = "mesh Path";
        static std::string texturePath = "texture Path";
        static std::string normalMapPath = "normalMap Path";

        float buttonOffsetY = (ImGui::GetTextLineHeight() - ImGui::GetFrameHeight()) * 0.5f;

        VERTICAL_SPACE(10);
        FIRST_IMGUI_TAB;
        ImGui::Text("Static Mesh :");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(meshPath, 150.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(315);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

        if (ImGui::Button("...##Mesh"))
        {
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        ImGui::Text("Texture:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(texturePath, 150.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(315);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);

        if (ImGui::Button("...##Texture"))
        {
        }

        VERTICAL_SPACE(5);
        FIRST_IMGUI_TAB;
        ImGui::Text("NormalMap:");
        ImGui::SameLine();
        ImGui::SetCursorPosX(150);
        DrawClippedPathText(normalMapPath, 150.0f);
        ImGui::SameLine();
        ImGui::SetCursorPosX(315);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);
        if (ImGui::Button("...##NormalMap"))
        {
        }
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

