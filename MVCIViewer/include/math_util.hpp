#pragma once
#include <cmath>

struct FVector2D_Custom {
	float X, Y;

	FVector2D_Custom()
	: X(0)
	, Y(0) {}
	FVector2D_Custom(float X, float Y)
	: X(X)
	, Y(Y) {}
	FVector2D_Custom(double X, double Y)
	: X(static_cast<float>(X))
	, Y(static_cast<float>(Y)) {}
	FVector2D_Custom(int X, int Y)
	: X(static_cast<float>(X))
	, Y(static_cast<float>(Y)) {}

	FVector2D_Custom Rotate(const float angle) const
	{
		const auto ca = cosf(angle);
		const auto sa = sinf(angle);
		return FVector2D_Custom(X * ca + Y * -sa, X * sa + Y * ca);
	}

	float SizeSquared() const
	{
		return X * X + Y * Y;
	}

	float Size() const
	{
		return sqrt(SizeSquared());
	}

	FVector2D_Custom operator+(const FVector2D_Custom& other) const
	{
		return FVector2D_Custom(X + other.X, Y + other.Y);
	}

	FVector2D_Custom operator-(const FVector2D_Custom& other) const
	{
		return FVector2D_Custom(X - other.X, Y - other.Y);
	}

	FVector2D_Custom operator*(float scalar) const
	{
		return FVector2D_Custom(X * scalar, Y * scalar);
	}

	FVector2D_Custom operator/(float scalar) const
	{
		return FVector2D_Custom(X / scalar, Y / scalar);
	}
};

template<typename T>
T min_(T a, T b)
{
	return a < b ? a : b;
}

template<typename T>
T max_(T a, T b)
{
	return a > b ? a : b;
}

bool line_box_intersection(
	const FVector2D_Custom& box_min, const FVector2D_Custom& box_max,
	const FVector2D_Custom& line_a, const FVector2D_Custom& line_b,
	float* entry_fraction, float* exit_fraction);
