#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MFnSet.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnDependencyNode.h>

class PotatoPlaneCommand : public MPxCommand {
public:
	PotatoPlaneCommand() {}
	virtual MStatus doIt(const MArgList& args);
	static void* creator() { return new PotatoPlaneCommand(); }
};

MStatus PotatoPlaneCommand::doIt(const MArgList& args) {
	MStatus status;

	MFloatPointArray points;
	points.append(MFloatPoint(-30.48f, 0.0f, -30.48f)); // bottom left
    points.append(MFloatPoint(-30.48f, 0.0f,  30.48f)); // top left
    points.append(MFloatPoint( 30.48f, 0.0f,  30.48f)); // top right
    points.append(MFloatPoint( 30.48f, 0.0f, -30.48f)); // bottom right

	MIntArray faceCounts;
	faceCounts.append(4);

	MIntArray faceConnects;
	faceConnects.append(0);
	faceConnects.append(1);
	faceConnects.append(2);
	faceConnects.append(3);

	MFnMesh meshFn;
	MObject mesh = meshFn.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, MObject::kNullObj, &status);
	if (!status) {
		MGlobal::displayError("Failed to create the PotatoPlane mesh.");
		return status;
	}

	MFnDagNode dagNode(mesh);
	dagNode.setName("PotatoPlane");

	MSelectionList selectionList;
    selectionList.add("initialShadingGroup");
    MObject shadingGroupNode;
    selectionList.getDependNode(0, shadingGroupNode);

    if (shadingGroupNode.isNull()) {
        MGlobal::displayError("initialShadingGroup shading group not found. Make sure it exists in the scene.");
        return MStatus::kFailure;
    }

	MFnSet shadingGroup(shadingGroupNode);
    shadingGroup.addMember(mesh);

    MGlobal::displayInfo("PotatoPlane created successfully with initialShadingGroup Shading Group assigned!");

    return MS::kSuccess;
}

MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "PotatoPlugin", "1.0", "Any");
	return plugin.registerCommand("createPotatoPlane", PotatoPlaneCommand::creator);
}

MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	return plugin.deregisterCommand("createPotatoPlane");
}