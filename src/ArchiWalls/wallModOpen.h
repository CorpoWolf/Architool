// Create wall openings for door and windows

class WallModOpen : public MPxCommand {
public:
	WallModOpen() {}
	virtual MStatus doIt(const MArgList& args);
	static void* creator() { return new WallModOpen(); }
};

class ArchiWallOpenNode : public MPxNode {
public:
	static MTypeId id; // set to id(0x13002)
	static MObject widthAttr;
	static MObject heightAttr;
	static MObject outputMeshAttr;
	ArchiWallOpenNode() {}
	static void* creator() { return new ArchiWallOpenNode(); }
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};