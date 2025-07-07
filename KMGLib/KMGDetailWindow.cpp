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

extern std::atomic<bool> bDetailFocused;

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

        static float position[3] = { 0.0f, 0.0f, 0.0f };
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        static float scale[3] = { 1.0f, 1.0f, 1.0f };

        float labelIndent = 25.0f;
        float controlOffsetX = 140.0f;

        style.ItemInnerSpacing.x = 15.0f;

        ImGui::PushItemWidth(200);

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Dummy(ImVec2(0, 3)); // 간격

            ImGui::SetCursorPosX(165);
            ImGui::Text("x");

            ImGui::SameLine(235);
            ImGui::Text("y");

            ImGui::SameLine(305);
            ImGui::Text("z");

            ImGui::Dummy(ImVec2(0, 3)); // 간격

            // Position
            ImGui::SetCursorPosX(labelIndent);
            ImGui::Text("Position");
            ImGui::SameLine(controlOffsetX);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.1f, 1.0f));
            ImGui::DragFloat3("##Position", position, 1.0f, 0.0f, 0.0f, "%.1f");
            ImGui::PopStyleColor();

            // Rotation
            ImGui::SetCursorPosX(labelIndent);
            ImGui::Text("Rotation");
            ImGui::SameLine(controlOffsetX);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.1f, 1.0f));
            ImGui::DragFloat3("##Rotation", rotation, 1.0f, 0.0f, 360.0f, "%.1f");
            ImGui::PopStyleColor();

            // Scale
            ImGui::SetCursorPosX(labelIndent);
            ImGui::Text("Scale");
            ImGui::SameLine(controlOffsetX);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.2f, 1.0f));
            ImGui::DragFloat3("##Scale", scale, 0.1f, 0.1f, 10.0f, "%.1f");
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(0, 10)); // 간격


        }

        if (ImGui::CollapsingHeader("Static Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

    }

    ImGui::PopItemWidth();

    ImGui::EndChild();
    ///////////////



    ImGui::End();
}
