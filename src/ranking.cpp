#include "ranking.h"
#include <jabber.h>

Gracz::Gracz(const char* jid, int wygrane, int przegrane, int remisy) : ilosc_wygranych(wygrane),
	ilosc_przegranych(przegrane), ilosc_remisow(remisy)
{
	this->jid = strdup(jid);
}

Gracz::Gracz(const char* tekst)
{
	char* pozycja = NULL;
	char* wygrane_tekst = NULL;
	char* przegrane_tekst = NULL;
	char* remisy_tekst = NULL;
	char* temp = strdup(tekst);
	int i = 0;

	pozycja = strchr(temp, ' ');
	jid = Jabber::Kopiuj(temp, pozycja - temp);

	/* wydzielenie wygranych */
	pozycja++;
	while (isdigit(pozycja[i]))
		i++;
	wygrane_tekst = Jabber::Kopiuj(pozycja, i);
	ilosc_wygranych = atoi(wygrane_tekst);
	delete[] wygrane_tekst;

	/* wydzielenie przegranych */
	pozycja += i + 1;
	i = 0;
	while (isdigit(pozycja[i]))
		i++;
	przegrane_tekst = Jabber::Kopiuj(pozycja, i);
	ilosc_przegranych = atoi(przegrane_tekst);
	delete[] przegrane_tekst;

	/* wydzielenie remisow */
	pozycja += i + 1;
	i = 0;
	while (isdigit(pozycja[i]))
		i++;
	remisy_tekst = Jabber::Kopiuj(pozycja, i);
	ilosc_remisow = atoi(remisy_tekst);
	delete[] remisy_tekst;
}

Gracz::Gracz(const Gracz& inny)
{
	jid = strdup(inny.jid);
	ilosc_wygranych = inny.ilosc_wygranych;
	ilosc_przegranych = inny.ilosc_przegranych;
	ilosc_remisow = inny.ilosc_remisow;
}

Gracz::~Gracz()
{
	delete[] jid;
}

void Gracz::Zapisz_do_pliku(FILE* plik) const
{
	char tekst[256];

	if (plik == NULL)
		return;
	sprintf(tekst, "%s %d %d %d\n", jid, ilosc_wygranych, ilosc_przegranych, ilosc_remisow);
	fputs(tekst, plik);
}

int Gracz::Pobierz_ilosc_wygranych() const
{
	return ilosc_wygranych;
}

int Gracz::Pobierz_ilosc_przegranych() const
{
	return ilosc_przegranych;
}

int Gracz::Pobierz_ilosc_remisow() const
{
	return ilosc_remisow;
}

const char* Gracz::Pobierz_jid() const
{
	return jid;
}

void Gracz::Dodaj_wygrana()
{
	ilosc_wygranych++;
}

void Gracz::Dodaj_przegrana()
{
	ilosc_przegranych++;
}

void Gracz::Dodaj_remis()
{
	ilosc_remisow++;
}

bool Gracz::operator>(const Gracz& inny)
{
	if (ilosc_wygranych > inny.ilosc_wygranych)
		return true;
	else
		return false;
}

Gracz& Gracz::operator=(const Gracz& inny)
{
	printf("Gracz& Gracz::operator=(const Gracz& inny)\n");
	if (jid != NULL)
		delete[] jid;
	jid = strdup(inny.jid);
	ilosc_wygranych = inny.ilosc_wygranych;
	ilosc_przegranych = inny.ilosc_przegranych;
	ilosc_remisow = inny.ilosc_remisow;

	return *this;
}

/*********************************Klasa Ranking*********************************/
Ranking::Ranking(const char* nazwa_pliku)
{
	if (nazwa_pliku != NULL)
		sciezka_do_pliku = strdup(nazwa_pliku);
	else
		sciezka_do_pliku = NULL;
}

Ranking::Ranking(const Ranking& inny)
{
	if (inny.sciezka_do_pliku != NULL)
		sciezka_do_pliku = strdup(inny.sciezka_do_pliku);
	else
		sciezka_do_pliku = NULL;
}

Ranking::~Ranking()
{
	if (sciezka_do_pliku != NULL)
		delete[] sciezka_do_pliku;
}

void Ranking::Uaktualnij_ranking_gracza(const char* jid, WYNIK_GRY wynik) const
{
	Gracz* gracz = Wczytaj_ranking_gracza(jid);
	char* id = NULL;

	if (gracz == NULL)
	{
		id = Wydziel_id_z_jid(jid);
		gracz = new Gracz(id, 0, 0, 0);
		delete[] id;
	}

	Uaktualnij_ranking_gracza(gracz, wynik);
	delete gracz;
}

void Ranking::Uaktualnij_ranking_gracza(Gracz* gracz, WYNIK_GRY wynik) const
{
	if (wynik == WYGRANA)
		gracz->Dodaj_wygrana();
	if (wynik == PRZEGRANA)
		gracz->Dodaj_przegrana();
	if (wynik == REMIS)
		gracz->Dodaj_remis();
	Uaktualnij_plik_z_rankingiem(gracz);
}

struct Gracz* Ranking::Wczytaj_ranking_gracza(const char* jid) const
{
	FILE* ranking = NULL;
	char bufor[256];
	char* id = NULL;
	Gracz* temp = NULL;

	id = Wydziel_id_z_jid(jid);
	ranking = fopen(sciezka_do_pliku, "r");
	if (ranking == NULL)
		return NULL;
	while (fgets(bufor, 255, ranking))
	{
		temp = new Gracz(bufor);
		if (strcmp(temp->Pobierz_jid(), id) == 0)
		{
			delete[] id;
			return temp;
		}
		else
			delete temp;
	}
	delete[] id;
	return NULL;
}
void Ranking::Uaktualnij_plik_z_rankingiem(Gracz* gracz_do_uaktualnienia) const
{
	FILE* stary_ranking = NULL;
	FILE* nowy_ranking = NULL;
	char bufor[256];
	char nazwa_pliku_tymczasowego[100];
	struct Gracz* temp = NULL;
	char komenda[256];
	bool zapisano = false;

	if (sciezka_do_pliku == NULL)
		return;
	stary_ranking = fopen(sciezka_do_pliku, "r");
	tmpnam(nazwa_pliku_tymczasowego);
	nowy_ranking = fopen(nazwa_pliku_tymczasowego, "w");
	if (stary_ranking != NULL)
	{
		while (fgets(bufor, 256, stary_ranking))
		{
			if (bufor[0] == '#')
			{
				fputs(bufor, nowy_ranking);
				continue;
			}
			temp = new Gracz(bufor);
			if (strcmp(temp->Pobierz_jid(), gracz_do_uaktualnienia->Pobierz_jid()) == 0)
			{
				gracz_do_uaktualnienia->Zapisz_do_pliku(nowy_ranking);
				zapisano = true;
			}
			else
				temp->Zapisz_do_pliku(nowy_ranking);
			delete temp;
		}
		/* to znaczy, ze gracza nie ma jeszcze w rankingu */
		if (zapisano == false)
			gracz_do_uaktualnienia->Zapisz_do_pliku(nowy_ranking);
	}
	/* to znaczy, ze ranking jeszcze nie istnieje, i trzeba stworzyc nowy */
	else
		gracz_do_uaktualnienia->Zapisz_do_pliku(nowy_ranking);
	
	fclose(nowy_ranking);
	if (stary_ranking != NULL)
		fclose(stary_ranking);
	
	sprintf(komenda, "mv %s %s", nazwa_pliku_tymczasowego, sciezka_do_pliku);
	system(komenda);	
}

/**
* @brief Funkcja, ktora wysyla ranking do gracza
*
* Ta funkcja zajmuje sie wysylaniem rankingu do gracza
* @param klient Klient uzywany do wyslania wiadomosci
* @param gracz Gracz, do ktorego wysylamy ranking
* @param plik_z_rankingiem Plik, w ktorym zapisany jest ranking
*/

void Ranking::Wyslij_ranking_do_gracza(Jabber::Klient* klient, const char* odbiorca, const char* gracz) const
{
	Gracz* temp = NULL;
	Gracz** gracze;
	int ilosc = 0;
	unsigned int najdluzsze_jid = 0;
	int i = 0;
	char bufor[256];
	Jabber::Wiadomosc wiadomosc(NULL, odbiorca, NULL, "Ten serwer nie prowadzi rankingu!");
	char* tresc_wiadomosci = NULL;
	FILE* ranking = NULL;
	char liczba_tekst[20];
	char format[100];

	ranking = fopen(sciezka_do_pliku, "r");
	if (ranking == NULL)
	{
		klient->Wyslij_wiadomosc(wiadomosc);
		return;
	}
	if (gracz != NULL)
		ilosc = 1;
	else
	{
		while (fgets(bufor, 255, ranking))
			ilosc++;
	}
	fclose(ranking);
	ranking = fopen(sciezka_do_pliku, "r");	
	gracze = new Gracz*[ilosc];
	i = 0;
	/* wczytanie graczy i znalezienie gracz z najdluzszym jid */
	if (gracz == NULL)
	{
		while (fgets(bufor, 255, ranking))
		{
			temp = new Gracz(bufor);
			if (strlen(temp->Pobierz_jid()) > najdluzsze_jid)
				najdluzsze_jid = strlen(temp->Pobierz_jid());
			gracze[i] = temp;
			i++;
		}
	}
	else
	{
		temp = Wczytaj_ranking_gracza(gracz);
		if (temp == NULL)
		{
			wiadomosc.Zmien_tresc("Tego gracza nie ma jeszcze w rankingu");
			klient->Wyslij_wiadomosc(wiadomosc);
			return;
		}
		gracze[0] = temp;
		najdluzsze_jid = strlen(temp->Pobierz_jid());
	}
	/* sortujemy */
	Posortuj_graczy(gracze, ilosc);
	
	najdluzsze_jid += 2;
	/* tworzenie naglowka tabeli */
	sprintf(format, "\%%%ds", najdluzsze_jid * (-1));	
	sprintf(bufor, format, "\n|Gracz:"); 
	Jabber::Polacz(&tresc_wiadomosci, bufor);
	Jabber::Polacz(&tresc_wiadomosci, "|Wygrane:|Przegrane:|Remisy:|\n");
	/* najglupszy kod, jaki w zyciu napisalem! */
	for (i = 0; i < ilosc; i++)
	{
		temp = gracze[i];
		sprintf(format, "|%%%ds", najdluzsze_jid * (-1));
		sprintf(bufor, format, temp->Pobierz_jid());
		
		sprintf(liczba_tekst, "%d", temp->Pobierz_ilosc_wygranych());
		sprintf(format, "%%s|%%%lds", strlen("Wygrane:") * (-1));
		sprintf(bufor, format, bufor, liczba_tekst);
		
		sprintf(liczba_tekst, "%d", temp->Pobierz_ilosc_przegranych());
		sprintf(format, "%%s|%%%lds", strlen("Przegrane") * (-1));
		sprintf(bufor, format, bufor, liczba_tekst);
		
		sprintf(liczba_tekst, "%d", temp->Pobierz_ilosc_remisow());
		sprintf(format, "%%s|%%%lds", strlen("Remisy") * (-1));
		sprintf(bufor, format, bufor, liczba_tekst);
		
		strcat(bufor, "|\n");
		Jabber::Polacz(&tresc_wiadomosci, bufor);
	}
	wiadomosc.Zmien_tresc(tresc_wiadomosci);
	klient->Wyslij_wiadomosc(wiadomosc);

	/* sprzatamy po sobie */
	for (i = 0; i < ilosc; i++)
		delete gracze[i];
	delete[] tresc_wiadomosci;
}

void Ranking::Zmien_plik_z_rankingiem(const char* nowy_plik)
{
	if (sciezka_do_pliku != NULL)
		delete[] sciezka_do_pliku;
	if (nowy_plik != NULL)
		sciezka_do_pliku = strdup(nowy_plik);
	else
		sciezka_do_pliku = NULL;
}

bool Ranking::Poprawny() const
{
	if (sciezka_do_pliku == NULL)
		return false;
	else
	{
		FILE* plik = fopen(sciezka_do_pliku, "r");
		if (plik != NULL)
			return true;
		else
			return false;
	}
}

const char* Ranking::Pobierz_plik_z_rankingiem() const
{
	return sciezka_do_pliku;
}

/*******************************Funkcje pomocnicze*******************************/
char* Wydziel_id_z_jid(const char* jid)
{
	unsigned int i;

	for (i = 0; i < strlen(jid); i++)
		if (jid[i] == '/')
			break;

	return Jabber::Kopiuj(jid, i);
}

void Posortuj_graczy(Gracz** gracze, int ilosc)
{
	int i;
	Gracz* temp;
	bool skonczono = false;

	while (skonczono == false)
	{
		skonczono = true;
		for (i = 0; i < ilosc - 1; i++)
		{
			if (gracze[i + 1]->operator>(*gracze[i]))
			{
				temp = gracze[i];
				gracze[i] = gracze[i + 1];
				gracze[i + 1] = temp;
				skonczono = false;
			}
		}
	}
}
