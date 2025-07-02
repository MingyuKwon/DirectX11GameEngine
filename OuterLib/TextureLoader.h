#pragma once
#include <wincodec.h>  
#include <wrl/client.h>
#include <KMGDataStructure.h>

HRESULT CreateSrvFromTexture(
    ID3D11Device* device,
    const wchar_t* filename,
    ID3D11ShaderResourceView** textureView);