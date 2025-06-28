#include <KMGActor.h>

KMGActor::KMGActor(std::wstring name) : name(name)
{

}

const std::vector<KMGVertex>& KMGActor::GetVertices()
{
    return vertices;
}

const std::vector<int>& KMGActor::GetIndexes()
{
    return indices;
}
