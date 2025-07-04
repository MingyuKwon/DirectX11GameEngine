#include <DirectX11Run.h>
#include <EngineData.h>
#include <iostream>
#include <KMGActor.h>

using namespace DirectX;
using namespace std;

void RenderThread(
    vector<ID3D11CommandList*>& DX11CommandLists, mutex& dx11CommandMutex,
    ID3D11Device* pMainDevice, 
    ID3D11VertexShader* pVertexShader,
    ID3D11PixelShader* pPixelShader,
    ID3D11InputLayout* pVertexLayout,
    ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV,
    ID3D11Buffer* pCBChangeOnResize, ID3D11Buffer* pCBChangeOnPlayer, ID3D11Buffer* pCBChangeOnActor, ID3D11Buffer* pCBLightArray,
    ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIdexBuffer,
    ID3D11ShaderResourceView* pTextureSRV, ID3D11ShaderResourceView* pNormalMapSRV, ID3D11SamplerState* pSamplerState,
    int drawIndexCount,
    int textureWidth, int textureHeight)
{

    ID3D11DeviceContext* pDeferredContext = nullptr;
    HRESULT hr = pMainDevice->CreateDeferredContext(0, &pDeferredContext);
    if (FAILED(hr)) {
        std::cout << "CreateDeferredContext failed: HRESULT = 0x"
            << std::hex << hr << std::endl;
    }

    pDeferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pDeferredContext->IASetInputLayout(pVertexLayout);
    pDeferredContext->VSSetShader(pVertexShader, nullptr, 0);
    pDeferredContext->PSSetShader(pPixelShader, nullptr, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(textureWidth);
    viewport.Height = static_cast<float>(textureHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    pDeferredContext->RSSetViewports(1, &viewport);

    pDeferredContext->OMSetRenderTargets(1, &pRTV, pDSV);

    pDeferredContext->VSSetConstantBuffers(0, 1, &pCBChangeOnActor);
    pDeferredContext->PSSetConstantBuffers(0, 1, &pCBChangeOnActor);

    pDeferredContext->VSSetConstantBuffers(1, 1, &pCBChangeOnPlayer);
    pDeferredContext->PSSetConstantBuffers(1, 1, &pCBChangeOnPlayer);

    pDeferredContext->VSSetConstantBuffers(2, 1, &pCBChangeOnResize);
    pDeferredContext->PSSetConstantBuffers(2, 1, &pCBChangeOnResize);

    pDeferredContext->PSSetConstantBuffers(3, 1, &pCBLightArray);

    UINT stride = sizeof(KMGVertex);
    UINT offset = 0;

    pDeferredContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pDeferredContext->IASetIndexBuffer(pIdexBuffer, DXGI_FORMAT_R32_UINT, 0);

    pDeferredContext->PSSetShaderResources(0, 1, &pTextureSRV);
    pDeferredContext->PSSetShaderResources(1, 1, &pNormalMapSRV);
    pDeferredContext->PSSetSamplers(0, 1, &pSamplerState);

    pDeferredContext->DrawIndexed(drawIndexCount, 0, 0);

    ID3D11CommandList* pCmdList = nullptr;
    pDeferredContext->FinishCommandList(FALSE, &pCmdList);

    {
        std::lock_guard<std::mutex> lock(dx11CommandMutex);
        DX11CommandLists.push_back(pCmdList);
    }

    if (pDeferredContext)
    {
        pDeferredContext->Release();
        pDeferredContext = nullptr;
    }
}