/++

	This Modules contains ways to easily create dummy data and nodes that accept any connections.
	This is mostly for testing other modules.

+/
module kiwi.utils.mock;

import kiwi.core.all;
import kiwi.dynamic.node;
import kiwi.dynamic.port;
import kiwi.dynamic.compatibility;

import std.conv;

/++
 + blanck data container that contains no data.
 +/
class MockData : Data
{
	static this()
	{
		_dataType = DataTypeManager.Register!MockData();
	}
	override
	{
		@property
		{
			DataTypeInfo type(){ return _dataType; }
			Data[] subData(){ return []; }
		}
	}
	static DataTypeInfo Type(){ return _dataType; }
	private static DataTypeInfo _dataType;
	static int updateCount;
	static void ResetUpdateCount()
	{
		updateCount = 0;
	}
}

/++
 + A functions that does nothing basically.
 +/
private void MockFunction(Data[],Data[])
{
	mixin( logFunction!"MockNode.update" );  
	MockData.updateCount++;  
	log.writeln(MockData.updateCount);
}

/++
 + Creates a dummy node with a given amount of inputs and outputs
 +/
Node NewMockNode(int nbInputs, int nbOutputs)
{
	mixin( logFunction!"NewMockNode" );
    InputPortInitializer[] inputs   = [];
    OutputPortInitializer[] outputs = [];
    for(int i = 0; i < nbInputs; ++i)  
    	inputs  ~= InputPortInitializer("Input", new AlwaysCompatible, OPTIONAL );
    
    for(int i = 0; i < nbOutputs; ++i)  
	    outputs ~= OutputPortInitializer("Output", MockData.Type );

    return new DynamicNode( inputs, outputs, &MockFunction );
}



//              #######   #####    ####   #####    ####
//                 #      #       #         #     #   
//                 #      ###      ###      #      ### 
//                 #      #           #     #         #
//                 #      #####   ####      #     #### 




unittest
{
	mixin( logTest!"kiwi.mock" );

	MockData.ResetUpdateCount();
	assert( MockData.updateCount == 0 );

	auto n1 = NewMockNode(2,2);
	auto n2 = NewMockNode(2,2);

	assert( n2.inputs.length == 2 );
	assert( n2.outputs.length == 2 );

	n1.input(0).connect( n2.output(1) );
	n2.output(0).connect( n1.input(1) );

	assert( n1.input(0).isConnected );
	assert( n1.input(1).isConnected );
	assert( n2.output(0).isConnected );
	assert( n2.output(1).isConnected );


	n1.update();
	n2.update();
	log.writeln( MockData.updateCount );
	assert( MockData.updateCount == 2 );
}