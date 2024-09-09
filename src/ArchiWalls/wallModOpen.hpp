#pragma once
#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>

class WallModOpenCmd : public MPxCommand {
public:
	WallModOpenCmd() {}
	virtual MStatus doIt(const MArgList& args);
	static void* creator() { return new WallModOpenCmd(); }
};

class ArchiWallOpenNode : public MPxNode {
public:
	static MTypeId id; // set to id(0x13002)
	static MObject widthAttr;
	static MObject heightAttr;
	static MObject inputMeshAttr;
	static MObject outputMeshAttr;
	ArchiWallOpenNode() {}
	static void* creator() { return new ArchiWallOpenNode(); }
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};