#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "./wallTool/wallTool.h"

MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "Architools", "1.0", "Any");
	plugin.registerNode("ArchiWallNode", WallNode::id, WallNode::creator, WallNode::initialize);
	plugin.registerCommand("ArchiCreateWall", WallCreateCmd::creator);

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	plugin.deregisterCommand("ArchiWallNode");
	plugin.deregisterCommand("ArchiCreateWall");

	return MS::kSuccess;
}