#pragma once
#include <directxmath.h>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <mutex>


struct KMGVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT2 Tex;
};