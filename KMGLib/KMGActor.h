#pragma once
#include <KMGDataStructure.h>

class KMGActor {

public:
    std::wstring name;
    std::vector<KMGVertex> vertices;
    std::vector<int> indices;

    const std::vector<KMGVertex>& GetVertices();
    const std::vector<int>& GetIndexes();
private:
    
   

};