
#include "kiwi/utils/Testing.hpp"
#include "kiwi/core/Node.hpp"
#include "kiwi/core/DataTypeManager.hpp"
#include "kiwi/core/NodeTypeManager.hpp"
#include "kiwi/core/InputPort.hpp"
#include "kiwi/core/OutputPort.hpp"
#include "kiwi/core/OpConnect.hpp"
#include "kiwi/core/Container.hpp"
#include "kiwi/extern/log/DebugStream.hpp"
#include "kiwi/mock/MockNodeUpdater.hpp"

using namespace kiwi;
using namespace kiwi::core;

struct Dummy{};
KIWI_DECLARE_CONTAINER(int,"Int")
KIWI_DECLARE_CONTAINER(Dummy,"Dummy")


int main()
{
	KIWI_BEGIN_TESTING("Kiwi::core::NodeTypeManager");


    auto IntInfo = DataTypeManager::RegisterDataType("Int", &Newint);
    auto DummyInfo = DataTypeManager::RegisterDataType("Dummmy", &NewDummy);


    NodeLayoutDescriptor layout1;
    layout1.inputs = {
        { "in1", IntInfo, READ },
        { "in2", IntInfo, READ }
    };
    layout1.outputs = {
        { "out", IntInfo, READ }
    };
    
    NodeLayoutDescriptor layout2;
    layout2.inputs  = { { "in" , IntInfo  , READ } };
    layout2.outputs = { { "out", DummyInfo, READ } };

    NodeTypeManager::RegisterNode("NodeTest1", layout1, new mock::MockNodeUpdater);
    NodeTypeManager::RegisterNode("NodeTest2", layout2, new mock::MockNodeUpdater);

    {   SCOPEDBLOCK("Type info state");
        auto info1 = NodeTypeManager::TypeOf("NodeTest1");
        auto info2 = NodeTypeManager::TypeOf("NodeTest2");
        KIWI_TEST("TypeOf(NodeTest1) not null.", info1 != 0 );
        KIWI_TEST("TypeOf(NodeTest2) not null.", info2 != 0 );
        KIWI_TEST("TypeOf(NodeTest1) name.", info1->name() == string("NodeTest1") );
        KIWI_TEST("TypeOf(NodeTest2) name.", info2->name() == string("NodeTest2") );
        KIWI_TEST_EQUAL("TypeOf(NodeTest1) nb inputs.", info1->inputs().size(), 2 );
        KIWI_TEST_EQUAL("TypeOf(NodeTest2) nb inputs.", info2->inputs().size(), 1 );
        KIWI_TEST_EQUAL("TypeOf(NodeTest1) nb outputs.", info1->outputs().size(), 1 );
        KIWI_TEST_EQUAL("TypeOf(NodeTest2) nb outputs.", info2->outputs().size(), 1 );

        KIWI_TEST("TypeOf(NodeTest1) input 0 type."
            , info1->inputs()[0].dataType() == IntInfo );
        KIWI_TEST("TypeOf(NodeTest1) input 1 type."
            , info1->inputs()[1].dataType() == IntInfo );
        KIWI_TEST("TypeOf(NodeTest2) input 0 type."
            , info2->inputs()[0].dataType() == IntInfo );

        KIWI_TEST("TypeOf(NodeTest1) output type."
            , info1->outputs()[0].dataType() == IntInfo );
        KIWI_TEST("TypeOf(NodeTest2) output type."
            , info2->outputs()[0].dataType() == DummyInfo );

        KIWI_TEST("TypeOf(NodeTest1) updater."
            , dynamic_cast<mock::MockNodeUpdater*>(info1->updater()) != 0 )    
        KIWI_TEST("TypeOf(NodeTest2) updater."
            , dynamic_cast<mock::MockNodeUpdater*>(info2->updater()) != 0 )    
    }

    {   SCOPEDBLOCK("Registration");
        auto info1 = NodeTypeManager::TypeOf("NodeTest1");
        auto info2 = NodeTypeManager::TypeOf("NodeTest2");

        KIWI_TEST( "Registering NodeTest1 twice"
            , NodeTypeManager::RegisterNode("NodeTest1", layout1, new mock::MockNodeUpdater) == info1);
        KIWI_TEST( "Registering NodeTest2 twice"
            , NodeTypeManager::RegisterNode("NodeTest2", layout1, new mock::MockNodeUpdater) == info2);
        KIWI_TEST( "Requesting unregistered node"
            , NodeTypeManager::TypeOf("some_unregistered_node") == 0);
        
        NodeTypeManager::Unregister("NodeTest1");

        KIWI_TEST( "Requesting NodeTest1 after unregistration"
            , NodeTypeManager::TypeOf("NodeTest1") == 0);

        info1 = NodeTypeManager::RegisterNode("NodeTest1", layout1, new mock::MockNodeUpdater);

        KIWI_TEST( "Registering NodeTest1 again"
            , info1->name() == "NodeTest1" );
    }
    
    Pipeline p(0,0,0);

    auto n1 = new Node(&p, NodeTypeManager::TypeOf("NodeTest1") );
    auto n2 = new Node(&p, NodeTypeManager::TypeOf("NodeTest2") );

    
    
    delete n1;
    delete n2;
    return KIWI_END_TESTING;
}
