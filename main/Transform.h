#ifndef __CORE_TRANSFORM_H__
#define __CORE_TRANSFORM_H__

#include "Math.h"

class Transform
{
public:
	Transform();
	virtual ~Transform();

	void setLocalPosition(Vector3 position) { mLocalPosition = position; }
	Vector3 getLocalPosition() const { return mLocalPosition; }

	void setLocalScale(Vector3 scale) { mLocalScale = scale; }
	Vector3 getLocalScale() const { return mLocalScale; }

	void setLocalRotation(Quaternion rotation) { mLocalRotation = rotation; }
	Quaternion getLocalRotation() const { return mLocalRotation; }

	XMMATRIX getWorldMatrix();

private:
	Vector3 mLocalPosition;
	Vector3 mLocalScale;
	Quaternion mLocalRotation;
};

#endif
