#include <KMGActor.h>

const std::vector<KMGVertex>& KMGActor::GetVertices()
{
    return vertices;
}

const std::vector<int>& KMGActor::GetIndexes()
{
    return indices;
}
