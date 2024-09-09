#include "imprMath.hpp"

namespace imprLib {

	MPoint toInchMPoint(double x, double y, double z) {
		return MPoint(x * CmToInch, y * CmToInch, z * CmToInch);
	}
	MPoint toInchMPoint(double x, double y) {
		return MPoint(x * CmToInch, y * CmToInch);
	}
}