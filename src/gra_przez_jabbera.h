#include <cstring>

#include <jabber.h>

#include "plansza.h"
#include "serwer.h"
#include "ranking.h"

#ifndef GRA_PRZEZ_JABBERA_H
#define GRA_PRZEZ_JABBERA_H


enum STAN_GRY {BLAD, WYSZUKIWANIE_GRACZA, OCZEKIWANIE, RUCH_1, RUCH_2};

class Gra
{
	public:
		Gra(const char* nowy_gracz1, Jabber::Klient* nowy_klient);
		Gra(const Gra& inna);
		~Gra();

		void Wyslij_podglad_planszy(const char* gracz) const;
		void Przerwij(const char* gracz);
		void Start();
		void Czekaj_na_ruch() const;
		void Zakoncz(const Ranking& ranking);
		void Popros_o_gracza2();
		void Wstaw_znak_na_plansze(const char* nadawca, int numer_pola);
		void Zapros_gracza2();
		void Zapisz_gre_do_pliku(const char* nazwa_pliku);		

		void Zmien_gracza2(const char* nowy_gracz);
		void Zmien_gracza1(const char* nowy_gracz);

		const char* Pobierz_gracza1() const;
		const char* Pobierz_gracza2() const;
		STAN_GRY Pobierz_stan_gry() const;
		const Plansza& Pobierz_plansze() const;

		Gra operator=(const Gra& inna);
		bool operator==(const Gra& inna);
	private:
		char* gracz1; /** Jid pierwszego gracza */
		char* gracz2; /** Jid drugiego gracza */
		Plansza plansza; /** Plansza do gry */
		Jabber::Klient* klient; /** Klient uzywany do gry */
		STAN_GRY stan_gry; /** Informuje, o tym, co sie aktualnie dzieje w grze */
};

int Znajdz_gre(std::vector<Gra*> lista, const char* gracz);
#endif
