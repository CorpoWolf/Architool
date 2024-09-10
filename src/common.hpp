#pragma once

#define mErr(status) CHECK_MSTATUS_AND_RETURN_IT(status)

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
#include <maya/MFnTransform.h>
#include <maya/MFnSet.h>