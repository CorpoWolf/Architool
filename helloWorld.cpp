#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>

class helloWorld : public MPxCommand
{
	public:
		MStatus doIt( const MArgList& args );
		static void* creator();
};

MStatus helloWorld::doIt( const MArgList& args ) {
	if (args.length() > 0) {
		MGlobal::displayInfo("Hello World " + args.asString(0));
	} else {
		MGlobal::displayInfo("Hello World!");
	}
	return MS::kSuccess;
}

void* helloWorld::creator() {
	return new helloWorld;
}

MStatus initializePlugin( MObject obj ) {
	MFnPlugin plugin( obj, "Autodesk", "1.0", "Any" );
	plugin.registerCommand("HelloWorld", helloWorld::creator);
	return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj ) {
	MFnPlugin plugin( obj );
	plugin.deregisterCommand("HelloWorld");
	return MS::kSuccess;
}
