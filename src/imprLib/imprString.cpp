#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include "imprString.hpp"

namespace imprLib {
	const std::string delimiters[] = {"ft", "'", "in", "\""};

	FtIn strFtIn(std::string strData) {
		std::vector<std::string> tokens;
		std::string token;

		if (strData.empty()) {
			return {0.0, 0.0};
		}
		strData.erase(std::remove(strData.begin(), strData.end(), ' '), strData.end()); // Remove all spaces

		double ft = 0.0;
		double inch = 0.0;

		for (const std::string& delim : delimiters) { // Loop through all deliminators
			size_t pos = strData.find(delim);
			if (pos != std::string::npos) {
				if (delim == "ft" || delim == "'") {
					ft = std::stod(strData.substr(0, pos));
					strData = strData.substr(pos + delim.length());
				} else if (delim == "in" || delim == "\"") {
					inch = std::stod(strData.substr(0, pos));
					strData = strData.substr(pos + delim.length());
				}
			}
		}
		return {ft, inch};
	}
}