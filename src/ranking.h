#include <cstdio>
#include <cstring>
#include <jabber.h>
#include <ctype.h>
#include <vector>

#ifndef RANKING_H
#define RANKING_H

enum WYNIK_GRY{ PRZEGRANA, REMIS, WYGRANA };

class Gracz
{
	public:
		Gracz(const char* jid, int wygrane, int przegrane, int remisy);
		Gracz(const char* tekst);
		Gracz(const Gracz& inny);
		~Gracz();

		void Zapisz_do_pliku(FILE* plik) const;

		int Pobierz_ilosc_wygranych() const;
		int Pobierz_ilosc_przegranych() const;
		int Pobierz_ilosc_remisow() const;
		const char* Pobierz_jid() const;

		void Dodaj_wygrana();
		void Dodaj_przegrana();
		void Dodaj_remis();

		bool operator>(const Gracz& inny);
		Gracz& operator=(const Gracz& inny);
	private:
		char* jid;
		int ilosc_wygranych;
		int ilosc_przegranych;
		int ilosc_remisow;
};

class Ranking
{
	public:
		Ranking(const char* nazwa_pliku);
		Ranking(const Ranking& inny);
		~Ranking();

		void Uaktualnij_ranking_gracza(const char* jid, WYNIK_GRY wynik) const;
		void Uaktualnij_ranking_gracza(Gracz* gracz, WYNIK_GRY wynik) const;
		Gracz* Wczytaj_ranking_gracza(const char* jid) const;
		void Wyslij_ranking_do_gracza(Jabber::Klient* klient, const char* odbiorca, const char* gracz = NULL) const;

		void Zmien_plik_z_rankingiem(const char* nowy_plik);

		bool Poprawny() const;
		const char* Pobierz_plik_z_rankingiem() const;			
	private:
		char* sciezka_do_pliku;

		/* funkcja pomocnicza */
		void Uaktualnij_plik_z_rankingiem(Gracz* gracz_do_uaktualnienia) const;
};

char* Wydziel_id_z_jid(const char* jid);

void Posortuj_graczy(Gracz** gracze, int ilosc);

#endif
