#include <maya/MPxCommand.h>
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>

MTypeId ArchiWallOpenNode::id(0x13002);
MObject ArchiWallOpenNode::widthAttr;
MObject ArchiWallOpenNode::heightAttr;
MObject ArchiWallOpenNode::outputMeshAttr;