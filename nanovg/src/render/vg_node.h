#ifndef VG_NODE_H
#define VG_NODE_H

#include <vector>
#include <string>
#include "nanovg.h"

using namespace std;

class VGNode
{
public:
	VGNode();

	virtual void render(NVGcontext*);

	string name;
	vector<VGNode> children;
};

#endif // VG_NODE_H
