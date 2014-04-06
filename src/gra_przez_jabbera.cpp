#include "gra_przez_jabbera.h"


/**
* @brief konstruktor klasy Gra
*
* @param nowy_gracz1 Gracz, ktory chce rozpoczac nowa gre
* @param klient Klient Jabbera zywany do komunikacji z serwerem
*/
Gra::Gra(const char* nowy_gracz1, Jabber::Klient* klient)
{
	gracz1 = strdup(nowy_gracz1);
	gracz2 = NULL;
	this->klient = klient;
	stan_gry = WYSZUKIWANIE_GRACZA; 
}

/**
* @biref Konstruktor kopiujacy klasy Gra

* @param inne obiekt klasy Gra, ktory bedzie kopiowany
*/
Gra::Gra(const Gra& inna)
{
	if (inna.gracz1 != NULL)
		gracz1 = strdup(inna.gracz1);
	else
		gracz1 = NULL;
	if (inna.gracz2 != NULL)
		gracz2 = strdup(inna.gracz2);
	else
		gracz2 = NULL;
	plansza = inna.plansza;
	klient = inna.klient;
	stan_gry = inna.stan_gry;
}

/**
* @brief Konstruktor klasy Gra
*/
Gra::~Gra()
{
	/* zwalnianie pamieci po grze */
	if (gracz1 != NULL)
		delete[] gracz1;
	if (gracz2 != NULL)
		delete[] gracz2;
}

/**
* @brief Funkcja, ktora wysyla do gracza aktualny wyglad plaszy do gry w Kolko i krzyzyk
* @param gracz JID gracza, do ktorego wysylamy plansze
*/
void Gra::Wyslij_podglad_planszy(const char* gracz) const
{
	char* plansza_do_wyslania = plansza.Rysuj();
	char tekst_wiadomosci[256];
	
	sprintf(tekst_wiadomosci, "Aktualny wyglad planszy:\n%s", plansza_do_wyslania);
	delete[] plansza_do_wyslania;
	Jabber::Wiadomosc wiadomosc(NULL, gracz, NULL, tekst_wiadomosci);
	klient->Wyslij_wiadomosc(wiadomosc);
}
	
/**
* @brief Funkcja, ktora przerywa gre pomiedzy dwoma graczami
*
* Ta funkcja zajmuje sie przerwaniem gry na zyczenie gracza. Jest ona wywolywana, gdy gracz wysle komende 'przerwij'.
* Funkcja wysyla wiadomosc do drugiego gracza zwaierajaca informacje o przerwaniu gry.
* @param gracz JID gracza, ktory przerwal gre
*/
void Gra::Przerwij(const char* gracz)
{
	Jabber::Wiadomosc wiadomosc(NULL, NULL, NULL, "Twoj przeciwnik przerwal gre. Aby zagrac ponownie wpisz \"start\"");
	if (strstr(gracz, gracz1) != NULL)
		wiadomosc.Zmien_odbiorce(gracz2);
	if (strstr(gracz, gracz2) != NULL)
		wiadomosc.Zmien_odbiorce(gracz1);
	klient->Wyslij_wiadomosc(wiadomosc);
}

/**
* @brief Funkcja, ktora rozpoczyna gre pomiedzy dwoma graczami
*
* Ta funkcja jest wywolywana, gdy gracz nr 2 zgodzi sie na gre. Jest ona odpowiedzialna za poinformowanie o tym gracza 1, a ponadto zada od niego wykonania
* pierwszego ruchu.
* @param gra Gra, ktora wlasnie sie rozpoczyna
*/
void Gra::Start()
{
	Jabber::Wiadomosc wiadomosc(NULL, gracz1, NULL, "Gracz nr 2 zgodzl sie na gre. Teraz twoj ruch");

	klient->Wyslij_wiadomosc(wiadomosc);
	
	wiadomosc.Zmien_odbiorce(gracz2);
	wiadomosc.Zmien_tresc("Zaczynam gre. Aktualnie czekam na ruch gracza 1");
	klient->Wyslij_wiadomosc(wiadomosc);
	stan_gry = RUCH_1;
	Czekaj_na_ruch();
}

/**
* @brief Funkcja, ktora informuje gracza o jego ruchu
*
* Ta funkcja wysyla do gracza, ktory ma aktualnie ruch informacje o tym, ze jego przeciwnik juz wykonal swoj ruch.
* Ponadto wysyla ona takze informacje o aktualnym wygladzie planszy (aktualnie tylko do gracza, ktory ma teraz ruch).
* @param gra Gra, w ktorej nastapil ruch
*/
void Gra::Czekaj_na_ruch() const
{
	Jabber::Wiadomosc wiadomosc(NULL, NULL, NULL, "Wyslij do mnie numer pola(1-9), na ktory chcesz wstawic swoj znak");

	Wyslij_podglad_planszy(gracz1);
	Wyslij_podglad_planszy(gracz2);
	
	if (stan_gry == RUCH_1)
	{
		wiadomosc.Zmien_odbiorce(gracz1);
	}
	if (stan_gry == RUCH_2)
	{
		wiadomosc.Zmien_odbiorce(gracz2);
	}
	klient->Wyslij_wiadomosc(wiadomosc);
}
	
/**
* @brief Funkcja, ktora konczy gre pomiedzy dwoma graczami
*
* Ta funkcja informuje oboje graczy o wyniku gry (kto wygral, lub czy jest remis).
* Ponadto jest ona odpowiedzialna za zapisanie wyniku gry w rankingu.
* @param ranking Ranking, ktory trzeba uaktualnic
*/
void Gra::Zakoncz(const Ranking& ranking)
{
	char znak;
	struct Jabber::Wiadomosc wiadomosc(NULL, NULL, NULL, "Gratulacje! Udalo ci sie wygrac");
	
	znak = plansza.Sprawdz_czy_ktos_wygral();
	/* wysylanie wiadomosci koncowych */
	Wyslij_podglad_planszy(gracz1);
	Wyslij_podglad_planszy(gracz2);	
	if (znak == 'x')
	{
		wiadomosc.Zmien_odbiorce(gracz1);
		klient->Wyslij_wiadomosc(wiadomosc);
		wiadomosc.Zmien_tresc("Niestety, przegrales ta parite. Jesli chcesz zagrac jeszcze raz wpisz \"start\"");
		wiadomosc.Zmien_odbiorce(gracz2);
		klient->Wyslij_wiadomosc(wiadomosc);
		
		/* uaktualnienie rankingu */
		ranking.Uaktualnij_ranking_gracza(gracz1, WYGRANA);
		ranking.Uaktualnij_ranking_gracza(gracz2, PRZEGRANA);
	}
	if (znak == 'o')
	{
		wiadomosc.Zmien_odbiorce(gracz2);
		klient->Wyslij_wiadomosc(wiadomosc);
		wiadomosc.Zmien_tresc("Niestety, przegrales ta parite. Jesli chcesz zagrac jeszcze raz wpisz \"start\"");
		wiadomosc.Zmien_odbiorce(gracz1);
		klient->Wyslij_wiadomosc(wiadomosc);

		/* uaktualnienie rankingu */
		ranking.Uaktualnij_ranking_gracza(gracz2, WYGRANA);
		ranking.Uaktualnij_ranking_gracza(gracz1, PRZEGRANA);
	}		
	if (znak == 0) /* to znaczy ze jest remis */
	{
		wiadomosc.Zmien_tresc("Ta parita zakonczyla sie remisem! Jesli chcesz zagrac dogrywke wpisz \"start\"");
		wiadomosc.Zmien_odbiorce(gracz1);
		klient->Wyslij_wiadomosc(wiadomosc);
		wiadomosc.Zmien_odbiorce(gracz2);
		klient->Wyslij_wiadomosc(wiadomosc);
		
		/* uaktualnienie rankingu */
		ranking.Uaktualnij_ranking_gracza(gracz2, REMIS);
		ranking.Uaktualnij_ranking_gracza(gracz1, REMIS);
	}
}

/**
* @brief Funkcja, ktora prosi o podanie JID drugiego gracza
*/ 
void Gra::Popros_o_gracza2()
{
	Jabber::Wiadomosc wiadomosc(NULL, gracz1, NULL, "Podaj JID uzytkownika, z ktorym chcesz zagrac");
	klient->Wyslij_wiadomosc(wiadomosc);		
}

/**
* @brief Funkcja, ktora wstawia znak(o, albo x) na plansze do gry
*
* @param nadawca Gracz, ktory wstawia znak
* @numer_pola Numer pola na planszy, na ktore zostanie wstawiony znak(1-9)
*/
void Gra::Wstaw_znak_na_plansze(const char* nadawca, int numer_pola)
{
	int x;
	int y;
	char znak;
	
	/* sprawdzenie czy, mamy wiadomosc od prawidlowego gracza */
	if ((stan_gry == RUCH_1) && (strcmp(gracz1, nadawca) != 0))
	{
		Jabber::Wiadomosc blad(NULL, nadawca, NULL, "Daj szanse na ruch twojemu przeciwnikowi");
		klient->Wyslij_wiadomosc(blad);
	}
	if ((stan_gry == RUCH_2) && (strcmp(gracz2, nadawca) != 0))
	{
		Jabber::Wiadomosc blad(NULL, nadawca, NULL, "Daj szanse na ruch twojemu przeciwnikowi");
		klient->Wyslij_wiadomosc(blad);
	}		
	/* wstawienie znaku na polu otrzymanym od gracza */
	if (numer_pola != 0 && numer_pola < 10)
	{
		if (numer_pola <= 3)
		{
			x = numer_pola - 1;
			y = 0;
		}
		if (numer_pola > 3 && numer_pola <= 6)
		{
			x = numer_pola - 4;
			y = 1;
		}
		if (numer_pola > 6)
		{
			x = numer_pola - 7;
			y = 2;
		}
		if (stan_gry == RUCH_1)
			znak = 'x';
		else
			znak = 'o';
		if (plansza.Pobierz_znak(x, y) == 0)
		{
			plansza.Wstaw_znak(x, y, znak);
			/* zmien informacje o tym, kto ma teraz swoj ruch */
			if (stan_gry == RUCH_1)
				stan_gry = RUCH_2;
			else
				stan_gry = RUCH_1;
		}
		else
		{
			Jabber::Wiadomosc blad(NULL, nadawca, NULL, "Wybrane pole jest juz zajete");
			klient->Wyslij_wiadomosc(blad);
		}
	}
	else
	{
		struct Jabber::Wiadomosc blad(NULL, nadawca, NULL, "Podano bledny numer pola\n");
		klient->Wyslij_wiadomosc(blad);
	}
}
	
/**
* @brief Funkcja, ktora zajmuje sie wyslanie zaproszenia do gracza nr 2.
*
* Ta funkcja wysyla zaproszenie do gra dla gracza nr 2.
* @param gra Gra, do ktorej gracz nr 2 chcemy wyslac zaproszenie
*/
void Gra::Zapros_gracza2()
{
	char tekst[256];
	char* jid_bez_zasobu = NULL;

	jid_bez_zasobu = Wydziel_id_z_jid(gracz1);
	sprintf(tekst, "Zostales zaproszony do gry \"Kolko i krzyzyk przez jabbera\" przez uzytkownika %s\n \
Czy przyjmujesz zaproszenie? Odpisz Tak lub Nie", jid_bez_zasobu);
	Jabber::Wiadomosc wiadomosc(NULL, gracz2, NULL, tekst);
	klient->Wyslij_wiadomosc(wiadomosc);
	
	stan_gry = OCZEKIWANIE;
	delete[] jid_bez_zasobu;
}	

/* FIXME: Do napisania! */
void Gra::Zapisz_gre_do_pliku(const char* nazwa_pliku)
{
	Parser_XML parser;

	parser.Przeladuj_plik(nazwa_pliku);
	if (parser.Czy_poprawny())
	{
	}
}

void Gra::Zmien_gracza1(const char* nowy_gracz)
{
	if (gracz1 != NULL)
		delete[] gracz1;
	gracz1 = strdup(nowy_gracz);
}

void Gra::Zmien_gracza2(const char* nowy_gracz)
{
	if (gracz2 != NULL)
		delete[] gracz2;
	gracz2 = strdup(nowy_gracz);
}

const char* Gra::Pobierz_gracza1() const
{
	return gracz1;
}

const char* Gra::Pobierz_gracza2() const
{
	return gracz2;
}

STAN_GRY Gra::Pobierz_stan_gry() const
{
	return stan_gry;
}

const Plansza& Gra::Pobierz_plansze() const
{
	return plansza;
}

Gra Gra::operator=(const Gra& inna)
{
	if (gracz1 != NULL)
		delete[] gracz1;
	if (gracz2 != NULL)
		delete[] gracz2;

	if (inna.gracz1 != NULL)
		gracz1 = strdup(inna.gracz1);
	else
		gracz1 = NULL;
	if (inna.gracz2 != NULL)
		gracz2 = strdup(inna.gracz2);
	else
		gracz2 = NULL;
	plansza = inna.plansza;
	klient = inna.klient;
	stan_gry = inna.stan_gry;

	return *this;
}

bool Gra::operator==(const Gra& inna)
{
	if (strcmp(inna.gracz1, gracz1) != 0)
		return false;
	if (strcmp(inna.gracz2, gracz2) != 0)
		return false;

	return true;
}


/**
* @brief Funkcja, ktora zajmuje sie znalezieniem odpowiedniej gry na liscie gier
*
* Ta funkcja odnajduje odpowiednia gre na odstawie podanego gracza. Zwraca ona ta gre, w ktorej bierze on udzial.
* Obojetnie, czy podany gracz jest graczem 1 czy 2.
* @param lista_gier Lista gier, ktora trzeba przeszukac w poszukiwaniu odpowiedniej gry
* @param gracz Gracz, ktorego gre chcemy znalezc. Szukamy takiej gry, w ktorej ten gracz bierze udzial.
* @return Gra, w ktorej nasz gracz bierz udzial.
*/
int Znajdz_gre(std::vector<Gra*> lista, const char* gracz)
{
	const char* gracz1;
	const char* gracz2;
	int i = 0;
	Gra* temp = NULL;
	
	if (gracz == NULL)
		return -1;

	std::vector<Gra*>::iterator koniec = lista.end();

	for (std::vector<Gra*>::iterator it = lista.begin(); it != koniec; it++)
	{
		temp = *it;
		gracz1 = temp->Pobierz_gracza1();
		gracz2 = temp->Pobierz_gracza2();

		if (gracz1 != NULL)
		{
			if (strstr(gracz, gracz1) != NULL)
				return i;
		}
		if (gracz2 != NULL)
		{
			if (strstr(gracz, gracz2) != NULL)
				return i;
		}
		i++;
	}
	return -1;
}