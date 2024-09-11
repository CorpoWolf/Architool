#include <cstdint>

#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>

template <typename T>
void cmdFlag(MArgDatabase& argData, const char* flag, T& arg) {
	if (argData.isFlagSet(flag)) {
		argData.getFlagArgument(flag, 0, arg);
		MGlobal::displayInfo(MString("Flag ") + flag + " is set.");
	}
}

// MSelectionList can have mutliple various data types
void archCmdFlag(MArgDatabase& argData, const char* flag, MString& arg) {
	cmdFlag(argData, flag, arg);
}

void archCmdFlag(MArgDatabase& argData, const char* flag, uint32_t& arg) {
	cmdFlag(argData, flag, arg);
}