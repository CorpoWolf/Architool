#include <cstdint>

#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>

namespace archiUtil {
	template <typename T>
	void cmdFlagTMP(MArgDatabase& argData, const char* flag, T& arg) {
		if (argData.isFlagSet(flag)) {
			argData.getFlagArgument(flag, 0, arg);
			MGlobal::displayInfo(MString("Flag ") + flag + " is set.");
		}
	}

	// MSelectionList can have mutliple various data types
	void CmdFlag(MArgDatabase& argData, const char* flag, MString& arg, MPlug& plug) {
		cmdFlagTMP(argData, flag, arg);
		plug.setString(arg);
	}

	void CmdFlag(MArgDatabase& argData, const char* flag, uint32_t& arg, MPlug& plug) {
		cmdFlagTMP(argData, flag, arg);
		plug.setInt(arg);
	}
}