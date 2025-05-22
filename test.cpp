#include <iostream>
#include <d3d11.h>

using namespace std;

int main(int argc, char* argv[], char* envp[])
{
    // D3D11 디바이스와 디바이스 컨텍스트를 가리킬 포인터
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel;

    // 디바이스를 생성합니다.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // 기본 어댑터 사용
        D3D_DRIVER_TYPE_HARDWARE,  // 하드웨어 가속 사용
        nullptr,                    // 소프트웨어 렌더러 없음
        0,                          // 플래그 없음
        nullptr, 0,                 // 기능 레벨 기본값 사용
        D3D11_SDK_VERSION,          // SDK 버전 (고정)
        &device,                    // 디바이스 결과값
        &featureLevel,              // 선택된 기능 레벨
        &context                    // 디바이스 컨텍스트
    );

    if (SUCCEEDED(hr)) {
        cout << "DirectX 11 디바이스 생성 성공!" << endl;
    }
    else {
        cout << "DirectX 11 디바이스 생성 실패. HRESULT = " << hex << hr << endl;
    }

    // 리소스 해제
    if (context) context->Release();
    if (device) device->Release();

    return 0;
}
