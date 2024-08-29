#pragma once
#include <maya/MPxCommand.h>

class WallCreateCmd : public MPxCommand {
public:
    WallCreateCmd() {}
    virtual MStatus doIt(const MArgList& args);
    static void* creator() { return new WallCreateCmd(); }
};