## Create Mesh
```cpp
MFnMesh meshFn;
MObject mesh = meshFn.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, MObject::kNullObj, &status);
if (!status) {
	MGlobal::displayError("Architools: Failed to create mesh.");
	return status;
}
```

## Create Transform Node
```cpp
MFnTransform transformFn; // Create the transform node first.
MObject transformNode = transformFn.create(MObject::kNullObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
transformFn.setName("MyPotatoCubeOBJ", &status); CHECK_MSTATUS_AND_RETURN_IT(status);
```

## Code to look at
```cpp
MPlugArray connections;
	outputMeshPlug.connectedTo(connections, false, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	MObject outputMeshObj;
	status = outputMeshPlug.getValue(outputMeshObj);
	if (!status) {
		MGlobal::displayError("Failed to get outputMesh value");
		return status;
	}

	// MFnDagNode dagNode(outputMeshObj, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	// if (!outputMeshObj.hasFn(MFn::kDagNode)) {
	// 	MGlobal::displayError("outputMeshObj is not a DAG node.");
	// 	return MS::kFailure;
	// }
	for (unsigned int i = 0; i < connections.length(); ++i) {
    MPlug connectedPlug = connections[i];
    MObject connectedNode = connectedPlug.node();

    if (connectedNode.hasFn(MFn::kDagNode)) {
			MFnDagNode dagNode(connectedNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Rename the node
			dagNode.setName("PotatoNode");  // Replace with your desired name
    }
	}

	MSelectionList selectionList;
	status = selectionList.add("initialShadingGroup"); CHECK_MSTATUS_AND_RETURN_IT(status);
	MObject shadingGroupNode;
	status = selectionList.getDependNode(0, shadingGroupNode); CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::displayInfo("Post selectionList for shader");

	if (shadingGroupNode.isNull()) {
		MGlobal::displayError("Architools: initialShadingGroup shading group not found. Make sure it exists in the scene.");
		return MStatus::kFailure;
	}

	if (shapeNodeObj.hasFn(MFn::kMesh)) {
    MFnDagNode dagNode(shapeNodeObj, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = shadingGroup.addMember(shapeNodeObj); 
    CHECK_MSTATUS_AND_RETURN_IT(status);
} else {
		MGlobal::displayError("Invalid object type for shading group assignment");
		return MS::kFailure;
}

	MFnSet shadingGroup(shadingGroupNode, &status); CHECK_MSTATUS_AND_RETURN_IT(status);
	status = shadingGroup.addMember(outputMeshObj); CHECK_MSTATUS_AND_RETURN_IT(status);
```