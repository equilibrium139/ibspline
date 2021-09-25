#ifndef POINT_H
#define POINT_H

#include <ostream>

extern const int max_graph_x;
extern const int max_graph_y;

struct Point
{
	float x, y;
};

inline Point operator*(float scalar, Point point)
{
	return
	{
		scalar * point.x,
		scalar * point.y
	};
}

inline Point operator+(Point a, Point b)
{
	return
	{
		a.x + b.x,
		a.y + b.y
	};
}

inline std::ostream& operator<<(std::ostream& os, Point point)
{
	return os << point.x << " " << point.y;
}

struct ScreenPoint
{
	int x, y;
};

// Convert screen point to [0, 1] x [0, 1] range
inline Point Normalize(int x, int y)
{

	return
	{
		(float)x / (max_graph_x),
		(float)(max_graph_y - y) / max_graph_y // y needs to be flipped up because in screen space it goes down
	};
}

// Convert point in range [0, 1] x [0, 1] to a pixel on the screen
inline ScreenPoint GetScreenPoint(Point point)
{
	return
	{
		std::lround(point.x * max_graph_x),
		std::lround((1.0f - point.y) * max_graph_y)
	};
}

#endif // POINT_H