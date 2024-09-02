## Example of imprLib::toInch Template working.
```cpp
auto restult = imprLib::toInch(13.2, 25.0, 43.5, 23.75);
std::string resultString = std::format("Result: {}, {}, {}, {}", restult[0], restult[1], restult[2], restult[3]);
MGlobal::displayInfo(resultString.c_str());
```