#pragma once
#include <maya/MPointArray.h>
#include <array>

constexpr double CmToInch = 2.54;
namespace imprMath {
	MPoint toInchMPoint(double x, double y, double z);
	MPoint toInchMPoint(double x, double y);
	std::array<double, 3> toInch(double x, double y, double z);
	std::array<double, 2> toInch(double x, double y);
	double toInch(double x);
}