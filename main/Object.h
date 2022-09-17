#ifndef __OBJECT_IOBJECT_H__
#define __OBJECT_IOBJECT_H__

#include "Transform.h"

using namespace DirectX;

class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

	/* Behavior
	virtual void onInit() = 0;
	virtual void onUpdate() = 0;
	virtual void onDraw(struct ID3D12GraphicsCommandList* commandList) = 0;
	virtual void onDestroy() = 0;
	*/
};

class GameObject : public Object
{
public:
	GameObject();
	virtual ~GameObject();

	Transform* getTransform() const { return mpTransform; }

private:
	Transform* mpTransform;
};

#endif
