#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTypedAttribute.h>

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
	tAttr.setWritable(false);
	addAttribute(inputMeshAttr);

	outputMeshAttr = tAttr.create("outputMesh", "outMesh", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	addAttribute(outputMeshAttr);

	attributeAffects(widthAttr, outputMeshAttr);
	attributeAffects(heightAttr, outputMeshAttr);

	MDataHandle outputHandle = dataBlock.outputValue(outputMesh);
	MFnMeshData dataCreator;
	MObject outMesh = dataCreator.create();



	return MS::kSuccess;
}

MStatus ArchiWallOpenNode::compute(const MPlug& plug, MDataBlock& data) {
	if (plug != outputMeshAttr) {
		return MS::kUnknownParameter;
	}

	MStatus status;
	MDataHandle inputMeshHandle = data.inputValue(inputMesh);
	MObject inMesh = inputMeshHandle.asMesh();

	MFnMesh meshFn(inMesh);
	MPointArray points;
	meshFn.getPoints(points);

	MObject newMesh = meshFn.create( // Going to need verification - placeholder for now
		points.length(),
		faceCounts.length(),
		points,
		faceCounts,
		faceConnects,
		wallMeshData,
		&status
	);

	return MS::kSuccess;
}

MStatus WallModOpenCmd::doIt(const MArgList& args) {
	MGlobal::displayInfo("WallModOpen command executed");

	return MS::kSuccess;
}