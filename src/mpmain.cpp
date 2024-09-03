#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "./ArchiWalls/wallGen.h"
#include "./ArchiWalls/wallModOpen.h"

MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "Architools", "1.0", "Any");
	plugin.registerNode("ArchiWallNode", ArchiWallNode::id, ArchiWallNode::creator, ArchiWallNode::initialize);
	plugin.registerCommand("ArchiCreateWall", WallCreateCmd::creator);

	plugin.registerNode("ArchiWallOpenNode", ArchiWallOpenNode::id, ArchiWallOpenNode::creator, ArchiWallOpenNode::initialize);
	plugin.registerCommand("ArchiModWallOpen", WallModOpenCmd::creator);

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);

	plugin.deregisterNode(ArchiWallNode::id);
	plugin.deregisterCommand("ArchiCreateWall");

	plugin.deregisterNode(ArchiWallOpenNode::id);
	plugin.deregisterCommand("ArchiModWallOpen");

	return MS::kSuccess;
}