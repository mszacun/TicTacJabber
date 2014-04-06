#include "plansza.h"

void Dolacz_znak(char* lancuch, char znak)
{
	char temp[strlen(lancuch) + 1];
	strcpy(temp, lancuch);
	sprintf(lancuch, "%s%c", temp, znak);
}

Pole::Pole(int x, int y)
{
	kolumna = x;
	wiersz = y;
}

Pole Pole::Nastepne_pole(KIERUNEK kierunek) const
{
	Pole wynik(kolumna, wiersz);
	
	if (kierunek == GORA)
		wynik.wiersz--;
	if (kierunek == DOL)
		wynik.wiersz++;
	if (kierunek == LEWO)
		wynik.kolumna--;
	if (kierunek == PRAWO)
		wynik.kolumna++;

	if (kierunek == LEWY_UKOS_GORA)
	{
		wynik.kolumna--;
		wynik.wiersz--;
	}
	if (kierunek == LEWY_UKOS_DOL)
	{
		wynik.kolumna--;
		wynik.wiersz++;			
	}
	if (kierunek == PRAWY_UKOS_DOL)
	{
		wynik.kolumna++;
		wynik.wiersz++;
	}
	if (kierunek == PRAWY_UKOS_GORA)
	{
		wynik.kolumna++;
		wynik.wiersz--;
	}
	return wynik;
}

Pole Pole::Nastepne_pole_domyslnie() const
{
	Pole wynik(kolumna, wiersz);
	wynik.kolumna++;
	if (wynik.kolumna > 2)
	{
		wynik.wiersz++;
		wynik.kolumna = 0;
	}
	return wynik;
}

bool Pole::Poprawne() const
{
	if ((wiersz >= 0) && (wiersz <= 2) && (kolumna >= 0) && (kolumna <= 2))
	    return true;
	else
		return false;
}

int Pole::Pobierz_wiersz() const
{
	return wiersz;
}

int Pole::Pobierz_kolumne() const
{
	return kolumna;
}

Plansza::Plansza()
{
	int i;

	plansza = new char*[3];
	for (i = 0; i < 3; i++)
		plansza[i] = new char[3];
	Resetuj();
}

Plansza::Plansza(const Plansza& inna)
{
	int i;
	int j;

	plansza = new char*[3];
	for (i = 0; i < 3; i++)
		plansza[i] = new char[3];	

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			plansza[i][j] = inna.plansza[i][j];
}

Plansza::~Plansza()
{
	int i; 
	
	for (i = 0; i < 3; i++)
	{
		delete[] plansza[i];
	}
	delete[] plansza;
}

void Plansza::Resetuj()
{
	int i;
	int j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			plansza[i][j] = 0;
}

char Plansza::Sprawdz_czy_ktos_wygral() const
{
	Pole aktualne_pole(0, 0);
	Pole nastepne_pole(0, 0);
	int kierunek;
	char znak;

	while (aktualne_pole.Poprawne())
	{
		if (plansza[aktualne_pole.Pobierz_wiersz()][aktualne_pole.Pobierz_kolumne()] == 0)
		{
			aktualne_pole = aktualne_pole.Nastepne_pole_domyslnie();
			continue;
		}
		/* pole u gory */
		znak = plansza[aktualne_pole.Pobierz_wiersz()][aktualne_pole.Pobierz_kolumne()];
		kierunek = GORA;
		/* sprawdzamy po kolei wszystkie pola sasiadujace z naszym aktualnym pole */
		while (kierunek <= PRAWY_UKOS_GORA)
		{
			nastepne_pole = aktualne_pole.Nastepne_pole((KIERUNEK)kierunek);
			if (nastepne_pole.Poprawne() == false)
			{
				kierunek++;
				continue;
			}
			if (plansza[nastepne_pole.Pobierz_wiersz()][nastepne_pole.Pobierz_kolumne()] == znak)
			{
				nastepne_pole = nastepne_pole.Nastepne_pole((KIERUNEK)kierunek);
				if (nastepne_pole.Poprawne() == false)
				{
					kierunek++;
					continue;
				}
				if (plansza[nastepne_pole.Pobierz_wiersz()][nastepne_pole.Pobierz_kolumne()] == znak)
					return znak;
			}
			kierunek++;
		}
		aktualne_pole = aktualne_pole.Nastepne_pole_domyslnie();
	}
	return 0;
}

bool Plansza::Sprawdz_czy_jest_remis() const
{
	int i;
	int j;
	
	/* jesli wszystkie pola sa zapelnione to jest remis */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (plansza[i][j] == 0)
				return false;
		}
	}
	return true;
}
char* Plansza::Rysuj() const
{
	int wiersz;
	int kolumna;
	int i;
	char* wynik = new char[50];

	for (i = 0; i < 50; i++)
		wynik[i] = '\0';
	for (wiersz = 0; wiersz < 3; wiersz++)
	{
		for (kolumna = 0; kolumna < 3; kolumna++)
		{
			if (plansza[wiersz][kolumna] != 0)
				Dolacz_znak(wynik, plansza[wiersz][kolumna]);
			else
			{
				if (wiersz < 2)
					Dolacz_znak(wynik, '_');
				else
					Dolacz_znak(wynik, ' ');
			}
			if (kolumna < 2)
				Dolacz_znak(wynik, '|');
		}
		Dolacz_znak(wynik, '\n');
	}
	return wynik;
}

void Plansza::Wstaw_znak(int x, int y, char znak)
{
	plansza[y][x] = znak;
}

char Plansza::Pobierz_znak(int x, int y) const
{
	return plansza[y][x];
}

Plansza& Plansza::operator=(const Plansza& inna)
{
	int i;
	int j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			plansza[i][j] = inna.plansza[i][j];

	return *this;
}