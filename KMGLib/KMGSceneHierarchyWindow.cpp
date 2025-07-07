#include "KMGSceneHierarchyWindow.h"
#include <EngineData.h>
#include <iostream>
#include <KMGScene.h>
#include <KMGUtility.h>
#include <CommandSchedular.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace DirectX;
using namespace std;

extern std::atomic<bool> bHierarchyFocused;
extern KMGScene* currentScene;

void KMGSceneHierarchyWindow::SelectActor(std::wstring name)
{
    selectActorName = name;

}

void KMGSceneHierarchyWindow::DrawHierarchyWindow(
    int mainWindowWidth, int mainWindowHeight
    )
{
    ImGuiID id = ImGui::GetID(CONTENT_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = 0;


        int WindowWidth = mainWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = mainWindowHeight * HIERARCHY_DETAIL_HEIGHT_RATIO;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(CONTENT_WINDOW_NAME);

    bHierarchyFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }


    if (currentScene)
    {
        if (ImGui::BeginChild("ActorListRegion", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            KMGActor* focusActor = currentScene->GetFocusActor();
            std::unordered_set<std::wstring>& actors = currentScene->GetActorNames();
            for (const std::wstring& name : actors)
            {
                bool isSelected = (focusActor && focusActor->GetWstrName() == name);

                string foucsActorStrName = KMGUtility::WStringToString(name);
                if (ImGui::Selectable(foucsActorStrName.c_str(), isSelected)) {
                    currentScene->SetFocusActor(name);
                }
                
            }
        }
        ImGui::EndChild(); 
    }

    ImGui::End();

}
