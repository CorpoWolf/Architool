#include <maya/MGlobal.h>
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
#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>

#include "./wallTool.h"

MStatus WallCreateCmd::doIt(const MArgList& args) {
	MStatus status;

	MFloatPointArray points;
	points.append(MFloatPoint(-30.48f, 0.0f, -30.48f)); // back left
	points.append(MFloatPoint(-30.48f, 0.0f,  30.48f)); // front left
	points.append(MFloatPoint( 30.48f, 0.0f,  30.48f)); // front right
	points.append(MFloatPoint( 30.48f, 0.0f, -30.48f)); // back right

	points.append(MFloatPoint(-30.48f, 60.96f, -30.48f)); // above back left
	points.append(MFloatPoint(30.48f, 60.96f, -30.48f));  // above back right

	MIntArray faceCounts;
	faceCounts.append(4); //floor
	faceCounts.append(4); //back wall

	MIntArray faceConnects;
	faceConnects.append(0);
	faceConnects.append(1);
	faceConnects.append(2);
	faceConnects.append(3);

	faceConnects.append(4); // above back left
	faceConnects.append(0); // back left
	faceConnects.append(3); // back right
	faceConnects.append(5); // above back right

	MFnMesh meshFn;
	MObject mesh = meshFn.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, MObject::kNullObj, &status);
	if (!status) {
		MGlobal::displayError("Architools: Failed to create mesh.");
		return status;
	}

	MFnDagNode dagNode(mesh);
	dagNode.setName("ArchiWallTest");

	MSelectionList selectionList;
	selectionList.add("initialShadingGroup");
	MObject shadingGroupNode;
	selectionList.getDependNode(0, shadingGroupNode);

	if (shadingGroupNode.isNull()) {
		MGlobal::displayError("Architools: initialShadingGroup shading group not found. Make sure it exists in the scene.");
		return MStatus::kFailure;
	}

	MFnSet shadingGroup(shadingGroupNode);
	shadingGroup.addMember(mesh);

	MGlobal::displayInfo("Architools: mesh created successfully with initialShadingGroup Shading Group assigned!");

	return MS::kSuccess;
}

MStatus WallNode::initialize() {
    MFnNumericAttribute nAttr;

    widthAttr = nAttr.create("width", "w", MFnNumericData::kFloat, 5.0);
    nAttr.setKeyable(true);
    addAttribute(widthAttr);

    heightAttr = nAttr.create("height", "h", MFnNumericData::kFloat, 3.0);
    nAttr.setKeyable(true);
    addAttribute(heightAttr);

    return MS::kSuccess;
}

MTypeId WallNode::id(0x00002);
MObject WallNode::widthAttr;
MObject WallNode::heightAttr;