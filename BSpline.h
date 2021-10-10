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

	// Used to add initial control point before curve is complete
	void AddControlPoint(Point p);
	// Used to add new control point after curve is complete
	void AddControlPoint(int existing_control_point_index);
	void SetPoint(int index, Point new_point);
	const std::vector<Point> ControlPoints() const;
	// Indicates if the spline has been supplied enough points to be drawn
	bool Complete() const;
	void Save(const char* path) const;
	std::vector<Point> GetPolyLine(int steps = 1000) const;
	int Degree() const { return degree; }
private:
	static Point DeBoor(const BSpline& bspline, float u);
	std::vector<Point> control_points;
	std::vector<float> knots;
	int num_points;
	int degree;
};

#endif // BSPLINE_H