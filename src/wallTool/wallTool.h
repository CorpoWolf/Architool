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

class WallNode : public MPxNode {
public:
	static MTypeId id;
	static MObject widthAttr;
	static MObject heightAttr;
	WallNode() {}
	static void* creator() { return new WallNode(); }
	static MStatus initialize();
};