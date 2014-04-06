DOLACZANE_PLIKI_O = -l jabber.a

PLIKI_H = -I ./jabber/ -I ./jabber/parser/

FLAGI = -ggdb -pg -Wall

all: plansza.o main.o gra_przez_jabbera.o serwer.o gui.o ranking.o
	make -C ./jabber/
	g++ plansza.o main.o gra_przez_jabbera.o serwer.o gui.o ranking.o -L./jabber/ -L./jabber/parser/ -l parser_xml -l jabber -l pthread -o kolko_i_krzyzyk $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`
	
plansza.o: plansza.cpp plansza.h
	g++ -c plansza.cpp $(FLAGI)
	
main.o: main.cpp
	g++ -c main.cpp $(PLIKI_H) $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`
	
gra_przez_jabbera.o: gra_przez_jabbera.cpp gra_przez_jabbera.h
	g++ -c gra_przez_jabbera.cpp $(PLIKI_H) $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`

serwer.o: serwer.cpp serwer.h
	g++ -c serwer.cpp $(PLIKI_H) $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`
	
gui.o: gui.cpp gui.h
	g++ -c gui.cpp $(PLIKI_H) $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`
	
ranking.o: ranking.h ranking.cpp
	g++ -c ranking.cpp $(PLIKI_H) $(FLAGI) `pkg-config --cflags --libs gtk+-2.0`
	

clean:
	rm *.o
	rm kolko_i_krzyzyk
	make -C ./jabber/ clean

big_clean:
	rm *.o
	rm kolko_i_krzyzyk
	rm *~
	make -C ./jabber/ big_clean
	rm gmon.out

recompile: clean all
	
