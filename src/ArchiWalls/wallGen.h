#pragma once
#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>

class WallCreateCmd : public MPxCommand {
public:
	WallCreateCmd() {}
	virtual MStatus doIt(const MArgList& args);
	static void* creator() { return new WallCreateCmd(); }
};

class ArchiWallNode : public MPxNode {
public:
	static MTypeId id;
	static MObject widthAttr;
	static MObject heightAttr;
	static MObject depthAttr;
	static MObject outputMeshAttr;
	ArchiWallNode() {}
	static void* creator() { return new ArchiWallNode(); }
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};