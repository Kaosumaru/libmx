#pragma once
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"
#include "utils/Time.h"

#include<cmath>
#include<algorithm>

#define MX_PI        3.14159265358979323846
#define MX_2PI       6.28318530717958647692

namespace MX
{
	inline glm::vec2 operator *( const glm::vec2& v, const Time::FloatPerSecond& t )
	{
		return v * t.getPerSecond();
	}

	inline bool zero(const glm::vec2& v)
	{
		return v.x == 0 && v.y == 0;
	}

	inline float angle(const glm::vec2& v)
	{
		return atan2f(v.x,-v.y);
	}

	inline float lengthSquared(const glm::vec2& v)
	{
		return v.x*v.x+v.y*v.y;
	}

	inline glm::vec2 CreateVectorFromAngle(float a)
	{
		return glm::vec2(sinf(a), -cosf(a));
	}

	inline glm::vec2 vectorByRotation(const glm::vec2&o, float a)
	{
		auto cs = cosf(a);
		auto sn = sinf(a);
		return glm::vec2(o.x * cs - o.y * sn, o.x * sn + o.y * cs);
	}

	inline glm::vec2 lerp(const glm::vec2& a, const glm::vec2& b, float p)
	{
		return glm::vec2(a.x + (b.x-a.x)*p, a.y + (b.y-a.y)*p);
	}

	inline glm::vec2 vectorFromPointToPoint(const glm::vec2& a, const glm::vec2& b)
	{
		return b - a;
	}

	inline glm::vec2 mean(const glm::vec2& a, const glm::vec2& b)
	{
		return (a + b) / 2.0f;
	}

	inline float VectorDotProduct(const glm::vec2& a, const glm::vec2& b)
	{
		return a.x*b.x+a.y*b.y;
	}

	inline float VectorCrossProduct(const glm::vec2& v1, const glm::vec2& v2)
	{
		return (v1.x*v2.y) - (v1.y*v2.x);
	}

	inline void clamp(float &v, float min, float max)
	{
		v = std::max(v, min);
		v = std::min(v, max);
	}

	template <typename T>
	T angle_difference(T firstAngle, T secondAngle)
	{
		T difference = secondAngle - firstAngle;

		while (difference < -MX_PI) 
			difference = difference + (T)(2.*MX_PI);

		while (difference > MX_PI) 
			difference = difference - (T)(2.*MX_PI);

		return difference;
	}

	inline float angle_difference(const glm::vec2& a, const glm::vec2& b)
	{
		return angle_difference(angle(a), angle(b));
	}

	//rotates vector by value no larger than maxAngle to match targetVector, and return angle of new vector
	inline float rotateVectorToTarget(const glm::vec2& targetVector, glm::vec2& vector, float maxAngle)
	{
		float targetAngle = angle(targetVector);
		float angl = angle(vector);
		float angleDiff = angle_difference(targetAngle, angl);
		if (fabs(angleDiff) <= maxAngle)
		{
			vector = targetVector;
			return targetAngle;
		}

		float target_angle = angleDiff > 0.f ? angl - maxAngle : angl + maxAngle;
		vector =CreateVectorFromAngle(target_angle) * length(targetVector);
		return target_angle;
	}

	inline float rotateVectorToTarget(float targetAngle, glm::vec2& vector, float maxAngle)
	{
		float angl = angle(vector);
		float angleDiff = angle_difference(targetAngle, angl);
		if (fabs(angleDiff) <= maxAngle)
		{
			vector = MX::CreateVectorFromAngle(targetAngle);
			return targetAngle;
		}

		float target_angle = angleDiff > 0.f ? angl - maxAngle : angl + maxAngle;
		vector = MX::CreateVectorFromAngle(target_angle);
		return target_angle;
	}

	inline bool distanceBetweenPointsLowerThan(const glm::vec2& vector1, const glm::vec2& vector2, float distance)
	{
		float a = vector2.x - vector1.x;
		float b = vector2.y - vector1.y;
		float c = distance;
		return a*a + b*b <= c*c;

	}

	inline float distanceBetween(const glm::vec2& vector1, const glm::vec2& vector2)
	{
		return glm::distance(vector1, vector2);
	}

	inline float distanceBetweenSquared(const glm::vec2& vector1, const glm::vec2& vector2)
	{
		float a = vector2.x - vector1.x;
		float b = vector2.y - vector1.y;
		return a*a + b*b;
	}

}

