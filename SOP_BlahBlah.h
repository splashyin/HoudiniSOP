#pragma once
#include <SOP/SOP_Node.h>

class SOP_BlahBlah : public SOP_Node
{
public:
    SOP_BlahBlah( OP_Network *net, const char *name, OP_Operator *op ) :
    SOP_Node( net, name, op ) {}

    virtual OP_ERROR cookMySop( OP_Context& context );
};