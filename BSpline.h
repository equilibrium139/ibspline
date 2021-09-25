#ifndef BSPLINE_H
#define BSPLINE_H

#include "Point.h"
#include <vector>

class BSpline
{
public:
	BSpline(int num_points, int degree);
	BSpline(const char* path);
	BSpline() = default;
	void AddControlPoint(Point p);
	void SetPoint(int index, Point new_point);
	const std::vector<Point> ControlPoints() const;
	bool Complete() const;
	void Save(const char* path) const;
	std::vector<Point> GetPolyLine(int steps = 100) const;
private:
	static Point DeBoor(const BSpline& bspline, float u);
	std::vector<Point> control_points;
	std::vector<float> knots;
	int num_points;
	int degree;
};

#endif // BSPLINE_H