#include "funkcje.h"

char* Jabber::Kopiuj(const char* const zrodlo, int ilosc)
{
	if (zrodlo == NULL)
		return NULL;
	char* wynik = (char*) malloc(sizeof(char) * (ilosc + 1));
	int i;
	for (i = 0; i < ilosc; i++)
		wynik[i] = zrodlo[i];
	wynik[i] = '\0';
	return wynik;
}

void Jabber::Dodaj_znak(char* cel, char znak)
{
	sprintf(cel, "%s%c", cel, znak);
}

void Jabber::Utnij_znak_nowej_lini(char* tekst)
{
	char* pozycja = NULL;
	char* temp;
	
	pozycja = strchr(tekst, '\n');
	if (pozycja == NULL)
		return;
	temp = Kopiuj(tekst, pozycja - tekst);
	strcpy(tekst, temp);
	if (strlen(pozycja) > 1)
	{
		pozycja++;
		strcat(tekst, pozycja);
	}
	free(temp);
}

char* Jabber::Wyciagnij_nazwe_ze_sciezki_linux(const char* sciezka)
{
	int dlugosc = strlen(sciezka);
	int i;
	for (i = dlugosc; i >= 0; i--)
	{
		if (sciezka[i] == '/')
			break;
	}
	return Kopiuj(sciezka + i + 1, strlen(sciezka + i + 1));
}

void Jabber::Polacz(char** cel, const char* tekst)
{
	int dlugosc = my_strlen(*cel) + strlen(tekst) + 1;
	char* wynik = (char*) calloc(dlugosc, sizeof(char));
	if (*cel != NULL)
	{
		strcpy(wynik, *cel);
		free(*cel);
	}
	strcat(wynik, tekst);
	*cel = wynik;
}

int Jabber::my_strlen(const char* tekst)
{
	if (tekst == NULL)
		return 0;
	else
		return strlen(tekst);
}
