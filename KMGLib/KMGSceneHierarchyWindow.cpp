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

wstring renameActorName;

void KMGSceneHierarchyWindow::SelectActor(std::wstring name)
{
    selectActorName = name;
}

bool KMGSceneHierarchyWindow::ShowContextItem(const char* str_id, KMGScene* currentScene, KMGActor* focusActor)
{
    if (ImGui::BeginPopupContextItem(str_id))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 1, 1, 1));
        ImGui::TextWrapped("%s", str_id); 
        ImGui::PopStyleColor();

        ImGui::Separator(); 
        ImGui::Spacing();   

        if (ImGui::MenuItem("Delete Actor")) {
            if (focusActor)
            {
                KMGCommand::RemoveActor(focusActor->GetName());
            }
        }

        if (ImGui::MenuItem("Rename Actor")) {
            renameActorName = focusActor->GetName();
            string str = KMGUtility::WStringToString(renameActorName);
            strncpy_s(renameBuffer, str.c_str(), sizeof(renameBuffer));
        }

        if (ImGui::MenuItem("Copy Actor")) {
            wstring copyActorName = focusActor->GetName();
            KMGCommand::CopyActor(copyActorName);
        }

        if (focusActor)
        {
            if (focusActor->IsVisible())
            {
                if (ImGui::MenuItem("Make InVisible")) {
                    KMGCommand::MakeVisibleActor(focusActor->GetName(), false);
                }
            }
            else
            {
                if (ImGui::MenuItem("Make Visible")) {
                    KMGCommand::MakeVisibleActor(focusActor->GetName(), true);
                }
            }
        }

        ImGui::EndPopup();

        return true;
    }

    return false;
}

void KMGSceneHierarchyWindow::ShowContextWindow(KMGScene* currentScene, bool bShow)
{
    if (bShow && ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Actor")) {
            KMGCommand::AddActor();
        }
        ImGui::EndPopup();
    }
}

void KMGSceneHierarchyWindow::DrawHierarchyWindow(
    int mainWindowWidth, int mainWindowHeight
    )
{
    ImGuiID id = ImGui::GetID(HIERARCHY_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowPosY = 0;


        int WindowWidth = mainWindowWidth * (1 - SCENE_DETAIL_WIDTH_RATIO);
        int WindowHeight = mainWindowHeight * HIERARCHY_DETAIL_HEIGHT_RATIO;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(HIERARCHY_WINDOW_NAME,
        0,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize
    );

    bHierarchyFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }


    if (currentScene)
    {
        bool bItemShowed = false;

        if (ImGui::BeginChild("ActorListRegion", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            KMGActor* focusActor = currentScene->GetFocusActor();
            const std::unordered_map<std::wstring, std::unique_ptr<KMGActor>>& actors = currentScene->getAllActors();


            for (const auto& bucket : actors)
            {
                const std::wstring& name = bucket.first;
                KMGActor* actor = bucket.second.get();

                bool isSelected = (focusActor && focusActor->GetName() == name);

                std::string focusActorStrName = KMGUtility::WStringToString(name);

                if (renameActorName == name)
                {
                    string inputTextId = "Enter to Change###";
                    inputTextId += focusActorStrName;

                    if (ImGui::InputText(inputTextId.c_str(), renameBuffer, IM_ARRAYSIZE(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        string resultStr(renameBuffer);
                        wstring w_resultStr = KMGUtility::StringToWString(resultStr);

                        if (currentScene)
                        {
                            std::unordered_set<std::wstring> actorNames = currentScene->GetActorNames();
                            if (actorNames.count(w_resultStr) == 0) // 만약 바꾸려는 이름이 이미 씬에 있어선 안된다
                            {
                                KMGCommand::RenameActor(name, w_resultStr);
                            }
                        }

                        renameActorName.clear();
                    }

                    if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) {
                        if (!ImGui::IsItemHovered()) {
                            renameActorName.clear(); 
                        }
                    }
                }
                else
                {
                    if (actor->IsVisible()) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1)); 
                    }
                    else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1)); 
                    }


                    if (ImGui::Selectable(focusActorStrName.c_str(), isSelected)) {
                        currentScene->SetFocusActor(name);
                    }

                    ImGui::PopStyleColor();

                }

                if (ShowContextItem(focusActorStrName.c_str(), currentScene, focusActor))
                {
                    bItemShowed = true;
                }


            }
        }

        ShowContextWindow(currentScene, !bItemShowed);

        ImGui::EndChild();
    }

    ImGui::End();

}


