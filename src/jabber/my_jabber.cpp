#include <cstdio> 
#include <cstring>

#include "jabber.h"

class Moj_parser : public Jabber::Parser_wiadomosci<Jabber::Wiadomosc>
{
	public:
		Moj_parser() : Jabber::Parser_wiadomosci<Jabber::Wiadomosc>(Jabber::Parser_wiadomosci<Jabber::Wiadomosc>::MESSAGE) {}
		virtual ~Moj_parser() {}

		virtual bool Parsuj(Jabber::Klient* klient, Jabber::Wiadomosc& odebrana)
		{
			if (odebrana.Pobierz_tresc())
			{
				printf("Odebralem wiadomosc od %s o tresci: %s\n", odebrana.Pobierz_nadawce(), odebrana.Pobierz_tresc());
		    	odebrana.Zmien_odbiorce(odebrana.Pobierz_nadawce());
				odebrana.Zmien_nadawce(NULL);
				if (strcmp(odebrana.Pobierz_tresc(), "bye") == 0)
					klient->Wyloguj();
			//	klient->Wyslij_wiadomosc(odebrana);
			}
			return false;
		}
};

class Parser_obecnosci : public Jabber::Parser_wiadomosci<Jabber::Obecnosc>
{
	public:
		Parser_obecnosci() : Jabber::Parser_wiadomosci<Jabber::Obecnosc>(Jabber::Parser_wiadomosci<Jabber::Obecnosc>::PRESENCE) {}
		virtual ~Parser_obecnosci() {}
		
		virtual bool Parsuj(Jabber::Klient* klient, Jabber::Obecnosc& odebrana)
		{
			printf("Odebralem wiadomosc typu PRESENCE od %s:\n", odebrana.Pobierz_nadawce());
			if (odebrana.Pobierz_typ() != Jabber::Obecnosc::NIEWAZNE)
			{
				if (odebrana.Pobierz_typ() == Jabber::Obecnosc::PROSBA_O_SUBSKRYPCJE)
				{
					odebrana.Zmien_typ(Jabber::Obecnosc::ZASUBSKRYBOWANY);
					odebrana.Zmien_odbiorce(odebrana.Pobierz_nadawce());
					klient->Wyslij_wiadomosc(odebrana);
				}
				printf("Typ: %s\n", Jabber::Obecnosc::Typ_na_tekst(odebrana.Pobierz_typ()));
			}
			else
				printf("Status: %s\n", Jabber::Obecnosc::Status_na_tekst(odebrana.Pobierz_status()));
			return false;
		}
};

int main()
{
	//Jabber::Klient klient("zauris.ru", "szacun");
	Jabber::Klient klient("jabber.snc.ru", "test5");

	Moj_parser* parser = new Moj_parser;
	Parser_obecnosci* parser2 = new Parser_obecnosci;
	klient.Dodaj_parser(parser);
	klient.Dodaj_parser(parser2);
	if (klient.Polacz_z_serwerem()  == Jabber::OK1)
		printf("Udalo sie polaczyc z serwerem\n");
	else
		printf("Wystapil blad podczas poloczenia\n");
	if (klient.Zaloguj("znwshr34", "moj_testowy_program") == Jabber::OK1)
	{
		printf("Otrzymane jid to: %s\n", klient.Pobierz_jid());
	}
	else
		printf("Nie udalo sie zalogowac");
	do
	{
		char tekst[256];
		printf("Podaj wiadomosc do wyslania: \n");
		gets(tekst);
		Jabber::Wiadomosc w1(NULL, "test5@jabber.snc.ru/trojan", NULL, tekst);
		klient.Wyslij_wiadomosc(w1);
	}
	while (1);
	/*Jabber_wyslij_plik(klient, "dexter@jabber.snc.ru/domek_serwer", NULL, "/home/marcin/czerwony kapturek");*/
//	klient.Zapytaj_o_mozliwosci("jabber.snc.ru");
	klient.Dzialaj();

	delete parser;
	delete parser2;
	return 0;
}
