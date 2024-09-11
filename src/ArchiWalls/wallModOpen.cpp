#include "common.hpp"
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
	tAttr.setReadable(true); // Make the output attribute readable for node input and output.
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
	MObject outMesh = dataCreator.create(&status); mErr(status);

	MFnMesh newMeshFn;
	newMeshFn.create(
		vertices.length(), 
		faceCounts.length(), 
		vertices,
		faceCounts,
		faceConnects,
		outMesh,
		&status
	); mErr(status);

	newMeshFn.updateSurface(); // Update the mesh to reflect the changes

	MFloatVectorArray floatNormals;
	int vertexIndexOffset = 0;

	for (unsigned int polyIdx = 0; polyIdx < faceCounts.length(); polyIdx++) { // Retrieve the normals for this polygon
		status = meshFn.getFaceVertexNormals(polyIdx, floatNormals, MSpace::kObject); // Get per-vertex per-polygon normals
		mErr(status);

		// Set the normals for each vertex in this polygon
		for (unsigned int vertexIdx = 0; vertexIdx < faceCounts[polyIdx]; vertexIdx++) {
			int faceVertexIdx = faceConnects[vertexIndexOffset + vertexIdx];
			MVector normal(floatNormals[vertexIdx]);  // Convert MFloatVector to MVector
			status = newMeshFn.setFaceVertexNormal(normal, polyIdx, faceVertexIdx, MSpace::kObject);
			mErr(status);
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
	status = MGlobal::getActiveSelectionList(selList);

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
	MFnDagNode transformDagNode(inputTransfromObj, &status); mErr(status);

	for (unsigned int i = 0; i < transformDagNode.childCount(); i++) {
		MObject childObj = transformDagNode.child(i, &status); mErr(status);
		if (childObj.hasFn(MFn::kMesh)) {
			shapeNodeObj = childObj;
			break;
		}
	}

	if (shapeNodeObj.isNull()) {
		MGlobal::displayError("Failed to get shape node object");
		return MS::kFailure;
	}

	MPlug inMeshPlug = MFnDagNode(shapeNodeObj).findPlug("inMesh", &status); mErr(status);

	MPlugArray plugArray;
	inMeshPlug.connectedTo(plugArray, true, false, &status); mErr(status);

	if (plugArray.length() == 0) {
		MGlobal::displayError("No connection found");
		return MS::kFailure;
	}

	MObject constructionNode = plugArray[0].node(&status); mErr(status);
	if (constructionNode.isNull()) {
		MGlobal::displayError("Failed to get construction node");
		return MS::kFailure;
	}

	MFnDependencyNode fn;
	MObject wallNodeOpenMod = fn.create(ArchiWallOpenNode::id, "ArchiWallOpenNode", &status); mErr(status);

	MFnDependencyNode constructionNodeFn(constructionNode, &status); mErr(status);
	MPlug constructionOutPlug = constructionNodeFn.findPlug("outputMesh", &status); mErr(status);
	MPlug customInMeshPlug = fn.findPlug("inMesh", &status); mErr(status);

	MDGModifier dgMod;
	dgMod.connect(constructionOutPlug, customInMeshPlug); mErr(status);
	// New Step 7
	if (inMeshPlug.isConnected()) {
		MPlugArray connectedPlugs;
		inMeshPlug.connectedTo(connectedPlugs, true, false, &status); mErr(status);

		if (connectedPlugs.length() > 0) {
			dgMod.disconnect(connectedPlugs[0], inMeshPlug); mErr(status);
		}
	}

	inMeshPlug.setLocked(false);  // Unlock the inMesh attribute to make it writable, if it's locked

	MPlug customOutMeshPlug = fn.findPlug("outMesh", &status); mErr(status);
	dgMod.connect(customOutMeshPlug, inMeshPlug); mErr(status);

	status = dgMod.doIt(); mErr(status);

	return MS::kSuccess;
}