#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef PLANSZA_H
#define PLANSZA_H

class Plansza
{
	public:
		Plansza();
		Plansza(const Plansza& inna);
		~Plansza();

		char Pobierz_znak(int x, int y) const;
		void Resetuj();
		char* Rysuj() const;
		bool Sprawdz_czy_jest_remis() const;
		char Sprawdz_czy_ktos_wygral() const;		
		void Wstaw_znak(int x, int y, char znak);

		Plansza& operator=(const Plansza& inna);
	private:
		char** plansza;
};

enum KIERUNEK {GORA, DOL, LEWO, PRAWO, LEWY_UKOS_DOL, PRAWY_UKOS_DOL, LEWY_UKOS_GORA, PRAWY_UKOS_GORA};

class Pole
{
	public:
		Pole(int x, int y);
		Pole Nastepne_pole(KIERUNEK kierunek) const;
		Pole Nastepne_pole_domyslnie() const;
		bool Poprawne() const;

		int Pobierz_wiersz() const;
		int Pobierz_kolumne() const;
	private:
		int wiersz;
		int kolumna;
};

#endif