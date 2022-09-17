#include "stdafx.h"
#include "Object.h"

GameObject::GameObject()
	: Object()
	, mpTransform(nullptr)
{
	mpTransform = new Transform();
}

GameObject::~GameObject()
{
}
