#pragma once
#include <KMGDataStructure.h>

//--------------------------------------------------------------------------------------
// GPU에 넘겨줄 구조체
//--------------------------------------------------------------------------------------

void RenderThread(
    std::vector<ID3D11CommandList*>& DX11CommandLists, std::mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice,
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangeOnPlayer,ID3D11Buffer* pCBChangesEveryFrame,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    ID3D11ShaderResourceView* pTextureSRV, ID3D11ShaderResourceView* pNormalMapSRV, ID3D11SamplerState* pSamplerState,
    int drawIndexCount,
    int textureWidth, int textureHeight);

