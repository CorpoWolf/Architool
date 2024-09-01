#include "archiMath.h"

MPoint imp_inch(double x, double y, double z) {
	return MPoint(x * inch, y * inch, z * inch);
}