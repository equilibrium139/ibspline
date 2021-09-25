#include "BSpline.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

BSpline::BSpline(int num_points, int degree)
	:num_points(num_points), degree(degree)
{
	for (int i = 0; i < degree + 1; i++)
	{
		knots.push_back(0);
	}
	for (int i = 0; i < num_points - degree - 1; i++)
	{
		knots.push_back(i + 1);
	}
	for (int i = 0; i < degree + 1; i++)
	{
		knots.push_back(num_points - degree);
	}
}

BSpline::BSpline(const char* path)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line) && !std::isdigit(line[0])); // Skip until first relevant line

	std::istringstream sstream(line);
	int n_dim, n_points, n_degree;
	sstream >> n_dim >> n_points >> n_degree;

	this->degree = n_degree;
	this->num_points = n_points;

	std::getline(file, line); // knot vector line
	std::istringstream knot_vector_stream(line);
	for (float knot; knot_vector_stream >> knot;)
	{
		knots.push_back(knot);
	}

	for (int i = 0; i < n_points; i++)
	{
		std::getline(file, line);
		std::istringstream sstream(line);
		Point point;
		sstream >> point.x >> point.y;
		control_points.push_back(point);
	}
}

void BSpline::AddControlPoint(Point p)
{
	assert(control_points.size() < num_points);

	control_points.push_back(p);
}

void BSpline::SetPoint(int index, Point new_point)
{
	control_points[index] = new_point;
}

const std::vector<Point> BSpline::ControlPoints() const
{
	return control_points;
}

bool BSpline::Complete() const
{
	return control_points.size() >= num_points;
}

void BSpline::Save(const char* path) const
{
	const int dimension = 2;
	const int n_points = control_points.size();

	std::ofstream file(path);
	file << "BSPLINE\n";
	file << dimension << " " << n_points << " " << degree << '\n';

	for (Point point : control_points)
	{
		file << point << '\n';
	}
}

std::vector<Point> BSpline::GetPolyLine(int steps) const
{
	std::vector<Point> polyline;
	const float step = (float)(knots.back() - knots.front()) / (float)steps;
	polyline.push_back(control_points.front());
	for (float u = step; u <= knots.back() - step; u += step)
	{
		polyline.push_back(DeBoor(*this, u));
	}
	polyline.push_back(control_points.back());
	return polyline;
}

Point BSpline::DeBoor(const BSpline& bspline, float u)
{
	const std::vector<float>& t = bspline.knots; 
	int j = -1;
	for (int i = 1; i < t.size(); i++)
	{
		if (t[i] > u)
		{
			j = i - 1; 
			break;
		}
	}

	assert(j != -1);

	const int k = bspline.degree + 1;
	std::vector<std::vector<Point>> levels(k);
	for (auto& level : levels) level.resize(bspline.control_points.size());
	levels[0] = bspline.control_points;

	for (int r = 1; r <= bspline.degree; r++)
	{
		for (int i = j - k + 1 + r; i <= j; i++)
		{
			levels[r][i] = (1 - (u - t[i]) / (t[i + k - r] - t[i])) * levels[r - 1][i - 1] +
				((u - t[i]) / (t[i + k - r] - t[i])) * levels[r - 1][i];
		}
	}

	return levels[bspline.degree][j];
}
