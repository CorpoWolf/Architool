#include "imprMath.h"

namespace imprMath {

	MPoint toInchMPoint(double x, double y, double z) {
		return MPoint(x * CmToInch, y * CmToInch, z * CmToInch);
	}
	MPoint toInchMPoint(double x, double y) {
		return MPoint(x * CmToInch, y * CmToInch);
	}

	std::array<double, 3> toInch(double x, double y, double z) {
		return {x * CmToInch, y * CmToInch, z * CmToInch};
	}
	std::array<double, 2> toInch(double x, double y) {
		return {x * CmToInch, y * CmToInch};
	}
	double toInch(double x) {
		return (x * CmToInch);
	}
}