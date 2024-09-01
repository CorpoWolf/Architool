## Some C++ standard library arrays
```cpp
std::array<double, 3> toInch(double x, double y, double z) {
	return {x * CmToInch, y * CmToInch, z * CmToInch};
}
std::array<double, 2> toInch(double x, double y) {
	return {x * CmToInch, y * CmToInch};
}
double toInch(double x) {
	return (x * CmToInch);
}
```