#include <TextureLoader.h>  

using namespace Microsoft::WRL; 

HRESULT CreateSrvFromTexture(
    ID3D11Device* device,
    const wchar_t* filename,
    ID3D11ShaderResourceView** textureView)
{
    // 1. WIC 팩토리 생성
    ComPtr<IWICImagingFactory> wicFactory;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    // 2. 이미지 디코더 생성
    ComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromFilename(
        filename, nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return hr;

    // 3. 첫 번째 프레임 추출
    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    // 4. 32비트 RGBA 포맷으로 변환
    ComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    hr = converter->Initialize(
        frame.Get(), GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.f,
        WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return hr;

    // 5. 이미지 크기 얻기
    UINT width, height;
    converter->GetSize(&width, &height);

    // 6. 픽셀 데이터 복사
    std::vector<BYTE> imageData(width * height * 4);
    hr = converter->CopyPixels(
        nullptr, width * 4,
        static_cast<UINT>(imageData.size()),
        imageData.data());
    if (FAILED(hr)) return hr;

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
    if (FAILED(hr)) return hr;

    // 8. 셰이더 리소스 뷰 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView);
    return hr;
}