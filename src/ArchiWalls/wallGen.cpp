#include "common.hpp"

#include <vector>
#include <format>
#include <cstdint>

#include "./wallGen.hpp"
#include "../imprLib/imprMath.hpp"
#include "../imprLib/imprString.hpp"
#include "../utils/utils.hpp"

MTypeId ArchiWallNode::id(0x13001);
MObject ArchiWallNode::widthAttr;
MObject ArchiWallNode::heightAttr;
MObject ArchiWallNode::depthAttr;
MObject ArchiWallNode::wallTypeAttr;
MObject ArchiWallNode::outputMeshAttr;

MStatus ArchiWallNode::initialize() {
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;

	widthAttr = tAttr.create("width", "w", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(widthAttr);

	heightAttr = tAttr.create("height", "h", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(heightAttr);

	depthAttr = tAttr.create("depth", "d", MFnData::kString);
	tAttr.setKeyable(true);
	addAttribute(depthAttr);

	wallTypeAttr = eAttr.create("wallType", "wt", 0);
	eAttr.addField("Flat Wall", 0);
	eAttr.addField("Corner Wall", 1);
	eAttr.addField("Nook Wall", 2);
	eAttr.setKeyable(true);
	addAttribute(wallTypeAttr);

	outputMeshAttr = tAttr.create("outputMesh", "out", MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	addAttribute(outputMeshAttr);

	// Define dependencies between inputs and outputs
	attributeAffects(widthAttr, outputMeshAttr);
	attributeAffects(heightAttr, outputMeshAttr);
	attributeAffects(depthAttr, outputMeshAttr);
	attributeAffects(wallTypeAttr, outputMeshAttr);

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
	int wallType = data.inputValue(wallTypeAttr).asShort();

	double width = imprLib::strFtIn(Rwidth).Ft * 12 + imprLib::strFtIn(Rwidth).Inch; // Step 2: Convert the input values to inches
	MGlobal::displayInfo(std::format("Width: {}", width).c_str());
	double height = imprLib::strFtIn(Rheight).Ft * 12 + imprLib::strFtIn(Rheight).Inch;
	MGlobal::displayInfo(std::format("Height: {}", height).c_str());
	double depth = imprLib::strFtIn(Rdepth).Ft * 12 + imprLib::strFtIn(Rdepth).Inch;

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
	faceCounts.append(4); // Bottom face
	faceConnects.append(0);
	faceConnects.append(1);
	faceConnects.append(5);
	faceConnects.append(4);
	faceCounts.append(4); // Top face
	faceConnects.append(7);
	faceConnects.append(6);
	faceConnects.append(2);
	faceConnects.append(3);
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
	faceCounts.append(4); // Front face
	faceConnects.append(4);
	faceConnects.append(5);
	faceConnects.append(6);
	faceConnects.append(7);
	faceCounts.append(4); // Right face
	faceConnects.append(5);
	faceConnects.append(1);
	faceConnects.append(2);
	faceConnects.append(6);

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

MSyntax WallCreateCmd::newSyntax() {
	MSyntax syntax;
	syntax.addFlag("-w", "-width", MSyntax::kString);
	syntax.addFlag("-h", "-height", MSyntax::kString);
	syntax.addFlag("-d", "-depth", MSyntax::kString);
	syntax.addFlag("-wt", "-wallType", MSyntax::kLong);
	return syntax;
}

MStatus WallCreateCmd::doIt(const MArgList& args) {
	MStatus status;
	MFnDependencyNode fn;

	MObject wallNodeObj = fn.create(ArchiWallNode::id, "ArchiWallNode", &status); mErr(status);
	MArgDatabase argData(WallCreateCmd::newSyntax(), args, &status); mErr(status);
	
	std::vector<MString> defaultSize = { "20'0\"", "10'0\"", "0'4.75\"" };
	uint32_t defaultOpt = 0; 
	MPlug widthPlug = fn.findPlug("width", true);
	MPlug heightPlug = fn.findPlug("height", true);
	MPlug depthPlug = fn.findPlug("depth", true);
	MPlug wallTypePlug = fn.findPlug("wallType", true);

	archiUtil::CmdFlag(argData, "-w", defaultSize[0], widthPlug);
	archiUtil::CmdFlag(argData, "-h", defaultSize[1], heightPlug);
	archiUtil::CmdFlag(argData, "-d", defaultSize[2], depthPlug);
	archiUtil::CmdFlag(argData, "-wt", defaultOpt, wallTypePlug);

	MFnTransform transformFn;
	MObject transformObj = transformFn.create(MObject::kNullObj, &status); mErr(status);
	transformFn.setName("ArchiWall");

	MFnDagNode dagNodeFn; // Creating the shape node
	MObject shapeObj = dagNodeFn.create("mesh", transformObj, &status);
	dagNodeFn.setName("ArchiWallShape"); mErr(status);

	MFnDependencyNode shapeFn(shapeObj); // Connect the output mesh of WallNode to the input of the shape node
	MPlug inMeshPlug = shapeFn.findPlug("inMesh", true, &status); mErr(status);

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

	MFnSet shadingGroup(shadingGroupObj, &status); mErr(status);
	status = shadingGroup.addMember(shapeObj); mErr(status); // Add the shape node to the initialShadingGroup

	MGlobal::displayInfo("Wall node created, mesh generated, and assigned to shading group!");
	return MS::kSuccess;
}