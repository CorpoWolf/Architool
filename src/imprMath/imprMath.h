#pragma once
#include <maya/MPointArray.h>
#include <concepts>
#include <array>

constexpr double CmToInch = 2.54;
namespace imprMath {
	MPoint toInchMPoint(double x, double y, double z);
	MPoint toInchMPoint(double x, double y);

	template<typename... Args>
	requires (std::same_as<Args, double> && ...)
	[[nodiscard]] constexpr auto toInch(Args... args) noexcept -> std::array<double, sizeof...(Args)> {
		return {(args * CmToInch)...};
	}
}