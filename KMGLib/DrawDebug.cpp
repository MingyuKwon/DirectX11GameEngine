#include <DrawDebug.h>
#include <CommandSchedular.h>

using namespace DirectX;

extern CommandSchedular* schedular;

void DrawDebug::DrawLine(std::wstring name, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color, float time)
{
    KMGDebugMesh mesh = MakeDebugLine(start, end, color);

    schedular->PushCommand(std::make_unique<SceneCommand_DrawDebug>(name, mesh, time));

}

void DrawDebug::DrawSphere(std::wstring name, DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color, float time)
{
    KMGDebugMesh mesh = MakeDebugSphere(origin, radius, color);

    schedular->PushCommand(std::make_unique<SceneCommand_DrawDebug>(name, mesh, time));

}


void DrawDebug::DrawBoundingBox(std::wstring name, const DirectX::BoundingBox box, DirectX::XMFLOAT4 color)
{
    KMGDebugMesh mesh = MakeDebugBoundingBox(box, color);

    schedular->PushCommand(std::make_unique<SceneCommand_DrawDebug>(name, mesh));

}

void DrawDebug::DrawRay(std::wstring name, DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color)
{
    KMGDebugMesh mesh = MakeDebugRay(origin, direction, length, color);

    schedular->PushCommand(std::make_unique<SceneCommand_DrawDebug>(name, mesh));

}

void DrawDebug::DrawAxes(std::wstring name, DirectX::XMMATRIX worldMatrix, float axisLength)
{
    KMGDebugMesh mesh = MakeDebugAxes(worldMatrix, axisLength);

    schedular->PushCommand(std::make_unique<SceneCommand_DrawDebug>(name, mesh));
}



KMGDebugMesh DrawDebug::MakeDebugLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
    KMGDebugMesh mesh;

    KMGVertex v0, v1;
    v0.Pos = start;
    v1.Pos = end;
    v0.Color = v1.Color = color;

    mesh.vertices.push_back(v0);
    mesh.vertices.push_back(v1);

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);

    return mesh;
}

KMGDebugMesh DrawDebug::MakeDebugSphere(DirectX::XMFLOAT3 origin, float radius, DirectX::XMFLOAT4 color)
{
    KMGDebugMesh mesh;

    const int slices = 12;  // 경도 방향 분할
    const int stacks = 6;   // 위도 방향 분할

    auto AddLine = [&](DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
        {
            int baseIdx = static_cast<int>(mesh.vertices.size());

            KMGVertex v0, v1;
            v0.Pos = a;
            v1.Pos = b;
            v0.Color = v1.Color = color;

            mesh.vertices.push_back(v0);
            mesh.vertices.push_back(v1);

            mesh.indices.push_back(baseIdx);
            mesh.indices.push_back(baseIdx + 1);
        };

    // 위도(θ), 경도(φ) 순회
    for (int i = 0; i <= stacks; ++i)
    {
        float theta1 = DirectX::XM_PI * i / stacks;
        float theta2 = DirectX::XM_PI * (i + 1) / stacks;

        for (int j = 0; j < slices; ++j)
        {
            float phi1 = DirectX::XM_2PI * j / slices;
            float phi2 = DirectX::XM_2PI * (j + 1) / slices;

            // 구면 좌표계 → 데카르트 좌표계
            auto sphericalToCartesian = [&](float theta, float phi) -> DirectX::XMFLOAT3 {
                float x = radius * sinf(theta) * cosf(phi) + origin.x;
                float y = radius * cosf(theta) + origin.y;
                float z = radius * sinf(theta) * sinf(phi) + origin.z;
                return DirectX::XMFLOAT3(x, y, z);
                };

            // 위도선 (horizontal)
            AddLine(
                sphericalToCartesian(theta1, phi1),
                sphericalToCartesian(theta1, phi2)
            );

            // 경도선 (vertical)
            if (i < stacks)
            {
                AddLine(
                    sphericalToCartesian(theta1, phi1),
                    sphericalToCartesian(theta2, phi1)
                );
            }
        }
    }

    return mesh;
}

KMGDebugMesh DrawDebug::MakeDebugBoundingBox(const DirectX::BoundingBox box, DirectX::XMFLOAT4 color)
{
    KMGDebugMesh mesh;
    XMFLOAT3 corners[8];
    box.GetCorners(corners);

    const int indices[] = {
        0,1, 1,2, 2,3, 3,0, // bottom 
        4,5, 5,6, 6,7, 7,4, // top 
        0,4, 1,5, 2,6, 3,7  // vertical 
    };

    for (int i = 0; i < 8; ++i) {
        KMGVertex v;
        v.Pos = corners[i];
        v.Color = color;
        mesh.vertices.push_back(v);
    }

    for (int i = 0; i < 24; ++i) {
        mesh.indices.push_back(indices[i]);
    }

    return mesh;
}

KMGDebugMesh DrawDebug::MakeDebugRay(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, float length, DirectX::XMFLOAT4 color)
{

    XMVECTOR dir = XMLoadFloat3(&direction);
    dir = XMVector3Normalize(dir) * length;

    XMVECTOR end = XMLoadFloat3(&origin) + dir;

    XMFLOAT3 endPos;
    XMStoreFloat3(&endPos, end);

    return MakeDebugLine(origin, endPos, color);

}

KMGDebugMesh DrawDebug::MakeDebugAxes(DirectX::XMMATRIX worldMatrix, float axisLength)
{
    KMGDebugMesh mesh;

    XMVECTOR origin = XMVector3TransformCoord(XMVectorZero(), worldMatrix);

    XMVECTOR xAxis = XMVector3TransformCoord(XMVectorSet(axisLength, 0, 0, 0), worldMatrix);
    XMVECTOR yAxis = XMVector3TransformCoord(XMVectorSet(0, axisLength, 0, 0), worldMatrix);
    XMVECTOR zAxis = XMVector3TransformCoord(XMVectorSet(0, 0, axisLength, 0), worldMatrix);

    XMFLOAT3 o, x, y, z;
    XMStoreFloat3(&o, origin);
    XMStoreFloat3(&x, xAxis);
    XMStoreFloat3(&y, yAxis);
    XMStoreFloat3(&z, zAxis);

    KMGDebugMesh xMesh = MakeDebugLine(o, x, XMFLOAT4(1, 0, 0, 1));
    KMGDebugMesh yMesh = MakeDebugLine(o, y, XMFLOAT4(0, 1, 0, 1));
    KMGDebugMesh zMesh = MakeDebugLine(o, z, XMFLOAT4(0, 0, 1, 1));

    int offset = 0;

    for (auto& v : xMesh.vertices) mesh.vertices.push_back(v);
    for (auto& i : xMesh.indices) mesh.indices.push_back(i + offset);
    offset += xMesh.vertices.size();

    for (auto& v : yMesh.vertices) mesh.vertices.push_back(v);
    for (auto& i : yMesh.indices) mesh.indices.push_back(i + offset);
    offset += yMesh.vertices.size();

    for (auto& v : zMesh.vertices) mesh.vertices.push_back(v);
    for (auto& i : zMesh.indices) mesh.indices.push_back(i + offset);

    return mesh;
}
