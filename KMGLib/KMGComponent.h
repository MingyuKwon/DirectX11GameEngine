#pragma once
#include <KMGDataStructure.h>

class KMGActor;

enum class EComponentType
{
	ECT_NONE,
	ECT_LIGHT,
	ECT_STATICMESH,
	ECT_MAX,

};

class KMGComponent {
public:
	KMGComponent() = default;
	inline KMGComponent(EComponentType componentType) : componentType (componentType){}

protected:
	KMGActor* owner = nullptr;

private:
	EComponentType componentType = EComponentType::ECT_NONE;
};

class LightComponent : public KMGComponent {
public:
	LightComponent() : KMGComponent(EComponentType::ECT_LIGHT){}
	Light light;

private:
};