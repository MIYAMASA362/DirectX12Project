#include "Transform.h"

Transform::Transform()
	: mLocalPosition(vector3::zero)
	, mLocalScale(vector3::one)
	, mLocalRotation(quaternion::identity)
{

}

Transform::~Transform()
{
}

XMMATRIX Transform::getWorldMatrix()
{
	XMMATRIX world = XMMatrixIdentity();
	world *= XMMatrixScalingFromVector(mLocalScale);
	world *= XMMatrixRotationQuaternion(mLocalRotation);
	world *= XMMatrixTranslationFromVector(mLocalPosition);
	return world;
}
