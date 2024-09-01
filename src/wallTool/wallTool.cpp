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

	MFnMeshData meshDataFn;
	MObject wallMeshData = meshDataFn.create(&status);
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
	// Front face (Vertices: 0, 1, 2, 3)
	faceCounts.append(4);
	faceConnects.append(0);
	faceConnects.append(1);
	faceConnects.append(2);
	faceConnects.append(3);
	// Back face (Vertices: 4, 7, 6, 5)
	faceCounts.append(4);
	faceConnects.append(4);
	faceConnects.append(7);
	faceConnects.append(6);
	faceConnects.append(5);
	// Left face (Vertices: 0, 3, 7, 4)
	faceCounts.append(4);
	faceConnects.append(0);
	faceConnects.append(3);
	faceConnects.append(7);
	faceConnects.append(4);
	// Right face (Vertices: 1, 5, 6, 2)
	faceCounts.append(4);
	faceConnects.append(1);
	faceConnects.append(5);
	faceConnects.append(6);
	faceConnects.append(2);
	// Top face (Vertices: 3, 2, 6, 7)
	faceCounts.append(4);
	faceConnects.append(3);
	faceConnects.append(2);
	faceConnects.append(6);
	faceConnects.append(7);
	// Bottom face (Vertices: 0, 4, 5, 1)
	faceCounts.append(4); 
	faceConnects.append(0);
	faceConnects.append(4);
	faceConnects.append(5);
	faceConnects.append(1);

	MFnMesh meshFn; // Create a new MFnMesh object that stores the mesh data but does not create the mesh
	MObject newMesh = meshFn.create(
		points.length(), 
		faceCounts.length(), 
		points, 
		faceCounts, 
		faceConnects, 
		wallMeshData, 
		&status
	);
	if (status != MS::kSuccess || newMesh.isNull()) {
		MGlobal::displayError("Failed to create mesh or mesh is null");
		return status;
	}

	MDataHandle outputHandle = data.outputValue(outputMeshAttr); 
	if (wallMeshData.isNull()) {
		MGlobal::displayError("wallMeshData is null before setting output.");
		return MS::kFailure;
	}
	outputHandle.set(wallMeshData);
	data.setClean(plug);

	MGlobal::displayInfo("Architools: Mesh Update!");
	return MS::kSuccess;
}

MStatus WallCreateCmd::doIt(const MArgList& args) {
	MStatus status;

	MGlobal::displayInfo("Running Wall Node doIt method");

	MFnDependencyNode fn; // Creating the construction history node
	MObject wallNodeObj = fn.create(ArchiWallNode::id, "WallNode", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug widthPlug = fn.findPlug("width", true);
	widthPlug.setFloat(60.96f);

	MPlug heightPlug = fn.findPlug("height", true);
	heightPlug.setFloat(60.96f); 

	MPlug depthPlug = fn.findPlug("depth", true);
	depthPlug.setFloat(60.96f);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnTransform transformFn; // Creating the transform node
	MObject transformObj = transformFn.create(MObject::kNullObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnDagNode dagNodeFn; // Creating the shape node
	MObject shapeObj = dagNodeFn.create("mesh", transformObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnDependencyNode shapeFn(shapeObj); // Connect the output mesh of WallNode to the input of the shape node
	MPlug inMeshPlug = shapeFn.findPlug("inMesh", true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug outputMeshPlug = fn.findPlug("outputMesh", true, &status); // Runs the compute method
	if (!status) {
		MGlobal::displayError("Failed to find outputMesh plug");
		return status;
	}

	MDGModifier dgModifier;
	dgModifier.connect(outputMeshPlug, inMeshPlug);
	CHECK_MSTATUS_AND_RETURN_IT(dgModifier.doIt());
	
	MObject outputMeshObj;
	status = outputMeshPlug.getValue(outputMeshObj); // Delivers the output of the compute method
	if (!status) {
		MGlobal::displayError("Failed to get outputMesh value");
		return status;
	}

	MGlobal::displayInfo("Wall node created, mesh generated, and assigned to shading group!");
	return MS::kSuccess;
}