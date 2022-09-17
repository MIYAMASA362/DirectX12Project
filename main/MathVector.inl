#pragma once

inline bool Vector4::operator==(const Vector4& vec)
{
	return vector4::Equal(*this, vec);
}

inline bool Vector4::operator!=(const Vector4& vec)
{
	return vector4::NotEqual(*this, vec);
}

inline Vector4 Vector4::operator+(const Vector4& vec)
{
	return vector4::Add(*this, vec);
}

inline Vector4 Vector4::operator-(const Vector4& vec)
{
	return vector4::Sub(*this, vec);
}

inline Vector4 Vector4::operator*(const float& s)
{
	return vector4::Multiply(*this, s);
}

inline Vector4 Vector4::operator*(const Vector4& vec)
{
	return vector4::Multiply(*this, vec);
}

inline Vector4 Vector4::operator*(const float& s) const
{
	return vector4::Multiply(*this, s);
}

inline Vector4 Vector4::operator*(const Vector4& vec) const
{
	return vector4::Multiply(*this, vec);
}

inline Vector4 Vector4::operator/(const float& s) const
{
	return vector4::Divide(*this, s);
}

inline Vector4 Vector4::operator/(const Vector4& vec) const
{
	return vector4::Divide(*this, vec);
}

inline Vector4 Vector4::operator/(const float& s)
{
	return vector4::Divide(*this, s);
}

inline Vector4 Vector4::operator/(const Vector4& vec)
{
	return vector4::Divide(*this, vec);
}

inline Vector4 Vector4::operator+(const Vector4& vec) const
{
	return vector4::Add(*this, vec);
}

inline Vector4 Vector4::operator-(const Vector4& vec) const
{
	return vector4::Sub(*this, vec);
}

inline Vector4 Vector4::operator+=(const Vector4& vec)
{
	return *this = vector4::Add(*this, vec);
}

inline Vector4 Vector4::operator-=(const Vector4& vec)
{
	return *this = vector4::Sub(*this, vec);
}

inline Vector4 Vector4::operator*=(const float& s)
{
	return *this = vector4::Multiply(*this, s);
}

inline Vector4 Vector4::operator*=(const Vector4& vec)
{
	return *this = vector4::Multiply(*this, vec);
}

inline Vector4 Vector4::operator/=(const float& s)
{
	return *this = vector4::Divide(*this, s);
}

inline Vector4 Vector4::operator/=(const Vector4& vec)
{
	return *this = vector4::Divide(*this, vec);
}



inline bool Vector3::operator==(const Vector3& vec)
{
	return vector3::Equal(*this, vec);
}

inline bool Vector3::operator!=(const Vector3& vec)
{
	return vector3::NotEqual(*this, vec);
}

inline Vector3 Vector3::operator+(const Vector3& vec)
{
	return vector3::Add(*this, vec);
}

inline Vector3 Vector3::operator-(const Vector3& vec)
{
	return vector3::Sub(*this, vec);
}

inline Vector3 Vector3::operator*(const float& s)
{
	return vector3::Multiply(*this, s);
}

inline Vector3 Vector3::operator*(const Vector3& vec)
{
	return vector3::Multiply(*this, vec);
}

inline Vector3 Vector3::operator*(const float& s) const
{
	return vector3::Multiply(*this, s);
}

inline Vector3 Vector3::operator*(const Vector3& vec) const
{
	return vector3::Multiply(*this, vec);
}

inline Vector3 Vector3::operator/(const float& s) const
{
	return vector3::Divide(*this, s);
}

inline Vector3 Vector3::operator/(const Vector3& vec) const
{
	return vector3::Divide(*this, vec);
}

inline Vector3 Vector3::operator/(const float& s)
{
	return vector3::Divide(*this, s);
}

inline Vector3 Vector3::operator/(const Vector3& vec)
{
	return vector3::Divide(*this, vec);
}

inline Vector3 Vector3::operator+(const Vector3& vec) const
{
	return vector3::Add(*this, vec);
}

inline Vector3 Vector3::operator-(const Vector3& vec) const
{
	return vector3::Sub(*this, vec);
}

inline Vector3 Vector3::operator+=(const Vector3& vec)
{
	return *this = vector3::Add(*this, vec);
}

inline Vector3 Vector3::operator-=(const Vector3& vec)
{
	return *this = vector3::Sub(*this, vec);
}

inline Vector3 Vector3::operator*=(const float& s)
{
	return *this = vector3::Multiply(*this, s);
}

inline Vector3 Vector3::operator*=(const Vector3& vec)
{
	return *this = vector3::Multiply(*this, vec);
}

inline Vector3 Vector3::operator/=(const float& s)
{
	return *this = vector3::Divide(*this, s);
}

inline Vector3 Vector3::operator/=(const Vector3& vec)
{
	return *this = vector3::Divide(*this, vec);
}



inline bool Vector2::operator==(const Vector2& vec)
{
	return vector2::Equal(*this, vec);
}

inline bool Vector2::operator!=(const Vector2& vec)
{
	return vector2::NotEqual(*this, vec);
}

inline Vector2 Vector2::operator+(const Vector2& vec)
{
	return vector2::Add(*this, vec);
}

inline Vector2 Vector2::operator-(const Vector2& vec)
{
	return vector2::Sub(*this, vec);
}

inline Vector2 Vector2::operator*(const float& s)
{
	return vector2::Multiply(*this, s);
}

inline Vector2 Vector2::operator*(const Vector2& vec)
{
	return vector2::Multiply(*this, vec);
}

inline Vector2 Vector2::operator*(const float& s) const
{
	return vector2::Multiply(*this, s);
}

inline Vector2 Vector2::operator*(const Vector2& vec) const
{
	return vector2::Multiply(*this, vec);
}

inline Vector2 Vector2::operator/(const float& s) const
{
	return vector2::Divide(*this, s);
}

inline Vector2 Vector2::operator/(const Vector2& vec) const
{
	return vector2::Divide(*this, vec);
}

inline Vector2 Vector2::operator/(const float& s)
{
	return vector2::Divide(*this, s);
}

inline Vector2 Vector2::operator/(const Vector2& vec)
{
	return vector2::Divide(*this, vec);
}

inline Vector2 Vector2::operator+(const Vector2& vec) const
{
	return vector2::Add(*this, vec);
}

inline Vector2 Vector2::operator-(const Vector2& vec) const
{
	return vector2::Sub(*this, vec);
}

inline Vector2 Vector2::operator+=(const Vector2& vec)
{
	return *this = vector2::Add(*this, vec);
}

inline Vector2 Vector2::operator-=(const Vector2& vec)
{
	return *this = vector2::Sub(*this, vec);
}

inline Vector2 Vector2::operator*=(const float& s)
{
	return *this = vector2::Multiply(*this, s);
}

inline Vector2 Vector2::operator*=(const Vector2& vec)
{
	return *this = vector2::Multiply(*this, vec);
}

inline Vector2 Vector2::operator/=(const float& s)
{
	return *this = vector2::Divide(*this, s);
}

inline Vector2 Vector2::operator/=(const Vector2& vec)
{
	return *this = vector2::Divide(*this, vec);
}



inline bool Quaternion::operator==(const Quaternion& quat)
{
	return quaternion::Equal(*this, quat);
}

inline bool Quaternion::operator!=(const Quaternion& quat)
{
	return quaternion::NotEqual(*this, quat);
}

inline Quaternion Quaternion::operator*(const Quaternion& quat)
{
	return quaternion::Multiply(*this, quat);
}

inline Quaternion Quaternion::operator*(const Quaternion& quat) const
{
	return quaternion::Multiply(*this, quat);
}

inline Quaternion Quaternion::operator*=(const Quaternion& quat)
{
	return *this = quaternion::Multiply(*this, quat);
}
