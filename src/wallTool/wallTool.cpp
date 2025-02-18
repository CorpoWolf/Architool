#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
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

#include <vector>
#include <format>

#include "./wallTool.h"
#include "../imprLib/imprMath.h"
#include "../imprLib/imprString.h"

MTypeId ArchiWallNode::id(0x13002);
MObject ArchiWallNode::widthAttr;
MObject ArchiWallNode::heightAttr;
MObject ArchiWallNode::depthAttr;
MObject ArchiWallNode::outputMeshAttr;
MObject ArchiWallNode::transformNodeAttr;

MStatus ArchiWallNode::initialize() {
	MFnTypedAttribute tAttr;

	widthAttr = tAttr.create("width", "w", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(widthAttr);

	heightAttr = tAttr.create("height", "h", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(heightAttr);

	depthAttr = tAttr.create("depth", "d", MFnData::kString);
	tAttr.setKeyable(true);
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

	std::string Rwidth = data.inputValue(widthAttr).asString().asChar(); // Step 1: Get the input values
	std::string Rheight = data.inputValue(heightAttr).asString().asChar();
	std::string Rdepth = data.inputValue(depthAttr).asString().asChar();

	double width = imprLib::strFtIn(Rwidth).Ft * 12 + imprLib::strFtIn(Rwidth).Inch; // Step 2: Convert the input values to inches
	MGlobal::displayInfo(std::format("Width: {}", width).c_str());
	double height = imprLib::strFtIn(Rheight).Ft * 12 + imprLib::strFtIn(Rheight).Inch;
	MGlobal::displayInfo(std::format("Height: {}", height).c_str());
	double depth = imprLib::strFtIn(Rdepth).Ft * 12 + imprLib::strFtIn(Rdepth).Inch;
	MGlobal::displayInfo(std::format("Depth: {}", depth).c_str());

	MFnMeshData meshDataFn;
	MObject wallMeshData = meshDataFn.create(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPointArray points;
	points.append(imprLib::toInchMPoint(-width / 2, 0, -depth / 2));
	points.append(imprLib::toInchMPoint(width / 2, 0, -depth / 2));
	points.append(imprLib::toInchMPoint(width / 2, height, -depth / 2));
	points.append(imprLib::toInchMPoint(-width / 2, height, -depth / 2));
	points.append(imprLib::toInchMPoint(-width / 2, 0, depth / 2));
	points.append(imprLib::toInchMPoint(width / 2, 0, depth / 2));
	points.append(imprLib::toInchMPoint(width / 2, height, depth / 2));
	points.append(imprLib::toInchMPoint(-width / 2, height, depth / 2));

	MIntArray faceCounts;
	MIntArray faceConnects;
	faceCounts.append(4); // Front face
	faceConnects.append(4);
	faceConnects.append(5);
	faceConnects.append(6);
	faceConnects.append(7);
	faceCounts.append(4); // Back face
	faceConnects.append(1);
	faceConnects.append(0);
	faceConnects.append(3);
	faceConnects.append(2);
	faceCounts.append(4); // Left face
	faceConnects.append(0);
	faceConnects.append(4);
	faceConnects.append(7);
	faceConnects.append(3);
	faceCounts.append(4); // Right face
	faceConnects.append(5);
	faceConnects.append(1);
	faceConnects.append(2);
	faceConnects.append(6);
	faceCounts.append(4); // Top face
	faceConnects.append(7);
	faceConnects.append(6);
	faceConnects.append(2);
	faceConnects.append(3);
	faceCounts.append(4); // Bottom face
	faceConnects.append(0);
	faceConnects.append(1);
	faceConnects.append(5);
	faceConnects.append(4);

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

	std::vector<int> edgeIndices;
	for (int i = 0; i < meshFn.numEdges(); i++) {
		edgeIndices.push_back(i);
	}
	for (int edgeIndex : edgeIndices) {
		status = meshFn.setEdgeSmoothing(edgeIndex, false);  // Mark the edge as hard
		CHECK_MSTATUS_AND_RETURN_IT(status);
	}
	meshFn.updateSurface(); // Apply changes to mesh surface

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
	MObject wallNodeObj = fn.create(ArchiWallNode::id, "ArchiWallNode", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug widthPlug = fn.findPlug("width", true);
	widthPlug.setString("2'0\"");

	MPlug heightPlug = fn.findPlug("height", true);
	heightPlug.setString("2'0\"");

	MPlug depthPlug = fn.findPlug("depth", true);
	depthPlug.setString("2'0\"");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnTransform transformFn; // Creating the transform node
	MObject transformObj = transformFn.create(MObject::kNullObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	transformFn.setName("ArchiWall");

	MFnDagNode dagNodeFn; // Creating the shape node
	MObject shapeObj = dagNodeFn.create("mesh", transformObj, &status);
	dagNodeFn.setName("ArchiWallShape");
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

	MSelectionList selList;
	selList.add("initialShadingGroup");
	MObject shadingGroupObj;
	selList.getDependNode(0, shadingGroupObj);

	MFnSet shadingGroup(shadingGroupObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = shadingGroup.addMember(shapeObj);  // Add the shape node to the initialShadingGroup
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::displayInfo("Wall node created, mesh generated, and assigned to shading group!");
	return MS::kSuccess;
}