#include "plansza.h"
#include "gra_przez_jabbera.h"
#include "serwer.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

void Uzycie(const char* nazwa_programu);

int main(int argc, char** argv)
{
	char* plik_konfiguracyjny = NULL;
	int i = 0;

	for (i = 0; i < argc; i++)
	{
		if (strcmp("-f", argv[i]) == 0)
		{
			if (i + 1 < argc)
				plik_konfiguracyjny = argv[i + 1];
		}
	}
	if (plik_konfiguracyjny == NULL)
	{
		printf("Nie podano pliku konfiguracyjnego!\n");
		Uzycie(argv[0]);
		/* FIXME: Trzeba wymyslic domyslne miejsce na plik konfiguracyjny */
		return 0;
	}
	
// 	gtk_init (&argc, &argv);
	Serwer serwer(plik_konfiguracyjny);
	serwer.Uruchom();
	return 0;
}


void Uzycie(const char* nazwa_programu)
{
	printf("\n***********************Kolko i krzyzyk przez jabbera***********************\n");
	printf("*****************************Autor: mandrak******************************\n\n");
	printf("Uzycie: %s [OPCJE]\n", nazwa_programu);
	printf("\t-f plik z zapisana konfiguracja serwera\n");
}