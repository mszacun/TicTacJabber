#include "serwer.h"

/* FIXME: Dodac obsluge strof typu presence za pomoca parserow wiadomosci! */

Serwer::Serwer(const char* nowy_plik_konfiguracyjny) : Jabber::Parser_wiadomosci<Jabber::Wiadomosc>(Jabber::Parser_wiadomosci<Jabber::Wiadomosc>::MESSAGE),
	ranking(NULL),klient(NULL), gui(NULL), jid(NULL), haslo(NULL), zasob(NULL),
	adres_serwera(NULL), opis(NULL), plik_z_rankingiem(NULL), tworzyc_gui(false)
{
	plik_konfiguracyjny = strdup(nowy_plik_konfiguracyjny);
	Wczytaj_konfiguracje();
	if (tworzyc_gui == true)
	{ 
		/* tworzenie gui */
		gui = new Gui(&lista_gier);
		gui->Pokaz();
	}
	Zaloguj();
	ranking.Zmien_plik_z_rankingiem(plik_z_rankingiem);
}


Serwer::~Serwer()
{
	delete klient;	
	delete gui;	
	delete[] plik_konfiguracyjny;
	delete[] jid;
	delete[] haslo;
	delete[] zasob;
	delete[] adres_serwera;
	delete[] opis;
	delete[] plik_z_rankingiem;
}

void Serwer::Wczytaj_konfiguracje()
{
	Parser_XML parser;

	parser.Przeladuj_plik(plik_konfiguracyjny);

	if (parser.Czy_poprawny() == false)
	{
		puts("Nie udalo sie wczytac konfiguracji!\n");
		return;
	}
	if (parser.Pobierz_tresc_elementu("nazwa_uzytkownika") != NULL)
		jid = strdup(parser.Pobierz_tresc_elementu("nazwa_uzytkownika"));
	if (parser.Pobierz_tresc_elementu("haslo") != NULL)
		haslo = strdup(parser.Pobierz_tresc_elementu("haslo"));
	if (parser.Pobierz_tresc_elementu("serwer") != NULL)
		adres_serwera = strdup(parser.Pobierz_tresc_elementu("serwer"));
	if (parser.Pobierz_tresc_elementu("zasob") != NULL)
		zasob = strdup(parser.Pobierz_tresc_elementu("zasob"));
	if (parser.Pobierz_tresc_elementu("opis") != NULL)
		opis = strdup(parser.Pobierz_tresc_elementu("opis"));
	if (parser.Pobierz_tresc_elementu("plik_z_rankingiem") != NULL)
		plik_z_rankingiem = strdup(parser.Pobierz_tresc_elementu("plik_z_rankingiem"));	
	if (parser.Znajdz_wezel("gui") != NULL)
		tworzyc_gui = true;
}

void Serwer::Zaloguj()
{
	/* przygotowywanie klienta */ 
	if (klient == NULL)
	{
		klient = new Jabber::Klient(adres_serwera, jid);
		klient->Podlacz_funkcje_odbioru_obecnosci(Odbierz_obecnosc, NULL);
		klient->Dodaj_parser(this);
	}
	if (tworzyc_gui == true)
		gui->Dodaj_wiadomosc_do_paska_statusu("Lacze z serwerem...");
	if (klient->Polacz_z_serwerem() == Jabber::OK1)
	{
		if (tworzyc_gui == true)		
			gui->Dodaj_wiadomosc_do_paska_statusu("Loguje sie...");
		if (klient->Zaloguj(haslo, zasob) == Jabber::ERROR2)
		{
			if (tworzyc_gui == true)
				gui->Dodaj_wiadomosc_do_paska_statusu("Nie udalo sie zalogowac, sprawdz dane do logowania(nazwa uzytkownika, haslo, adres serwera");
			else	                               
				printf("Nie udalo sie zalogowac, sprawdz dane do logowania(nazwa uzytkownika, haslo, adres serwera\n");
		}
		else
		{
			klient->Zmien_status(Jabber::Obecnosc::DOSTEPNY, opis);
			if (tworzyc_gui == true)
				gui->Dodaj_wiadomosc_do_paska_statusu("Serwer gotowy do pracy");
			else
				printf("Serwer gotowy do pracy!\n");
		}
	}
	else
	{
		if (tworzyc_gui == true)
			gui->Dodaj_wiadomosc_do_paska_statusu("Nie udalo sie polaczyc z serwerem");
		else
			printf("Nie udalo sie polaczyc z serwerem\n");
	}
}

/**
* @brief Funkcja, ktora zajmuje sie analizowaniem wiadomosci otrzymanych od graczy.
*
* Ta funkcja analizuje kazda wiadomosc, ktora zostanie przyslana do serwera gry.
* Nastepnie na podstawie tresci tej wiadomosci wykonuje odpowiednie dzialanie i wywoluje funkcje.
* @param klient Klient, ktory odebral wiadomosc (parametr dostarczany prze funkcje odbierajaca w bibliotece do obslugi protokolu Jabbera)
* @param wiadomosc Przyslana przez gracza wiadomosc
* @param dane Wskaznik na typ void rzutowany do wskaznika na serwer
*/
bool Serwer::Parsuj(Jabber::Klient* nie_uzywany, Jabber::Wiadomosc& wiadomosc)
{
	int pozycja = Znajdz_gre(lista_gier, wiadomosc.Pobierz_nadawce());
	char* tresc = strdup(wiadomosc.Pobierz_tresc());
	
	if (gui != NULL)
		gui->Dodaj_wiadomosc(wiadomosc);

	if (pozycja == -1)
	{
		bool obsluzono_wiadomosc = false;
		if (strstr(tresc, "ranking"))
		{
			char* pozycja = strstr(tresc, " ");
			char* gracz = NULL;
			if (pozycja != NULL)
			{
				pozycja++;
				gracz = strdup(pozycja);
			}
			ranking.Wyslij_ranking_do_gracza(klient, wiadomosc.Pobierz_nadawce(), gracz);
			if (gracz != NULL)
				delete[] gracz;
			obsluzono_wiadomosc = true;
		}
		if (strcmp(tresc, "start") == 0)
		{
			Gra* nowa = new Gra(wiadomosc.Pobierz_nadawce(), klient);
			nowa->Popros_o_gracza2();
			lista_gier.push_back(nowa);
			obsluzono_wiadomosc = true;
		}
		if (obsluzono_wiadomosc == false)
		{
			Jabber::Wiadomosc blad(NULL, wiadomosc.Pobierz_nadawce(), NULL, "Blad! Nie znaleziono gry, w ktorej bierzesz udzial!");
			klient->Wyslij_wiadomosc(blad);
		}
	}
	else
	{
		Gra* gra = lista_gier[pozycja];
		bool zapisz_gre = true;
		
		if (strstr(tresc, "msg"))
		{
			char tekst[200];
			snprintf(tekst, 200, "Twoj przeciwnik mowi: %s", tresc + strlen("msg "));
			struct Jabber::Wiadomosc info(NULL, NULL, NULL, tekst);
			if (strstr(wiadomosc.Pobierz_nadawce(), gra->Pobierz_gracza1()) != NULL)
				info.Zmien_nadawce(gra->Pobierz_gracza2());
			else
				info.Zmien_nadawce(gra->Pobierz_gracza1());
			klient->Wyslij_wiadomosc(info);
		}			
		if (strcmp(tresc, "przerwij") == 0)
		{			
			gra->Przerwij(wiadomosc.Pobierz_nadawce());
			Usun_gre_z_listy(gra);
			/* aktualizacja gui */
			if (gui != NULL)
				gui->Zaaktualizuj_liste_gier();
			delete[] tresc;
			return false;
		}
		if ((gra->Pobierz_stan_gry() == RUCH_1) || (gra->Pobierz_stan_gry() == RUCH_2))
		{
			gra->Wstaw_znak_na_plansze(wiadomosc.Pobierz_nadawce(), atoi(tresc));
			if ((gra->Pobierz_plansze().Sprawdz_czy_ktos_wygral() != 0) || gra->Pobierz_plansze().Sprawdz_czy_jest_remis())
			{
				gra->Zakoncz(ranking);
				Usun_gre_z_listy(gra);
				zapisz_gre = false;
			}
			else
				gra->Czekaj_na_ruch();
		}
		if (gra->Pobierz_stan_gry() == OCZEKIWANIE)
		{
			if ((strcmp(wiadomosc.Pobierz_tresc(), "tak") == 0))
			{
				gra->Zmien_gracza2(wiadomosc.Pobierz_nadawce());
				gra->Start();
			}
			else
			{
				Jabber::Wiadomosc blad(NULL, gra->Pobierz_gracza1(), NULL, "Grarcz nr 2 nie zgodzil sie gre z toba");
				klient->Wyslij_wiadomosc(blad);
				Usun_gre_z_listy(gra);
				delete[] tresc;
				return false;
			}
		}
		if (gra->Pobierz_stan_gry() == WYSZUKIWANIE_GRACZA)
		{
			gra->Zmien_gracza2(wiadomosc.Pobierz_tresc());
			gra->Zapros_gracza2();
		}
	}
	/* aktualizacja gui */
	if (gui != NULL)
		gui->Zaaktualizuj_liste_gier();
	delete[] tresc;
	return false;
}

void Serwer::Uruchom()
{
	if (gui != NULL)
		gtk_main();
	else
		klient->Dzialaj();
}

void Serwer::Usun_gre_z_listy(Gra* gra)
{
	std::vector<Gra*>::iterator koniec = lista_gier.end();

	for (std::vector<Gra*>::iterator it = lista_gier.begin(); it != koniec; it++)
	{
		Gra* temp = *it;
		if (gra->operator==(*temp))
		{
			lista_gier.erase(it);
			delete temp;
		}
	}
}

void Serwer::Zakoncz_prace()
{
	klient->Wyloguj();
}

bool Serwer::Poprawny_ranking() const
{
	return ranking.Poprawny();
}

void Odbierz_obecnosc(Jabber::Klient* klient, Jabber::Obecnosc& obecnosc, void* dane)
{
	if (obecnosc.Pobierz_typ() == Jabber::Obecnosc::PROSBA_O_SUBSKRYPCJE)
	{
		Jabber::Obecnosc odpowiedz(NULL, obecnosc.Pobierz_nadawce(),
		                           Jabber::Obecnosc::ZASUBSKRYBOWANY, NULL);
		klient->Wyslij_wiadomosc(odpowiedz);
	}
}
