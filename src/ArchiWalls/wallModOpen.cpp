#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMeshData.h>
#include <maya/MSelectionList.h>
#include <maya/MArgList.h>

#include "./wallModOpen.hpp"

MTypeId ArchiWallOpenNode::id(0x13002);
MObject ArchiWallOpenNode::widthAttr;
MObject ArchiWallOpenNode::heightAttr;
MObject ArchiWallOpenNode::inputMeshAttr;
MObject ArchiWallOpenNode::outputMeshAttr;

MStatus ArchiWallOpenNode::initialize() {
	MFnTypedAttribute tAttr;

	widthAttr = tAttr.create("width", "w", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(widthAttr);

	heightAttr = tAttr.create("height", "h", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(heightAttr);

	inputMeshAttr = tAttr.create("inputMesh", "inMesh", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(true);
	addAttribute(inputMeshAttr);

	outputMeshAttr = tAttr.create("outputMesh", "outMesh", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	addAttribute(outputMeshAttr);

	attributeAffects(widthAttr, outputMeshAttr);
	attributeAffects(heightAttr, outputMeshAttr);
	attributeAffects(inputMeshAttr, outputMeshAttr);

	return MS::kSuccess;
}

MStatus ArchiWallOpenNode::compute(const MPlug& plug, MDataBlock& data) {
	if (plug != outputMeshAttr) {
		return MS::kUnknownParameter;
	}

	MGlobal::displayInfo("WallModOpen compute executed");

	MStatus status;
	MDataHandle inputMeshHandle = data.inputValue(inputMeshAttr);
	MObject inMesh = inputMeshHandle.asMesh();

	MFnMesh meshFn(inMesh);
	MPointArray points;
	meshFn.getPoints(points);

	MDataHandle outputHandle = data.outputValue(outputMeshAttr);
	MFnMeshData dataCreator;
	MObject outMesh = dataCreator.create();

	return MS::kSuccess;
}

MStatus WallModOpenCmd::doIt(const MArgList& args) {
	MGlobal::displayInfo("WallModOpen command executed");

	MStatus status;
	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);

	if (selection.length() != 1) {
		MGlobal::displayError("Please select one mesh object");
		return MS::kFailure;
	}

	auto getShapeNodeFromTransform = [](MObject& transformObj, MStatus& status) -> MObject {
		MFnDagNode dagNodeFn(transformObj, &status);
		for (unsigned int i = 0; i < dagNodeFn.childCount(); i++) {
			MObject child = dagNodeFn.child(i, &status);
			if (child.hasFn(MFn::kMesh)) {
				return child;
			}
		}
		return MObject::kNullObj;
	};

	MObject inputTransformObj;
	status = selection.getDependNode(0, inputTransformObj);
	if (status != MS::kSuccess || !inputTransformObj.hasFn(MFn::kTransform)) {
		MGlobal::displayError("Failed to get the selected object");
		return MS::kFailure;
	}

	MFnDependencyNode fn; // Creating the construction history node
	MObject wallNodeObj = fn.create(ArchiWallOpenNode::id, "ArchiWallOpenNode", &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	MPlug inputMeshPlug = fn.findPlug("inMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MObject inputMeshObj = getShapeNodeFromTransform(inputTransformObj, status);
	if (status != MS::kSuccess) {
		MGlobal::displayError("Failed to get the shape node from the selected object");
		return MS::kFailure;
	}

	MFnDagNode meshDagNode(inputMeshObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	MPlug outMeshPlug = meshDagNode.findPlug("outMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	MDGModifier dgMod;
	dgMod.connect(outMeshPlug, inputMeshPlug); CHECK_MSTATUS_AND_RETURN_IT(status);
	
	status = dgMod.doIt(); CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}