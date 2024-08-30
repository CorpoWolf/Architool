# Create Mesh
```cpp
MFnMesh meshFn;
MObject mesh = meshFn.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, MObject::kNullObj, &status);
if (!status) {
	MGlobal::displayError("Architools: Failed to create mesh.");
	return status;
}
```
