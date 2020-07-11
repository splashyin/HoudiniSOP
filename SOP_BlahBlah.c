#include "SOP_BlahBlah.h"
#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <UT/UT_DSOVersion.h>

PRM_Template* blahParams()
{
    return nullptr;
}

void newSopOperator( OP_OperatorTable* i_table )
{
    i_table->addOperator(
        new OP_Operator(
            "blahblah", //name
            "Blah Blah", //display name
            []( OP_Network* net, const char* name, OP_Operator* op ) -> OP_Node*{
                return new SOP_BlahBlah( net, name, op );
            },          // Constructor
            blahParams(), // parameters
            0,          // min # of sources
            0,          // max # of sources
            nullptr,    // local variables
            OP_FLAG_GENERATOR // generates data
        )
    );
}

OP_ERROR SOP_BlahBlah::cookMySop( OP_Context& i_context )
{
    gdp->clearAndDestroy();
    return error();
}