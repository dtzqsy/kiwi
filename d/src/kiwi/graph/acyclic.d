module kiwi.graph.acyclic;

import kiwi.core.base;
import kiwi.core.commons;

import std.range;


bool IsAcyclic( Node[] )
{
	throw new NotImplementedYetException("kiwi.graph.traversal.isAcyclic", __FILE__, __LINE__);
}


Node[] DirectDependencies(Node n)
in
{
	assert( n !is null );
}
body
{
	Node[] result = [];
	foreach ( port ; n.inputs )
        if( port.isConnected )
			if ( port.connection.node !is null && ! _contains(result, port.connection.node) )
				result ~= port.connection.node;
	return result;
}

Node[] IndirectDependencies(Node n)
in
{
	assert( n !is null );
}
body
{
	Node[] result = [];
	foreach ( dep ; DirectDependencies(n) )
	{
		result ~= IndirectDependencies(n);
		result ~= n;
	}
				
	return result;
}


Node[] OrderedNodes( Node[] nodes )
{
	Node[] result = [];
	foreach ( node ; nodes )
		_recOrdered( nodes, result, node );
		
	return result;
}


// --------------------------------------------------------------------

private bool _isBefore(Node[] nodes, Node before, Node after)
in
{
	assert( _contains(nodes, before) );
	assert( _contains(nodes, after) );
}
body
{	
	for(int i = 0; i < nodes.length; ++i)
	{
		if ( nodes[i] is before )
			return true;
		else if ( nodes[i] is after )
			return false;
	}
	return false;
}

private bool _contains(Node[] nodes, Node toFind)
{
	foreach ( n ; nodes )
		if ( n is toFind) 
			return true;
	return false;
}

private Node[] _intersection(Node[] r1, Node[]r2)
{
	Node[] result = [];
	foreach ( n ; r1 )
		if( _contains(r2, n) )
			result ~= n;
	return result;
}

void _recOrdered(ref Node[] nodes, ref Node[] result, Node n)
{
	mixin( logFunction!"_recOrdered" );
	if ( !_contains(result, n) && (nodes.length == 0 || _contains(nodes, n)) )
	{
		foreach ( n_in ; n.inputs )
			if ( n_in.isConnected )
				_recOrdered( nodes, result, n_in.connection.node );
		for(int i=0; i<nodes.length; ++i)
			if( nodes[i] is n ) log.writeln(i);
		result ~= n;
	}
}




//              #######   #####    ####   #####    ####
//                 #      #       #         #     #   
//                 #      ###      ###      #      ### 
//                 #      #           #     #         #
//                 #      #####   ####      #     #### 


version(unittest)
{
	import kiwi.utils.mock;
}

unittest
{
	mixin( logTest!"kiwi.graph.acyclic" );

	Node[] nodes = [];

	nodes ~= NewMockNode(2,2);
	nodes ~= NewMockNode(2,2);
	nodes ~= NewMockNode(2,2);
	nodes ~= NewMockNode(2,2);
	nodes ~= NewMockNode(2,2);

	// check that the _contains function works
	assert( _contains( nodes, nodes[0]) );
	assert( _contains( nodes, nodes[1]) );
	assert( _contains( nodes, nodes[2]) );
	assert( _contains( nodes, nodes[3]) );
	assert( _contains( nodes, nodes[4]) );
	assert( !_contains( nodes, NewMockNode(0,0) ) );

	// connect the nodes
	nodes[2].output().connect( nodes[0].input(0) );
	nodes[1].output().connect( nodes[0].input(1) );
	nodes[0].output().connect( nodes[3].input() );

	// compute the direct dependencies of nodes[0]
	auto n0_dependencies = DirectDependencies( nodes[0] );

	// check that nodes[0]'s dependencies are nodes[2] and nodes[1]
	assert( n0_dependencies.length == 2 );
	assert( n0_dependencies[0] is nodes[2] || n0_dependencies[1] is nodes[2] );
	assert( n0_dependencies[0] is nodes[1] || n0_dependencies[1] is nodes[1] );

	// check that _isBefore works
	assert( _isBefore( nodes, nodes[1], nodes[3]) );
	assert( !_isBefore( nodes, nodes[3], nodes[1]) );

	Node[] sortedNodes = OrderedNodes( nodes );
	log.writeln("nodes.length", nodes.length);
	log.writeln("sortedNodes.length", sortedNodes.length);
	assert( sortedNodes.length == nodes.length );
}
