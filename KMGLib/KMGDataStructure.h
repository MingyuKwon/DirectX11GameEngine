#include <directxmath.h>
#include <vector>
#include <string>
#include <unordered_map>

struct KMGVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT2 Tex;
};