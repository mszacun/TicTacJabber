#ifndef SERWER_H
#define SERWER_H

#include <list>
#include <jabber.h>
#include <cstring>
#include <cstdlib>
#include "gui.h"
#include "ranking.h"

void Odbierz_obecnosc(Jabber::Klient* klient, Jabber::Obecnosc& obecnosc, void* dane);

class Serwer : public Jabber::Parser_wiadomosci<Jabber::Wiadomosc>
{
	public:
		Serwer(const char* nowy_plik_konfiguracyjny);
		~Serwer();

		void Wczytaj_konfiguracje();		
		void Zaloguj();
		bool Parsuj(Jabber::Klient* nie_uzywany, Jabber::Wiadomosc& wiadomosc);
		void Uruchom();
		void Usun_gre_z_listy(Gra* gra);
		void Zakoncz_prace();

		const char* Pobierz_plik_z_rankingiem() const;
		bool Poprawny_ranking() const;
	private:
		std::vector<Gra*> lista_gier;
		Ranking ranking;
		Jabber::Klient* klient;
		Gui* gui;
		char* plik_konfiguracyjny;
		char* jid;
		char* haslo;
		char* zasob;
		char* adres_serwera;
		char* opis;
		char* plik_z_rankingiem;
		bool tworzyc_gui;
};

#endif
