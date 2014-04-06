FLAGI=-ggdb -pg -Wall -I ./parser
KOMPILATOR=g++
PLIKI_O = jabber_wiadomosc.o md5.o base64.o funkcje.o jabber.o jabber_strofa.o \
	 jabber_obecnosc.o jabber_iq.o my_jabber.o parser/libparser_xml.a

all: $(PLIKI_O) parser_xml.o parser_wiadomosci.h
	$(KOMPILATOR) $(FLAGI) $(PLIKI_O) -o jabber -l pthread -L ./parser/ -l parser_xml
	ar rcsv libjabber.a $(PLIKI_O)

my_jabber.o: my_jabber.cpp
	$(KOMPILATOR) $(FLAGI) -c my_jabber.cpp -l pthread
	
funkcje.o: funkcje.cpp funkcje.h
	$(KOMPILATOR) $(FLAGI) -c funkcje.cpp -l pthread	
	
md5.o: md5.cpp md5.h
	$(KOMPILATOR) $(FLAGI) -c md5.cpp -l pthread
	
base64.o: base64.h base64.cpp
	$(KOMPILATOR) $(FLAGI) -c base64.cpp -l pthread

jabber_wiadomosc.o: jabber_wiadomosc.h jabber_wiadomosc.cpp
	$(KOMPILATOR) $(FLAGI) -c jabber_wiadomosc.cpp -l pthread
	
jabber_wysylanie_plikow.o: jabber_wysylanie_plikow.cpp jabber_wysylanie_plikow.h
	$(KOMPILATOR) $(FLAGI) -c jabber_wysylanie_plikow.cpp -l pthread

parser_xml.o: 
	make -C ./parser

jabber.o: jabber.cpp jabber.h
	$(KOMPILATOR) $(FLAGI) -c jabber.cpp -l pthread

jabber_strofa.o: jabber_strofa.cpp jabber_strofa.h
	$(KOMPILATOR) $(FLAGI) -c jabber_strofa.cpp -l pthread
	
jabber_obecnosc.o: jabber_obecnosc.cpp jabber_obecnosc.h
	$(KOMPILATOR) $(FLAGI) -c jabber_obecnosc.cpp -l pthread
	
jabber_iq.o: jabber_iq.cpp jabber_iq.h
	$(KOMPILATOR) $(FLAGI) -c jabber_iq.cpp -l pthread
	
clean:
	rm *.o
	rm jabber
	make clean -C ./parser

recompile: clean all

big_clean: clean
	rm *~
	

