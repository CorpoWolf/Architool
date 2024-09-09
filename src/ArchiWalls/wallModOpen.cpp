#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatVectorArray.h>
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
	tAttr.setWritable(true);
	tAttr.setReadable(true); // Needed for mesh recursion computation with other nodes
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

	if (inMesh.isNull()) {
		MGlobal::displayError("Input mesh is null");
		return MS::kFailure;
	}

	MFnMesh meshFn(inMesh);
	MPointArray vertices;
	meshFn.getPoints(vertices);

	MIntArray faceCounts;
	MIntArray faceConnects;
	meshFn.getVertices(faceCounts, faceConnects);

	// testing for output mesh modification
	for (unsigned int i = 0; i < vertices.length(); i++) {
		vertices[i].y += 1.0f;
	}

	MDataHandle outputHandle = data.outputValue(outputMeshAttr);
	MFnMeshData dataCreator;
	MObject outMesh = dataCreator.create(&status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnMesh newMeshFn;
	newMeshFn.create(
		vertices.length(), 
		faceCounts.length(), 
		vertices,
		faceCounts,
		faceConnects,
		outMesh,
		&status
	); CHECK_MSTATUS_AND_RETURN_IT(status);

	newMeshFn.updateSurface(); // Update the mesh to reflect the changes

	MFloatVectorArray floatNormals;
	int vertexIndexOffset = 0;

	for (unsigned int polyIdx = 0; polyIdx < faceCounts.length(); polyIdx++) { // Retrieve the normals for this polygon
		status = meshFn.getFaceVertexNormals(polyIdx, floatNormals, MSpace::kObject); // Get per-vertex per-polygon normals
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Set the normals for each vertex in this polygon
		for (unsigned int vertexIdx = 0; vertexIdx < faceCounts[polyIdx]; vertexIdx++) {
			int faceVertexIdx = faceConnects[vertexIndexOffset + vertexIdx];
			MVector normal(floatNormals[vertexIdx]);  // Convert MFloatVector to MVector
			status = newMeshFn.setFaceVertexNormal(normal, polyIdx, faceVertexIdx, MSpace::kObject);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		vertexIndexOffset += faceCounts[polyIdx];
	}

	outputHandle.set(outMesh);
	outputHandle.setClean();

	return MS::kSuccess;
}

MStatus WallModOpenCmd::doIt(const MArgList& args) {
	MStatus status;
	MGlobal::displayInfo("WallModOpen command executed");

	MSelectionList selList;
	MGlobal::getActiveSelectionList(selList);

	if (selList.length() != 1) {
		MGlobal::displayError("Select one mesh object");
		return MS::kFailure;
	}

	MObject inputTransfromObj;
	status = selList.getDependNode(0, inputTransfromObj);
	if (status != MS::kSuccess || inputTransfromObj.isNull()) {
		MGlobal::displayError("Failed to get selected object");
		return status;
	}

	MObject shapeNodeObj;
	MFnDagNode transformDagNode(inputTransfromObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	for (unsigned int i = 0; i < transformDagNode.childCount(); i++) {
		MObject childObj = transformDagNode.child(i, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
		if (childObj.hasFn(MFn::kMesh)) {
			shapeNodeObj = childObj;
			break;
		}
	}

	if (shapeNodeObj.isNull()) {
		MGlobal::displayError("Failed to get shape node object");
		return MS::kFailure;
	}

	MPlug inMeshPlug = MFnDagNode(shapeNodeObj).findPlug("inMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlugArray plugArray;
	inMeshPlug.connectedTo(plugArray, true, false, &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	if (plugArray.length() == 0) {
		MGlobal::displayError("No connection found");
		return MS::kFailure;
	}

	MObject constructionNode = plugArray[0].node(&status); CHECK_MSTATUS_AND_RETURN_IT(status);
	if (constructionNode.isNull()) {
		MGlobal::displayError("Failed to get construction node");
		return MS::kFailure;
	}

	MFnDependencyNode fn;
	MObject wallNodeOpenMod = fn.create(ArchiWallOpenNode::id, "ArchiWallOpenNode", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnDependencyNode constructionNodeFn(constructionNode, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	MPlug constructionOutPlug = constructionNodeFn.findPlug("outputMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	MPlug customInMeshPlug = fn.findPlug("inMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);

	MDGModifier dgMod;
	dgMod.connect(constructionOutPlug, customInMeshPlug); CHECK_MSTATUS_AND_RETURN_IT(status);

	// New Step 7
	if (inMeshPlug.isConnected()) {
		MPlugArray connectedPlugs;
		inMeshPlug.connectedTo(connectedPlugs, true, false, &status); CHECK_MSTATUS_AND_RETURN_IT(status);

		if (connectedPlugs.length() > 0) {
			dgMod.disconnect(connectedPlugs[0], inMeshPlug); CHECK_MSTATUS_AND_RETURN_IT(status);
		}
	}

	inMeshPlug.setLocked(false);  // Unlock the inMesh attribute to make it writable, if it's locked

	MPlug customOutMeshPlug = fn.findPlug("outMesh", &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	dgMod.connect(customOutMeshPlug, inMeshPlug); CHECK_MSTATUS_AND_RETURN_IT(status);

	status = dgMod.doIt(); CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}