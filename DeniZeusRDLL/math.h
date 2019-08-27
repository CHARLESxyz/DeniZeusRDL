#pragma once
#include <sstream>
#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846

class Vector
{
public:
	Vector(void)
	{
		Invalidate();
	}
	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	Vector(const float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix; y = iy; z = iz;
	}
	bool IsValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}
	void Invalidate()
	{
		x = y = z = std::numeric_limits<float>::infinity();
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}
	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	void Zero()
	{
		x = y = z = 0.0f;
	}

	bool operator==(const Vector& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}
	bool operator!=(const Vector& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	Vector& operator-=(const Vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	Vector& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}
	Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	Vector& operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}
	Vector& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	Vector& operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	void NormalizeInPlace()
	{
		*this = Normalized();
	}
	Vector Normalized() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}

	float DistTo(const Vector& vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float DistToSqr(const Vector& vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	float Dot(const Vector& vOther) const
	{
		return (x * vOther.x + y * vOther.y + z * vOther.z);
	}
	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}
	float Length2D() const
	{
		return sqrt(x * x + y * y);
	}

	Vector& operator=(const Vector& vOther)
	{
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}

	Vector operator-(void) const
	{
		return Vector(-x, -y, -z);
	}
	Vector operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	Vector operator*(float fl) const
	{
		return Vector(x * fl, y * fl, z * fl);
	}
	Vector operator*(const Vector& v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}
	Vector operator/(float fl) const
	{
		return Vector(x / fl, y / fl, z / fl);
	}
	Vector operator/(const Vector& v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	float x, y, z;
};

inline Vector operator*(float lhs, const Vector& rhs)
{
	return rhs * lhs;
}
inline Vector operator/(float lhs, const Vector& rhs)
{
	return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	inline VectorAligned(void) {};
	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

public:
	explicit VectorAligned(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned& operator=(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned& operator=(const VectorAligned& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	float w;
};

class QAngle
{
public:
	QAngle(void)
	{
		Init();
	}
	QAngle(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}
	QAngle(const float* clr)
	{
		Init(clr[0], clr[1], clr[2]);
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		pitch = ix;
		yaw = iy;
		roll = iz;
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}
	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	QAngle& operator+=(const QAngle& v)
	{
		pitch += v.pitch; yaw += v.yaw; roll += v.roll;
		return *this;
	}
	QAngle& operator-=(const QAngle& v)
	{
		pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
		return *this;
	}
	QAngle& operator*=(float fl)
	{
		pitch *= fl;
		yaw *= fl;
		roll *= fl;
		return *this;
	}
	QAngle& operator*=(const QAngle& v)
	{
		pitch *= v.pitch;
		yaw *= v.yaw;
		roll *= v.roll;
		return *this;
	}
	QAngle& operator/=(const QAngle& v)
	{
		pitch /= v.pitch;
		yaw /= v.yaw;
		roll /= v.roll;
		return *this;
	}
	QAngle& operator+=(float fl)
	{
		pitch += fl;
		yaw += fl;
		roll += fl;
		return *this;
	}
	QAngle& operator/=(float fl)
	{
		pitch /= fl;
		yaw /= fl;
		roll /= fl;
		return *this;
	}
	QAngle& operator-=(float fl)
	{
		pitch -= fl;
		yaw -= fl;
		roll -= fl;
		return *this;
	}

	QAngle& operator=(const QAngle& vOther)
	{
		pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
		return *this;
	}

	QAngle operator-(void) const
	{
		return QAngle(-pitch, -yaw, -roll);
	}
	QAngle operator+(const QAngle& v) const
	{
		return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
	}
	QAngle operator-(const QAngle& v) const
	{
		return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
	}
	QAngle operator*(float fl) const
	{
		return QAngle(pitch * fl, yaw * fl, roll * fl);
	}
	QAngle operator*(const QAngle& v) const
	{
		return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
	}
	QAngle operator/(float fl) const
	{
		return QAngle(pitch / fl, yaw / fl, roll / fl);
	}
	QAngle operator/(const QAngle& v) const
	{
		return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
	}

	float Length() const
	{
		return sqrt(pitch * pitch + yaw * yaw + roll * roll);
	}
	float LengthSqr(void) const
	{
		return (pitch * pitch + yaw * yaw + roll * roll);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (pitch > -tolerance && pitch < tolerance &&
			yaw > -tolerance && yaw < tolerance &&
			roll > -tolerance && roll < tolerance);
	}

	float Normalize() const
	{
		QAngle res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res[0] = res[1] = res[2] = 0.0f;
		}
		return l;
	}

	float pitch;
	float yaw;
	float roll;
};

inline QAngle operator*(float lhs, const QAngle& rhs)
{
	return rhs * lhs;
}
inline QAngle operator/(float lhs, const QAngle& rhs)
{
	return rhs / lhs;
}

namespace Math
{
	QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		QAngle vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
		vAngle.roll = 0.0f;

		if (delta.x >= 0.0)
			vAngle.yaw += 180.0f;

		return vAngle;
	}


	void AngleVectors(const QAngle& angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		Vector ang, aim;

		AngleVectors(viewAngle, aim);
		AngleVectors(aimAngle, ang);

		return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
	}

	void VectorAngles(const Vector& forward, QAngle& angles)
	{
		float	tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	void NormalizeClamp(QAngle& angles)
	{
		while (angles.pitch < -180.0f) angles.pitch += 360.0f;
		while (angles.yaw < -180.0f) angles.yaw += 360.0f;

		while (angles.pitch > 180.0f) angles.pitch -= 360.0f;
		while (angles.yaw > 180.0f) angles.yaw -= 360.0f;

		if (angles.pitch > 89.0f) angles.pitch = 89.0f;
		else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

		if (angles.yaw > 180.0f) angles.yaw = 180.0f;
		else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

		angles.roll = 0;
	}

	void SmoothAngle(float smooth, QAngle currentAngle, QAngle aimAngle, QAngle& angle)
	{
		/*QAngle dest = angle;
		if (smooth > 0)
		{
			QAngle delta;
			delta.yaw = currentAngle.yaw - aimAngle.yaw;
			delta.pitch = currentAngle.pitch - aimAngle.pitch;
			NormalizeClamp(delta);
			dest.yaw = currentAngle.yaw - delta.yaw / (smooth);
			dest.pitch = currentAngle.pitch - delta.pitch / (smooth);
			NormalizeClamp(dest);
		}

		angle = dest;*/

		QAngle dest;
		if (smooth > 0)
		{
			QAngle delta;
			delta.yaw = currentAngle.yaw - aimAngle.yaw;
			delta.pitch = currentAngle.pitch - aimAngle.pitch;
			NormalizeClamp(delta);
			dest.yaw = currentAngle.yaw - delta.yaw / smooth;
			dest.pitch = currentAngle.pitch - delta.pitch / smooth;
			NormalizeClamp(dest);
		}

		angle = dest;

	}

}