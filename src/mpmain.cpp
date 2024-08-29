#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "./wallTool/wallTool.h"

MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "Architools", "1.0", "Any");
	return plugin.registerCommand("ArchiCreateWall", WallCreateCmd::creator);
}

MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	return plugin.deregisterCommand("ArchiCreateWall");
}