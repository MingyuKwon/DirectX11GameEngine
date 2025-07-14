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

#define FIRST_IMGUI_TAB(x)  ImGui::SetCursorPosX(x)
#define VERTICAL_SPACE(x)  ImGui::Dummy(ImVec2(0, x))

extern std::atomic<bool> bSceneFocused;
extern std::atomic<float> deltaTime;
extern float g_cameraMoveSpeed;

void KMGSceneWindow::DrawSceneWindow(
    KMGScene* currentScene,
    int mainWindowWidth, int mainWindowHeight,
    std::atomic<int>& sceneWindowWidth, std::atomic<int>& sceneWindowHeight,
    std::atomic<bool>& resizeRequested,
    ID3D11ShaderResourceView* pSceneSRV,
    DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix

)
{
    if (!currentScene) return;

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

    ImGui::Begin(SCENE_WINDOW_NAME,
        0,
        ImGuiWindowFlags_NoMove 
        );

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
        currentScene,
        sceneWindowWidth, sceneWindowHeight, 
        currentCameraViewMatrix, currentCameraProjectionMatrix
    );

    ShowFPS_CameraSpeed();
    ShowSceneSetting(currentScene);

    ImGui::End();
}

void KMGSceneWindow::CheckSceneClick(
    KMGScene* currentScene,
    int sceneWindowWidth, int sceneWindowHeight,
    DirectX::XMMATRIX currentCameraViewMatrix, DirectX::XMMATRIX currentCameraProjectionMatrix
)
{ 
    if (!currentScene) return;

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 texScreenPos = ImVec2(windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y);

    ImVec2 mousePos = ImGui::GetMousePos();
    static ImVec2 prevMousePos = mousePos;

    ImVec2 localClick = ImVec2(mousePos.x - texScreenPos.x, mousePos.y - texScreenPos.y);

    XMVECTOR rayDir = KMGUtility::GenerateCameraRayDirection(
        localClick.x, localClick.y,
        sceneWindowWidth, sceneWindowHeight,
        currentCameraViewMatrix, currentCameraProjectionMatrix);


    bool currentGrabbing = bSceneFocused && ImGui::IsMouseDown(0);
    currentScene->GrabAxis(rayDir, currentGrabbing);

    if (localClick.x >= 0 && localClick.y >= 0 &&
        localClick.x < sceneWindowWidth && localClick.y < sceneWindowHeight) {

        if (!currentGrabbing)
        {
            currentScene->CheckHoverAxis(rayDir);
        }
        

        if (bSceneFocused && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        {
            KMGCommand::CameraRayTrace_Select(rayDir);
        }

    }

    prevMousePos = mousePos;

}


void KMGSceneWindow::ShowSceneSetting(KMGScene* currentScene)
{
    if (!currentScene) return;

    struct SceneModeEntry {
        ESceneEditMode mode;
        const char* label;
    };

    SceneModeEntry modes[] = {
        { ESceneEditMode::ESEM_SELECT, "(Q) SELECT" },
        { ESceneEditMode::ESEM_MOVE,   "(W)   MOVE"   },
        { ESceneEditMode::ESEM_ROTATE, "(E) ROTATE" },
        { ESceneEditMode::ESEM_SCALE,  "(R)  SCALE"  }
    };

    ESceneEditMode currentMode = currentScene->GetSceneEditMode();

    ImGui::SetCursorPosY(35); 

    for (int i = 0; i < IM_ARRAYSIZE(modes); ++i)
    {
        ImGui::SetCursorPosX(15);

        const auto& entry = modes[i];

        if (entry.mode == currentMode)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.3f, 0.6f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.4f, 0.7f, 1.0f));
        }

        if (ImGui::Button(entry.label, ImVec2(80, 20))) {
            currentScene->SetSceneEditMode(entry.mode);
        }

        if (entry.mode == currentMode) {
            ImGui::PopStyleColor(2);
        }

    }

    VERTICAL_SPACE(5);
    ImGui::SetCursorPosX(15);
    
    static bool enabled = true;

    bool beforeEnable = enabled;

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(4, 0));
    ImGui::Checkbox("Show Gizmo", &enabled);
    ImGui::PopStyleVar(); 
    ImGui::PopStyleColor();

    if (beforeEnable != enabled) // 이러면 버튼을 눌러서 변한거다
    {
        currentScene->SetShowGizmo(enabled);
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

