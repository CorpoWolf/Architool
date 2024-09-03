# Types of attributes in Maya Nodes
In creating nodes for Maya there are multiple kinds of attributes for different data types.  

# MFnNumericAttribute
**Stuff**
To be able to use simply `#include <maya/MFnNumericAttribute.h>`
This node can be used for numeric types of data and:
- MFnNumericData::kBoolean
- MFnNumericData::kInt
- MFnNumericData::kFloat
- MFnNumericData::kDouble
- MFnNumericData::k3Float

# MFnTypedAttribute
**Stuff**
To be able to use simply `#include <maya/MFnTypedAttribute.h>`
This node can be used for string data and:
- MFnData::kMesh
- MFnData::kNurbsCurve
- MFnData::kString
- MFnData::kMatrix
- MFnData::kPointArray

# MFnEnumAttribute
**Create a drop-down attribute with a list of options**
To be able to use simply `#include <maya/MFnEnumAttribute.h>`
- Enum Fields: You can add multiple options to the drop-down by using addField. Each option is given a name and an associated integer value.
Example code:
```cpp
MFnEnumAttribute eAttr;

operationMode = eAttr.create("operationMode", "opMode");
eAttr.addField("Mode 1", 0);  // Add "Mode 1" with a value of 0
eAttr.addField("Mode 2", 1);  // Add "Mode 2" with a value of 1
eAttr.addField("Mode 3", 2);  // Add "Mode 3" with a value of 2
eAttr.setStorable(true);
eAttr.setKeyable(true);
addAttribute(operationMode);
```