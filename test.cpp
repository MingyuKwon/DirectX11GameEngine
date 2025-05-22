#include <iostream>
#include <d3d11.h>

using namespace std;

int main(int argc, char* argv[], char* envp[])
{
    // D3D11 ����̽��� ����̽� ���ؽ�Ʈ�� ����ų ������
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel;

    // ����̽��� �����մϴ�.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // �⺻ ����� ���
        D3D_DRIVER_TYPE_HARDWARE,  // �ϵ���� ���� ���
        nullptr,                    // ����Ʈ���� ������ ����
        0,                          // �÷��� ����
        nullptr, 0,                 // ��� ���� �⺻�� ���
        D3D11_SDK_VERSION,          // SDK ���� (����)
        &device,                    // ����̽� �����
        &featureLevel,              // ���õ� ��� ����
        &context                    // ����̽� ���ؽ�Ʈ
    );

    if (SUCCEEDED(hr)) {
        cout << "DirectX 11 ����̽� ���� ����!" << endl;
    }
    else {
        cout << "DirectX 11 ����̽� ���� ����. HRESULT = " << hex << hr << endl;
    }

    // ���ҽ� ����
    if (context) context->Release();
    if (device) device->Release();

    return 0;
}
