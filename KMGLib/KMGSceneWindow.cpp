#include "KMGSceneWindow.h"
#include <EngineData.h>
#include <KMGDataStructure.h>
#include <iostream>
#include <KMGScene.h>
#include <KMGUtility.h>
#include <CommandSchedular.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace DirectX;
using namespace std;


extern std::atomic<bool> bSceneFocused;
extern std::atomic<float> deltaTime;
extern CommandSchedular* schedular;
extern float g_cameraMoveSpeed;

void KMGSceneWindow::DrawSceneWindow(
    int mainWindowWidth, int mainWindowHeight,
    std::atomic<int>& sceneWindowWidth, std::atomic<int>& sceneWindowHeight,
    std::atomic<bool>& resizeRequested,
    ID3D11ShaderResourceView* pSceneSRV,
    DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix

)
{
    ImGuiID id = ImGui::GetID(SCENE_WINDOW_NAME);
    ImGuiStorage* storage = ImGui::GetStateStorage();
    if (!storage->GetBool(id)) {
        int WindowPosX = 0;
        int WindowPosY = 0;

        int WindowWidth = mainWindowWidth * SCENE_DETAIL_WIDTH_RATIO;
        int WindowHeight = mainWindowHeight;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    }

    ImGui::Begin(SCENE_WINDOW_NAME);

    bSceneFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_RootAndChildWindows);

    if (!storage->GetBool(id)) {
        storage->SetBool(id, true);
    }

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (sceneWindowWidth != contentSize.x || sceneWindowHeight != contentSize.y)
    {
        sceneWindowWidth.store(contentSize.x);
        sceneWindowHeight.store(contentSize.y);
        resizeRequested.store(true);
    }

    ImGui::Image(pSceneSRV, ImVec2(sceneWindowWidth, sceneWindowHeight));

    CheckSceneClick(
        sceneWindowWidth, sceneWindowHeight, 
        currentCameraViewMatrix, currentCameraProjectionMatrix
    );

    ShowFPS_CameraSpeed();

    ImGui::End();
}

void KMGSceneWindow::CheckSceneClick(int sceneWindowWidth, int sceneWindowHeight,
    DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
)
{
    // 여기에서 화면의 어느 지점을 플레이어가 눌렀는지를 확인한다
    if (bSceneFocused && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
        ImVec2 texScreenPos = ImVec2(windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y);

        ImVec2 mousePos = ImGui::GetMousePos();

        ImVec2 localClick = ImVec2(mousePos.x - texScreenPos.x, mousePos.y - texScreenPos.y);

        if (localClick.x >= 0 && localClick.y >= 0 &&
            localClick.x < sceneWindowWidth && localClick.y < sceneWindowHeight) {

            XMVECTOR rayDir = KMGUtility::GenerateCameraRayDirection(
                localClick.x, localClick.y,
                sceneWindowWidth, sceneWindowHeight,
                currentCameraViewMatrix, currentCameraProjectionMatrix);

            // 여기서 스케큘러에게 줘야 한다
            if (schedular)
            {
                schedular->PushCommand(KMGCommand::CameraRayTrace(rayDir));
            }
        }
    }
}


void KMGSceneWindow::ShowFPS_CameraSpeed()
{
    ImVec2 imagePos = ImGui::GetItemRectMin();
    ImVec2 imageSize = ImGui::GetItemRectSize();

    ImVec2 bgPadding = ImVec2(8, 4);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ///////////////////////////////
    // 이게 FPS 보여주는 텍스트 띄우기
    //////////////////////////////
    static float showDeltaTime = deltaTime;
    if (deltaTime > 0.000001f) showDeltaTime = deltaTime;

    int framePerSecond = static_cast<int>(1 / showDeltaTime);
    string fpsStr = "FPS : " + to_string(framePerSecond);
    const char* c_showStr = fpsStr.c_str();

    ImVec2 textSize = ImGui::CalcTextSize(c_showStr);

    ImVec2 textPos = ImVec2(
        imagePos.x + imageSize.x - 8,
        imagePos.y + 4
    );
    textPos.x -= textSize.x;

    drawList->AddRectFilled(
        ImVec2(textPos.x - bgPadding.x, textPos.y - bgPadding.y),
        ImVec2(textPos.x + textSize.x + bgPadding.x, textPos.y + textSize.y + bgPadding.y),
        IM_COL32(0, 0, 0, 220)
    );

    drawList->AddText(
        textPos,
        framePerSecond > 40 ? IM_COL32(29, 219, 22, 255) : IM_COL32(255, 0, 0, 255),
        c_showStr
    );

    textPos.x += textSize.x;
    textPos.y += (textSize.y + 2 * bgPadding.y);

    ///////////////////////////////
    // 이게 카메라 속도 보여주는 텍스트 띄우기
    //////////////////////////////

    int cameraSpeed = g_cameraMoveSpeed * 100;
    string cameraSpeedStr = "Camera Speed : " + to_string(cameraSpeed);
    c_showStr = cameraSpeedStr.c_str();
    textSize = ImGui::CalcTextSize(c_showStr);

    textPos.x -= textSize.x;

    drawList->AddRectFilled(
        ImVec2(textPos.x - bgPadding.x, textPos.y - bgPadding.y),
        ImVec2(textPos.x + textSize.x + bgPadding.x, textPos.y + textSize.y + bgPadding.y),
        IM_COL32(0, 20, 0, 220)
    );

    drawList->AddText(
        textPos,
        IM_COL32(255, 255, 255, 255),
        c_showStr
    );

    textPos.x += textSize.x;
    textPos.y += (textSize.y + bgPadding.y);
}

