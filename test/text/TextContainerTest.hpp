#ifndef KIWI_TEST_TEXTCONTAINER_HPP
#define KIWI_TEST_TEXTCONTAINER_HPP


#include "text/TextContainer.hpp"
#include "text/TextReader.hpp"
#include "text/TextWriter.hpp"
#include "utils/modulo.hpp"

void TextContainerTest()
{
ScopedBlockMacro(__scp, "kiwi::test::TextContainer")

	kiwi::text::TextContainer tc;
	
	kiwi::text::TextContainer::ReaderType reader(tc,0);
	kiwi::text::TextContainer::WriterType writer(tc,0);
	
	writer.getLine() = kiwi::string("1st line test");
	
	assert(reader.nbLines() == writer.nbLines() );
	Debug::print() << "nbLines " << reader.nbLines() << endl();
	assert(reader.nbLines() == 1);

	
	writer.insertLine( kiwi::string("2nd line test") );//, 0, kiwi::text::TextWriter::BEFORE );
	Debug::print() << "nbLines " << reader.nbLines() << endl();
	assert(reader.nbLines() == 2);

	
	writer.insertLine( kiwi::string("3rd line test") );//, 0, kiwi::text::TextWriter::BEFORE );
	assert(reader.nbLines() == writer.nbLines() );
	Debug::print() << "nbLines " << reader.nbLines() << endl();
	assert(reader.nbLines() == 3);
	
	
	reader.gotoLine(0);
	assert( reader.getLine() == kiwi::string("1st line test") );
	Debug::print() << "#" << reader.getLine() << endl();
	
	reader.gotoNextLine();
	assert( reader.getLine() == kiwi::string("2nd line test") );
	Debug::print() << "#" << reader.getLine() << endl();
	
	reader.gotoNextLine();
	assert( reader.getLine() == kiwi::string("3rd line test") );
	Debug::print() << "#" << reader.getLine() << endl();
	
	writer.removeLine(1);
	assert(reader.nbLines() == 2);
	
	Debug::endl(3);
	
	reader.gotoLine(0);
	assert(reader.currentLine() == 0);
	
	Debug::print() << "#" << reader.getLine() << endl();
	reader.gotoNextLine();
	Debug::print() << "#"  << reader.getLine() << endl();
	
	Debug::print() << "nbLines " << reader.nbLines() << endl();
	
	writer.removeLine(0);
	Debug::print() << "#" << reader.getLine() << endl();
	
	writer.insertLine(kiwi::string("Added out of the text"), 10);
	reader.gotoLine(0);
	while(reader.currentLine() < reader.nbLines() - 1)
	{
		Debug::print() << "#"<< reader.getLine() << endl();
		reader.gotoNextLine();
	}
	
	// remove more lines than the total number of lines
	for(unsigned i = 0; i < 15; ++i) 
		writer.removeLine(0);
	assert(reader.nbLines() == 1);
	reader.gotoLine(0);
	assert(reader.getLine() == "");
	Debug::print() << "NbLines (after deletes): " << reader.nbLines() <<endl();
	
}




#endif