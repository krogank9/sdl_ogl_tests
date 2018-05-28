#include "vg_node.h"

VGNode::VGNode()
{

}

void VGNode::render(NVGcontext* ctx)
{
	vector<VGNode>::iterator it = children.begin();
	for (;it != children.end(); it++)
		it->render(ctx);
}
