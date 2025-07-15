#include <TextureLoader.h>  
#include <comdef.h> // _com_error 사용

using namespace Microsoft::WRL; 

void LogIfFailed(HRESULT hr, const wchar_t* context) {
    _com_error err(hr);
    std::wcout << L"[ERROR] " << context << L" failed: " << err.ErrorMessage() << std::endl;
}

HRESULT CreateSrvFromTexture(
    ID3D11Device* device,
    const wchar_t* filename,
    ID3D11ShaderResourceView** textureView)
{
    // 0. 전체 경로 출력
    wchar_t fullPath[MAX_PATH];
    DWORD pathLen = GetFullPathNameW(filename, MAX_PATH, fullPath, nullptr);
    if (pathLen > 0 && pathLen < MAX_PATH) {
        std::wcout << L"[Texture Load] Full Path: " << fullPath << std::endl;
    }
    else {
        std::wcout << L"[Texture Load] Failed to get full path for: " << filename << std::endl;
    }

    // 1. WIC 팩토리 생성
    ComPtr<IWICImagingFactory> wicFactory;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) {
        LogIfFailed(hr, L"CoCreateInstance(IWICImagingFactory)");
        return hr;
    }

    // 2. 이미지 디코더 생성
    ComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromFilename(
        filename, nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) {
        std::wcout << L"[ERROR] HRESULT: 0x" << std::hex << hr << std::endl;

        LogIfFailed(hr, L"CreateDecoderFromFilename");
        return hr;
    }

    // 3. 첫 번째 프레임 추출
    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) {
        LogIfFailed(hr, L"GetFrame(0)");
        return hr;
    }

    // 4. 32비트 RGBA 포맷으로 변환
    ComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) {
        LogIfFailed(hr, L"CreateFormatConverter");
        return hr;
    }

    hr = converter->Initialize(
        frame.Get(), GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.f,
        WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) {
        LogIfFailed(hr, L"FormatConverter::Initialize");
        return hr;
    }

    // 5. 이미지 크기 얻기
    UINT width, height;
    converter->GetSize(&width, &height);

    // 6. 픽셀 데이터 복사
    std::vector<BYTE> imageData(width * height * 4);
    hr = converter->CopyPixels(
        nullptr, width * 4,
        static_cast<UINT>(imageData.size()),
        imageData.data());
    if (FAILED(hr)) {
        LogIfFailed(hr, L"CopyPixels");
        return hr;
    }

    // 7. 텍스처 생성
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = imageData.data();
    initData.SysMemPitch = width * 4;

    ComPtr<ID3D11Texture2D> texture;
    hr = device->CreateTexture2D(&texDesc, &initData, &texture);
    if (FAILED(hr)) {
        LogIfFailed(hr, L"CreateTexture2D");
        return hr;
    }

    // 8. 셰이더 리소스 뷰 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView);
    if (FAILED(hr)) {
        LogIfFailed(hr, L"CreateShaderResourceView");
    }

    return hr;
}
