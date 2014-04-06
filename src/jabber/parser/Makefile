FLAGI=-ggdb -pg -Wall

all: parser_xml.o main.o
	g++ parser_xml.o main.o -o parser $(FLAGI)
	ar crsv libparser_xml.a parser_xml.o main.o
	
main.o: main.cpp
	g++ -c main.cpp $(FLAGI)
	
parser_xml.o: parser_xml.h parser_xml.cpp
	g++ -c parser_xml.cpp $(FLAGI)

clean:
	rm *.o
	rm parser

big_clean:
	rm *.o
	rm *~
	rm parser

recompile: clean all
