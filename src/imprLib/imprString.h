namespace imprLib {
	struct FtIn {
		double Ft;
		double Inch;
	};

	FtIn parseImprString(const std::string& strData);
}