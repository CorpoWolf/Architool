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
#include <maya/MFnTypedAttribute.h>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>

#include "./wallTool.h"

MTypeId ArchiWallNode::id(0x13002);
MObject ArchiWallNode::widthAttr;
MObject ArchiWallNode::heightAttr;
MObject ArchiWallNode::depthAttr;
MObject ArchiWallNode::outputMeshAttr;
MObject ArchiWallNode::transformNodeAttr;

MStatus ArchiWallNode::initialize() {
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;

	widthAttr = nAttr.create("width", "w", MFnNumericData::kFloat, 1.0);
	nAttr.setKeyable(true);
	addAttribute(widthAttr);

	heightAttr = nAttr.create("height", "h", MFnNumericData::kFloat, 1.0);
	nAttr.setKeyable(true);
	addAttribute(heightAttr);

	depthAttr = nAttr.create("depth", "d", MFnNumericData::kFloat, 1.0);
	nAttr.setKeyable(true);
	addAttribute(depthAttr);

	outputMeshAttr = tAttr.create("outputMesh", "out", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	addAttribute(outputMeshAttr);

	transformNodeAttr = tAttr.create("transformNode", "tn", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(true);
	addAttribute(transformNodeAttr);

	// Define dependencies between inputs and outputs
	attributeAffects(widthAttr, outputMeshAttr);
	attributeAffects(heightAttr, outputMeshAttr);
	attributeAffects(depthAttr, outputMeshAttr);

	return MS::kSuccess;
}

MStatus ArchiWallNode::compute(const MPlug& plug, MDataBlock& data) {
	if (plug != outputMeshAttr) {
		return MS::kUnknownParameter;
	}

	MGlobal::displayInfo("ArchiWallNode::compute called");
	MStatus status;

	float width = data.inputValue(widthAttr).asFloat(); // Step 1: Get the input values
	float height = data.inputValue(heightAttr).asFloat();
	float depth = data.inputValue(depthAttr).asFloat();

	MFnMeshData meshDataFn; // Step 5: 
	MObject newMesh = meshDataFn.create(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPointArray points;
	points.append(MPoint(-width / 2, -height / 2, -depth / 2));
	points.append(MPoint(width / 2, -height / 2, -depth / 2));
	points.append(MPoint(width / 2, height / 2, -depth / 2));
	points.append(MPoint(-width / 2, height / 2, -depth / 2));
	points.append(MPoint(-width / 2, -height / 2, depth / 2));
	points.append(MPoint(width / 2, -height / 2, depth / 2));
	points.append(MPoint(width / 2, height / 2, depth / 2));
	points.append(MPoint(-width / 2, height / 2, depth / 2));

	MIntArray faceCounts;
	MIntArray faceConnects;
	faceCounts.append(4); faceConnects.append(0); faceConnects.append(1); faceConnects.append(2); faceConnects.append(3);  // Front face
	faceCounts.append(4); faceConnects.append(4); faceConnects.append(5); faceConnects.append(6); faceConnects.append(7);  // Back face
	faceCounts.append(4); faceConnects.append(0); faceConnects.append(3); faceConnects.append(7); faceConnects.append(4);  // Left face
	faceCounts.append(4); faceConnects.append(1); faceConnects.append(2); faceConnects.append(6); faceConnects.append(5);  // Right face
	faceCounts.append(4); faceConnects.append(3); faceConnects.append(2); faceConnects.append(6); faceConnects.append(7);  // Top face
	faceCounts.append(4); faceConnects.append(0); faceConnects.append(1); faceConnects.append(5); faceConnects.append(4);  // Bottom face

	// Create Shape Node
	MFnMesh meshFn;
	meshFn.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, MObject::kNullObj);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle outputHandle = data.outputValue(outputMeshAttr);
	outputHandle.set(newMesh);
	data.setClean(plug);

	MGlobal::displayInfo("Architools: Mesh Update!");
	return MS::kSuccess;
}

MStatus WallCreateCmd::doIt(const MArgList& args) {
	MStatus status;

	MGlobal::displayInfo("Running Wall Node doIt method");

	MFnTransform transformFn; // Create the transform node first.
	MObject transformNode = transformFn.create(MObject::kNullObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	transformFn.setName("MyPotatoCubeOBJ", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnDependencyNode fn; // Creating the construction history node
	MObject wallNodeObj = fn.create(ArchiWallNode::id, "WallNode", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug widthPlug = fn.findPlug("width", true);
	widthPlug.setFloat(60.96f);

	MPlug heightPlug = fn.findPlug("height", true);
	heightPlug.setFloat(60.96f); 

	MPlug depthPlug = fn.findPlug("depth", true);
	depthPlug.setFloat(60.96f);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug outputMeshPlug = fn.findPlug("outputMesh", true, &status);
	if (!status) {
		MGlobal::displayError("Failed to find outputMesh plug");
		return status;
	}
	MObject outputMeshObj;
	status = outputMeshPlug.getValue(outputMeshObj);
	if (!status) {
		MGlobal::displayError("Failed to get outputMesh value");
		return status;
	}

	MFnDagNode dagNode(outputMeshObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	if (!outputMeshObj.hasFn(MFn::kDagNode)) {
		MGlobal::displayError("outputMeshObj is not a DAG node.");
		return MS::kFailure;
	}
	MString newName = dagNode.setName("ArchiWallTest"); CHECK_MSTATUS_AND_RETURN_IT(status);
	if (newName != "ArchiWallTest") {
		MGlobal::displayError("Failed to set the name of the DAG node.");
		return MS::kFailure;
	}

	MSelectionList selectionList;
	status = selectionList.add("initialShadingGroup"); CHECK_MSTATUS_AND_RETURN_IT(status);
	MObject shadingGroupNode;
	status = selectionList.getDependNode(0, shadingGroupNode); CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::displayInfo("Post selectionList for shader");

	if (shadingGroupNode.isNull()) {
		MGlobal::displayError("Architools: initialShadingGroup shading group not found. Make sure it exists in the scene.");
		return MStatus::kFailure;
	}

	MFnSet shadingGroup(shadingGroupNode, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	status = shadingGroup.addMember(outputMeshObj); CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::displayInfo("Wall node created, mesh generated, and assigned to shading group!");
	return MS::kSuccess;
}