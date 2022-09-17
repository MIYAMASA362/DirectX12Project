#pragma once
#ifndef __CORE_MATH_H__
#define __CORE_MATH_H__

#include <DirectXMath.h>

using namespace DirectX;

namespace mathf
{
	constexpr float kEpsilon = 1.0e-7f;
	constexpr float Epsilon = 1.192092896e-7f;
	constexpr float PI = DirectX::XM_PI;
	constexpr float PI2 = DirectX::XM_2PI;
	constexpr float PIDIV2 = DirectX::XM_PIDIV2;
	constexpr float PIDIV4 = DirectX::XM_PIDIV4;

	constexpr float Deg2Rad = PI / 180.0f;
	constexpr float Rad2Deg = 180.0f / PI;

	static float Clamp(float value, const float min, const float max)
	{
		value = max < value ? max : value;
		value = min > value ? min : value;
		return value;
	}

	static float Clamp01(float value)
	{
		value = 1.0f < value ? 1.0f : value;
		value = 0.0f > value ? 0.0f : value;
		return value;
	}

	static float Lerp(const float start, const float end, float time)
	{
		time = Clamp01(time);
		return (1 - time) * start + time * end;
	}

	static float Slerp(const float start, const float end, float time)
	{
		time = Clamp01(time);
		return sinf((1.0f - time) * PIDIV2) * start + sinf(time * PI2) * end;
	}

	static float Min(const float value0, const float value1)
	{
		return value0 < value1 ? value0 : value1;
	}

	inline  float Sqrtf(const float f) { return sqrtf(f); }

	inline float ToRadian(float euler) { return euler * Deg2Rad; }
	inline float ToEuler(const float radian) { return radian * Rad2Deg; }
	inline bool IsEqualUsingDot(const float dot) { return dot > 1.0f - kEpsilon; }

	inline float Sinf(float angle) { return sinf(angle); }
	inline float Cosf(float angle) { return cosf(angle); }
	inline float Tanf(float angle) { return tanf(angle); }

	inline float Asinf(float angle) { return asinf(angle); }
	inline float Acosf(float angle) { return acosf(angle); }
	inline float Atanf(float angle) { return atanf(angle); }
	inline float Atan2f(float y, float x) { return atan2f(y, x); }
}

struct Vector4
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float f[4];
	};

	Vector4() = default;

	Vector4(const Vector4&) = default;
	Vector4& operator=(const Vector4&) = default;

	Vector4(Vector4&&) = default;
	Vector4& operator=(Vector4&&) = default;

	constexpr Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
	explicit Vector4(const float* pfArray) : x(pfArray[0]), y(pfArray[1]), z(pfArray[2]), w(pfArray[3]) {}

	operator const float* () { return f; }
	float& operator[](int index) { return f[index]; }

	operator const XMVECTOR() { return XMVectorSet(x, y, z, w); }
	operator const XMFLOAT4() { return { x, y, z, w }; }

	inline bool operator==(const Vector4& vec);
	inline bool operator!=(const Vector4& vec);

	inline Vector4 operator+(const Vector4& vec);
	inline Vector4 operator-(const Vector4& vec);
	inline Vector4 operator*(const float& s);
	inline Vector4 operator*(const Vector4& vec);
	inline Vector4 operator/(const float& s);
	inline Vector4 operator/(const Vector4& vec);

	inline Vector4 operator+(const Vector4& vec) const;
	inline Vector4 operator-(const Vector4& vec) const;
	inline Vector4 operator*(const float& s) const;
	inline Vector4 operator*(const Vector4& vec) const;
	inline Vector4 operator/(const float& s) const;
	inline Vector4 operator/(const Vector4& vec) const;

	inline Vector4 operator+=(const Vector4& vec);
	inline Vector4 operator-=(const Vector4& vec);
	inline Vector4 operator*=(const float& s);
	inline Vector4 operator*=(const Vector4& vec);
	inline Vector4 operator/=(const float& s);
	inline Vector4 operator/=(const Vector4& vec);
};

struct Vector3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float f[4];
	};

	Vector3() = default;

	Vector3(const Vector3&) = default;
	Vector3& operator=(const Vector3&) = default;

	Vector3(Vector3&&) = default;
	Vector3& operator=(Vector3&&) = default;

	constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	explicit Vector3(const float* pfArray) : x(pfArray[0]), y(pfArray[1]), z(pfArray[2]) {}

	operator const float* () { return f; }
	float& operator[](int index) { return f[index]; }

	operator const XMVECTOR() { return XMVectorSet(x, y, z, 0.0f); }
	operator const XMFLOAT3() { return { x, y, z }; }

	inline bool operator==(const Vector3& vec);
	inline bool operator!=(const Vector3& vec);

	inline Vector3 operator+(const Vector3& vec);
	inline Vector3 operator-(const Vector3& vec);
	inline Vector3 operator*(const float& s);
	inline Vector3 operator*(const Vector3& vec);
	inline Vector3 operator/(const float& s);
	inline Vector3 operator/(const Vector3& vec);

	inline Vector3 operator+(const Vector3& vec) const;
	inline Vector3 operator-(const Vector3& vec) const;
	inline Vector3 operator*(const float& s) const;
	inline Vector3 operator*(const Vector3& vec) const;
	inline Vector3 operator/(const float& s) const;
	inline Vector3 operator/(const Vector3& vec) const;

	inline Vector3 operator+=(const Vector3& vec);
	inline Vector3 operator-=(const Vector3& vec);
	inline Vector3 operator*=(const float& s);
	inline Vector3 operator*=(const Vector3& vec);
	inline Vector3 operator/=(const float& s);
	inline Vector3 operator/=(const Vector3& vec);

};

struct Vector2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		float f[4];
	};

	Vector2() = default;

	Vector2(const Vector2&) = default;
	Vector2& operator=(const Vector2&) = default;

	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	constexpr Vector2(float x, float y) : x(x), y(y) {}
	explicit Vector2(const float* pfArray) : x(pfArray[0]), y(pfArray[1]) {}

	operator const float* () { return f; }
	float& operator[](int index) { return f[index]; }

	operator const XMVECTOR() { return XMVectorSet(x, y, 0.0f, 0.0f); }
	operator const XMFLOAT2() { return { x, y }; }

	inline bool operator==(const Vector2& vec);
	inline bool operator!=(const Vector2& vec);

	inline Vector2 operator+(const Vector2& vec);
	inline Vector2 operator-(const Vector2& vec);
	inline Vector2 operator*(const float& s);
	inline Vector2 operator*(const Vector2& vec);
	inline Vector2 operator/(const float& s);
	inline Vector2 operator/(const Vector2& vec);

	inline Vector2 operator+(const Vector2& vec) const;
	inline Vector2 operator-(const Vector2& vec) const;
	inline Vector2 operator*(const float& s) const;
	inline Vector2 operator*(const Vector2& vec) const;
	inline Vector2 operator/(const float& s) const;
	inline Vector2 operator/(const Vector2& vec) const;

	inline Vector2 operator+=(const Vector2& vec);
	inline Vector2 operator-=(const Vector2& vec);
	inline Vector2 operator*=(const float& s);
	inline Vector2 operator*=(const Vector2& vec);
	inline Vector2 operator/=(const float& s);
	inline Vector2 operator/=(const Vector2& vec);

};

struct Quaternion
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float f[4];
	};

	Quaternion() = default;

	Quaternion(const Quaternion&) = default;
	Quaternion& operator=(const Quaternion&) = default;

	Quaternion(Quaternion&&) = default;
	Quaternion& operator=(Quaternion&&) = default;

	constexpr Quaternion(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
	explicit Quaternion(const float* pfArray) : x(pfArray[0]), y(pfArray[1]), z(pfArray[2]), w(pfArray[3]) {}

	operator const XMVECTOR() { return XMVectorSet(x, y, z, w); }

	inline bool operator==(const Quaternion& quat);
	inline bool operator!=(const Quaternion& quat);

	inline Quaternion operator*(const Quaternion& quat);
	inline Quaternion operator*(const Quaternion& quat) const;
	inline Quaternion operator*=(const Quaternion& quat);
};

struct Matrix
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
		float f[16];
	};

	Matrix() = default;

	Matrix(const Matrix&) = default;
	Matrix& operator=(const Matrix&) = default;

	Matrix(Matrix&&) = default;
	Matrix& operator=(Matrix&&) = default;

	Matrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: _11(m00), _12(m01), _13(m02), _14(m03)
		, _21(m10), _22(m11), _23(m12), _24(m13)
		, _31(m20), _32(m21), _33(m22), _34(m23)
		, _41(m30), _42(m31), _43(m32), _44(m33)
	{

	}

	explicit Matrix(const float* pfArray)
		: _11(pfArray[0]), _12(pfArray[1]), _13(pfArray[2]), _14(pfArray[3])
		, _21(pfArray[4]), _22(pfArray[5]), _23(pfArray[6]), _24(pfArray[7])
		, _31(pfArray[8]), _32(pfArray[9]), _33(pfArray[10]), _34(pfArray[11])
		, _41(pfArray[12]), _42(pfArray[13]), _43(pfArray[14]), _44(pfArray[15])
	{

	}

	operator const XMMATRIX() { return XMMATRIX(f); }
};

namespace vector4
{
	constexpr Vector4 XAxis = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	constexpr Vector4 YAxis = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	constexpr Vector4 ZAxis = Vector4(0.0f, 0.0f, 1.0f, 0.0f);

	constexpr Vector4 zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	constexpr Vector4 one = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	constexpr Vector4 forward = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	constexpr Vector4 right = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	constexpr Vector4 up = Vector4(0.0f, 1.0f, 0.0f, 0.0f);

	inline bool IsNan(const Vector4& vec)
	{
		return isnan(vec.x) || isnan(vec.y) || isnan(vec.z) || isnan(vec.w);
	}

	inline bool Equal(const Vector4& vec0, const Vector4& vec1)
	{
		return vec0.x == vec1.x && vec0.y == vec1.y && vec0.z == vec1.z && vec0.w == vec1.w;
	}

	inline bool NotEqual(const Vector4& vec0, const Vector4& vec1)
	{
		return vec0.x != vec1.x || vec0.y != vec1.y || vec0.z != vec1.z || vec0.w != vec1.w;
	}

	inline Vector4 Add(const Vector4& vec0, const Vector4& vec1)
	{
		return { vec0.x + vec1.x, vec0.y + vec1.y, vec0.z + vec1.z, vec0.w + vec1.w };
	}

	inline Vector4 Sub(const Vector4& vec0, const Vector4& vec1)
	{
		return { vec0.x - vec1.x, vec0.y - vec1.y, vec0.z - vec1.z, vec0.w - vec1.w };
	}

	inline Vector4 Multiply(const Vector4& vec, const float scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar };
	}

	inline Vector4 Multiply(const Vector4& vec0, const Vector4& vec1)
	{
		return { vec0.x * vec1.x, vec0.y * vec1.y, vec0.z * vec1.z, vec0.w * vec1.w };
	}

	inline Vector4 Divide(const Vector4& vec, const float scalar)
	{
		return { vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar };
	}

	inline Vector4 Divide(const Vector4& vec0, const Vector4& vec1)
	{
		return { vec0.x / vec1.x, vec0.y / vec1.y, vec0.z / vec1.z, vec0.w / vec1.w };
	}

	inline float Lenght(const Vector4& vec)
	{
		return mathf::Sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
	}

	inline float LenghtSq(const Vector4& vec)
	{
		return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w);
	}
}

namespace vector3
{
	constexpr Vector3 XAxis = Vector3(1.0f, 0.0f, 0.0f);
	constexpr Vector3 YAxis = Vector3(0.0f, 1.0f, 0.0f);
	constexpr Vector3 ZAxis = Vector3(0.0f, 0.0f, 1.0f);

	constexpr Vector3 zero = Vector3(0.0f, 0.0f, 0.0f);
	constexpr Vector3 one = Vector3(1.0f, 1.0f, 1.0f);

	constexpr Vector3 forward = Vector3(0.0f, 0.0f, 1.0f);
	constexpr Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
	constexpr Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

	inline bool IsNan(const Vector3& vec)
	{
		return isnan(vec.x) || isnan(vec.y) || isnan(vec.z);
	}

	inline bool Equal(const Vector3& vec0, const Vector3& vec1)
	{
		return vec0.x == vec1.x && vec0.y == vec1.y && vec0.z == vec1.z;
	}

	inline bool NotEqual(const Vector3& vec0, const Vector3& vec1)
	{
		return vec0.x != vec1.x || vec0.y != vec1.y || vec0.z == vec1.z;
	}

	inline Vector3 Add(const Vector3& vec0, const Vector3& vec1)
	{
		return { vec0.x + vec1.x,vec0.y + vec1.y,vec0.z + vec1.z };
	}

	inline Vector3 Sub(const Vector3& vec0, const Vector3& vec1)
	{
		return { vec0.x - vec1.x, vec0.y - vec1.y, vec0.z - vec1.z };
	}

	inline Vector3 Multiply(const Vector3& vec0, const Vector3& vec1)
	{
		return { vec0.x * vec1.x, vec0.y * vec1.y, vec0.z * vec1.z };
	}

	inline Vector3 Multiply(const Vector3& vec, const float scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	inline Vector3 Divide(const Vector3& vec0, const Vector3& vec1)
	{
		return { vec0.x / vec1.x, vec0.y / vec1.y, vec0.z / vec1.z };
	}

	inline Vector3 Divide(const Vector3& vec, const float scalar)
	{
		return { vec.x / scalar, vec.y / scalar, vec.z / scalar };
	}

	inline float Length(const Vector3& vec)
	{
		return mathf::Sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	}

	inline float LengthSq(const Vector3& vec)
	{
		return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
	}

	inline float Dot(const Vector3& vec0, const Vector3& vec1)
	{
		return (vec0.x * vec1.x) + (vec0.y * vec1.y) + (vec0.z * vec1.z);
	}

	inline Vector3 Cross(const Vector3& vec0, const Vector3& vec1)
	{
		return { vec0.y * vec1.z - vec0.z * vec1.y, vec0.z * vec1.x - vec0.x * vec1.z, vec0.x * vec1.y - vec0.y * vec1.x };
	}

	inline Vector3 Normalize(const Vector3& vec)
	{
		float len = Length(vec);
		if (len == 0) {
			return zero;
		}
		return { vec.x / len, vec.y / len, vec.z / len };
	}

	inline Vector3 Lerp(const Vector3& vec0, const Vector3& vec1, const float t)
	{
		float f = mathf::Clamp01(t);
		return { (1 - f) * vec0.x + f * vec1.x, (1 - f) * vec0.y + f * vec1.y, (1 - f) * vec0.z + f * vec1.z };
	}

}

namespace vector2
{
	constexpr Vector2 XAxis = Vector2(1.0f, 0.0f);
	constexpr Vector2 YAxis = Vector2(0.0f, 1.0f);

	constexpr Vector2 zero = Vector2(0.0f, 0.0f);
	constexpr Vector2 one = Vector2(1.0f, 1.0f);

	constexpr Vector2 right = Vector2(1.0f, 0.0f);
	constexpr Vector2 up = Vector2(0.0f, 1.0f);

	inline bool IsNan(const Vector2& vec)
	{
		return isnan(vec.x) || isnan(vec.y);
	}

	inline bool Equal(const Vector2& vec0, const Vector2& vec1)
	{
		return vec0.x == vec1.x && vec0.y == vec1.y;
	}

	inline bool NotEqual(const Vector2& vec0, const Vector2& vec1)
	{
		return vec0.x != vec1.x || vec0.y != vec1.y;
	}

	inline float Dot(const Vector2& vec0, const Vector2& vec1) 
	{
		return (vec0.x * vec1.x) + (vec0.y * vec1.y);
	}

	inline Vector2 Cross(const Vector2& vec0, const Vector2& vec1)
	{
		return Vector2(
			vec0.x * vec1.y - vec0.y * vec1.x,
			vec0.y * vec1.x - vec0.x * vec1.y
		);
	}

	inline Vector2 Add(const Vector2& vec0, const Vector2& vec1)
	{
		return { vec0.x + vec1.x, vec0.y + vec1.y };
	}

	inline Vector2 Sub(const Vector2& vec0, const Vector2& vec1)
	{
		return { vec0.x - vec1.x, vec0.y - vec1.y }; 
	}

	inline Vector2 Multiply(const Vector2& vec0, const float& scalar)
	{
		return { vec0.x * scalar, vec0.y * scalar };
	}

	inline Vector2 Multiply(const Vector2& vec0, const Vector2& vec1)
	{
		return { vec0.x * vec1.x, vec0.y * vec1.y };
	}

	inline Vector2 Divide(const Vector2& vec0, const float& scalar)
	{
		return { vec0.x / scalar, vec0.y / scalar };
	}

	inline Vector2 Divide(const Vector2& vec0, const Vector2& vec1)
	{
		return { vec0.x / vec1.x, vec0.y / vec1.y };
	}

	inline float Length(const Vector2& vec)
	{
		return mathf::Sqrtf((vec.x * vec.x) + (vec.y * vec.y));
	}

	inline float LenghtSq(const Vector2& vec)
	{
		return (vec.x * vec.x) + (vec.y * vec.y);
	}

	inline Vector2 Normalize(const Vector2& vec)
	{
		float len = Length(vec);
		if (len == 0) {
			return zero;
		}
		return { vec.x / len, vec.y / len };
	}

	inline Vector2 Lerp(const Vector2& vec0, const Vector2& vec1, const float t)
	{
		float f = mathf::Clamp01(t);
		return { (1 - f) * vec0.x + vec1.x * f, (1 - f) * vec0.y + vec1.y * f };
	}

}

namespace quaternion
{
	constexpr Quaternion identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	inline bool IsNan(const Quaternion& quat)
	{
		return isnan(quat.x) || isnan(quat.y) || isnan(quat.z) || isnan(quat.w);
	}

	inline bool Equal(const Quaternion& quat0, const Quaternion& quat1)
	{
		return quat0.x == quat1.x && quat0.y == quat1.y && quat0.z == quat1.z && quat0.w == quat1.w;
	}

	inline bool NotEqual(const Quaternion& quat0, const Quaternion& quat1)
	{
		return quat0.x != quat1.x || quat0.y != quat1.y || quat0.z != quat1.z || quat0.w != quat1.w;
	}

	inline float Dot(const Quaternion& quat0, const Quaternion& quat1)
	{
		return quat0.x * quat1.x + quat0.y * quat1.y + quat0.z * quat1.z + quat0.w * quat1.w;
	}

	inline float Length(const Quaternion& quat)
	{
		return sqrtf(quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w);
	}

	inline float LengthSq(const Quaternion& quat)
	{
		return quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
	}

	inline float Angle(const Quaternion& quat0, const Quaternion& quat1)
	{
		float dot = Dot(quat0, quat1);
		if (mathf::IsEqualUsingDot(dot)) {
			return 0.0f;
		}
		else {
			return mathf::Acosf(mathf::Min(fabsf(dot), 1.0f) * 2.0f * mathf::Rad2Deg);
		}
	}

	inline bool IsIdentity(const Quaternion& quat)
	{
		return quat.x == 0.0f && quat.y == 0.0f && quat.z == 0.0f && quat.w == 1.0f;
	}

	inline Quaternion Normalize(const Quaternion& quat)
	{
		float mag = sqrtf(Dot(quat, quat));
		if (mag < 1.17549435E-38f) {
			return identity;
		}
		else {
			return { quat.x / mag, quat.y / mag, quat.z / mag, quat.w / mag };
		}
	}

	inline Quaternion Conjugate(const Quaternion& quat)
	{
		return Quaternion(-quat.x, -quat.y, -quat.z, quat.w);
	}

	inline Quaternion Multiply(const Quaternion& quat0, const Quaternion& quat1)
	{
		return{
			quat0.x * quat1.w + quat0.w * quat1.x - quat0.z * quat1.y + quat0.y * quat1.z,
			quat0.y * quat1.w + quat0.z * quat1.x + quat0.w * quat1.y - quat0.x * quat1.z,
			quat0.z * quat1.w - quat0.y * quat1.x + quat0.x * quat1.y + quat0.w * quat1.z,
			quat0.w * quat1.w - quat0.x * quat1.x - quat0.y * quat1.y - quat0.z * quat1.z
		};
	}

	inline Quaternion Lerp(const Quaternion& start, const Quaternion& end, const float time)
	{
		float t = mathf::Clamp01(time);
		return{
			start.x * (1.0f - t) + end.x * t,
			start.y * (1.0f - t) + end.y * t,
			start.z * (1.0f - t) + end.z * t,
			start.w * (1.0f - t) + end.w * t
		};
	}

	inline Quaternion Slerp(const Quaternion& start, const Quaternion& end, const float time)
	{
		float t = mathf::Clamp01(time);
		float dot = Dot(start, end);
		float sinDot = sinf(dot);
		float lerp0 = sinf((1.0f - t) * dot) / sinDot;
		float lerp1 = sinf((t * dot)) / sinDot;
		return{
			start.x * lerp0 + end.x * lerp1,
			start.y * lerp0 + end.y * lerp1,
			start.z * lerp0 + end.z * lerp1,
			start.w * lerp0 + end.w * lerp1
		};
	}

	inline Quaternion Euler(Vector3& vec)
	{
		float x = mathf::ToRadian(vec.x);
		float y = mathf::ToRadian(vec.y);
		float z = mathf::ToRadian(vec.z);

		float cosX = mathf::Cosf(x * 0.5f);
		float cosY = mathf::Cosf(y * 0.5f);
		float cosZ = mathf::Cosf(z * 0.5f);

		float sinX = mathf::Sinf(x * 0.5f);
		float sinY = mathf::Sinf(y * 0.5f);
		float sinZ = mathf::Sinf(z * 0.5f);

		return {
			sinX * cosY * cosZ + cosX * sinY * sinZ,
			cosX * sinY * cosZ - sinX * cosY * sinZ,
			cosX * cosY * sinZ - sinX * sinY * cosZ,
			cosX * cosY * cosZ + sinX * sinY * sinZ
		};
	}

	inline Vector3 ToEuler(const Quaternion& quat)
	{
		float xx = quat.x * quat.x;
		float yy = quat.y * quat.y;
		float zz = quat.z * quat.z;
		float ww = quat.w * quat.w;

		float xy = quat.x * quat.y;
		float xz = quat.x * quat.z;
		float yz = quat.y * quat.z;
		float wx = quat.w * quat.x;
		float wy = quat.w * quat.y;
		float wz = quat.w * quat.z;

		float m00 = 1.0f - (2.0f * yy) - (2.0f * zz);
		float m01 = (2.0f * xy) + (2.0f * wz);
		float m10 = (2.0f + xy) - (2.0f * wz);
		float m11 = 1.0f - (2.0f * xx) - (2.0f * zz);
		float m20 = (2.0f * xz) + (2.0f * wy);
		float m21 = (2.0f * yz) - (2.0f * wx);
		float m22 = 1.0f - (2.0f * xx) - (2.0f * yy);

		Vector3 euler;
		if (1.0f - mathf::kEpsilon < m10 && m10 < 1.0f + mathf::kEpsilon) {
			euler.x = -mathf::PI2;
			euler.y = 0.0f;
			euler.z = mathf::Atan2f(m10, m00);
		}
		else if (-1.0f - mathf::kEpsilon < m21 && m21 < -1.0f - mathf::kEpsilon) {
			euler.x = -mathf::PI2;
			euler.y = 0.0f;
			euler.z = mathf::Atan2f(m10, m00);
		}
		else {
			euler.x = mathf::Asinf(-m21);
			euler.y = mathf::Atan2f(m20, m22);
			euler.z = mathf::Atan2f(m01, m11);
		}

		euler.x *= mathf::Rad2Deg;
		euler.y *= mathf::Rad2Deg;
		euler.z *= mathf::Rad2Deg;
		return euler;
	}

	inline Quaternion Radian(Vector3& vec)
	{
		float cosX = cosf(vec.x * 0.5f);
		float cosY = cosf(vec.y * 0.5f);
		float cosZ = cosf(vec.z * 0.5f);

		float sinX = sinf(vec.x * 0.5f);
		float sinY = sinf(vec.y * 0.5f);
		float sinZ = sinf(vec.z * 0.5f);

		return {
			sinX * cosY * cosZ + cosX * sinY * sinZ,
			cosX * sinY * cosZ - sinX * cosY * sinZ,
			cosX * cosY * sinZ - sinX * sinY * cosZ,
			cosX * cosY * cosZ + sinX * sinY * sinZ
		};
	}

	inline Quaternion AxisToRadian(const float radian, Vector3 axis)
	{
		float angle = radian * 0.5f;
		return {
			axis.x * sinf(angle),
			axis.y * sinf(angle),
			axis.z * sinf(angle),
			cosf(angle)
		};
	}

	inline Quaternion AxisToEuler(const float euler, Vector3 axis)
	{
		return AxisToRadian(euler * mathf::Deg2Rad, axis);
	}

	inline Quaternion LookAt(Vector3& lookAt, Vector3 up)
	{
		return AxisToRadian(mathf::Atan2f(lookAt.x, lookAt.z), up);
	}

	inline Quaternion LookForawrd(Vector3& forward, Vector3& up)
	{
		Vector3 vec = vector3::Normalize(forward);
		Vector3 vec0 = vector3::Normalize(vector3::Cross(up, vec));
		Vector3 vec1 = vector3::Cross(vec, vec0);

		float m00 = vec0.x;
		float m01 = vec0.y;
		float m02 = vec0.z;

		float m10 = vec1.x;
		float m11 = vec1.y;
		float m12 = vec1.z;

		float m20 = vec.x;
		float m21 = vec.y;
		float m22 = vec.z;

		float num8 = (m00 + m11) + m22;
		Quaternion quat;
		if (num8 > 0.0f) {
			float num = sqrtf(num8 + 1);
			quat.w = num * 0.5f;
			num = 0.5f / num;
			quat.x = (m12 - m21) * num;
			quat.y = (m20 - m02) * num;
			quat.z = (m20 - m10) * num;
			return quat;
		}
		if ((m00 >= m11) && (m00 >= m22)) {
			float num7 = sqrtf((1.0f + m00) - m11 - m22);
			float num4 = 0.5f / num7;
			quat.x = 0.5f * num7;
			quat.y = (m01 + m10) * num4;
			quat.z = (m02 + m20) * num4;
			quat.w = (m11 - m21) * num4;
			return quat;
		}
		if (m11 > m22) {
			float num6 = sqrtf((1.0f + m11) - m00 - m22);
			float num3 = 0.5f / num6;
			quat.x = (m10 + m01) * num3;
			quat.y = 0.5f * num6;
			quat.z = (m21 + m12) * num3;
			quat.w = (m20 - m02) * num3;
			return quat;
		}
		float num5 = sqrtf((1.0f + m22) - m00 - m11);
		float num2 = 0.5f / num5;
		quat.x = (m20 + m02) * num2;
		quat.y = (m21 + m11) * num2;
		quat.z = 0.5f * num5;
		quat.w = (m01 - m10) * num2;

		if (IsNan(quat)) {
			return identity;
		}
		return quat;
	}

	inline Quaternion FromToRotation(Vector3 from, Vector3 to)
	{
		return AxisToRadian(vector3::Dot(from, to), vector3::Cross(from, to));
	}

	inline Matrix ToMatrix(Quaternion& quat)
	{
		float xx = quat.x * quat.x * 2.0f;
		float yy = quat.y * quat.y * 2.0f;
		float zz = quat.z * quat.z * 2.0f;

		float xy = quat.x * quat.y * 2.0f;
		float yz = quat.y * quat.z * 2.0f;
		float zx = quat.z * quat.x * 2.0f;

		float xw = quat.x * quat.w * 2.0f;
		float yw = quat.y * quat.w * 2.0f;
		float zw = quat.z * quat.w * 2.0f;

		Matrix mtx;

		mtx.m[0][0] = 1.0f - yy - zz;
		mtx.m[0][1] = xy + zw;
		mtx.m[0][2] = zx - yw;
		mtx.m[0][3] = 0.0f;

		mtx.m[1][0] = xy - zw;
		mtx.m[1][1] = 1.0f - zz - xx;
		mtx.m[1][2] = yz + xw;
		mtx.m[1][3] = 0.0f;

		mtx.m[2][0] = zx + yw;
		mtx.m[2][1] = yz - xw;
		mtx.m[2][2] = 1.0f - xx - yy;
		mtx.m[2][3] = 0.0f;

		mtx.m[3][0] = 0.0f;
		mtx.m[3][1] = 0.0f;
		mtx.m[3][2] = 0.0f;
		mtx.m[3][3] = 1.0f;

		return mtx;
	}

	
}

#include "MathVector.inl"

#endif