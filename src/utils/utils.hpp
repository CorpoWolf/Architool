#pragma once

namespace archiUtil {
	void CmdFlag(MArgDatabase& argData, const char* flag, MString& arg, MPlug& plug);
	void CmdFlag(MArgDatabase& argData, const char* flag, uint32_t& arg, MPlug& plug);
}