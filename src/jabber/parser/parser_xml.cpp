#include "parser_xml.h"


void Polacz(char** cel, const char* tekst)
{
	int dlugosc = (*cel != NULL ? strlen(*cel) : 0) + strlen(tekst) + 1;
	char* wynik = (char*) calloc(dlugosc, sizeof(char));
	if (*cel != NULL)
	{
		strcpy(wynik, *cel);
		free(*cel);
	}
	strcat(wynik, tekst);
	*cel = wynik;
}

void Usun(char* tekst, int pozycja, int ile)
{
	int dlugosc = strlen(tekst) - ile;
	char temp[dlugosc];

	strncpy(temp, tekst, pozycja);
	temp[pozycja] = '\0';
	strcat(temp, tekst + pozycja + ile);
	strcpy(tekst, temp);
}

Wezel::Wezel(const char* nowa_nazwa, const char* nowy_tekst, Wezel* nast, Wezel* nowe_dziecko,
		      Wezel* nowy_rodzic, Wezel* pop) : poprawny(true)
{
	if (nowa_nazwa != NULL)
		nazwa = strdup(nowa_nazwa);
	else
	{
		nazwa = NULL;
		poprawny = false;
	}
	if (nowy_tekst != NULL)
	{
		if (nowe_dziecko == NULL) /* wezly z dziecmi nie maja tekstu */
			tekst = strdup(nowy_tekst);
	}
	else
		tekst = NULL; /* np. <przyklad/> -> nie ma tekstu, to nie blad */
	nastepny = nast;
	dziecko = nowe_dziecko;
	poprzedni = pop;
	rodzic = nowy_rodzic;
}

Wezel::Wezel(char* xml, Wezel* nowy_rodzic, Wezel* pop) : 
			nastepny(NULL), dziecko(NULL),
			rodzic(nowy_rodzic),poprzedni(pop), nazwa(NULL),
			tekst(NULL), poprawny(true)
{
	unsigned int i = 0;
	char ogranicznik;
	char* pozycja = NULL;
	char* kopia = NULL;
	char* pozycja2 = NULL;
	char* nazwa_atrybutu = NULL;
	char* tresc_atrybutu = NULL;
	char* temp = NULL;
	bool koniec = false;	

	try
	{
		if (xml == NULL)
			throw 1;
		kopia = xml;
		kopia = strchr(kopia, '<');
		if (kopia == NULL)
			throw 1;
		/* kopiujemy nazwe znacznika */
		while ((kopia[i] != ' ') && (kopia[i] != '>' && kopia[i] != '/'))
		{
			i++;
		}
		nazwa = new char[i];
		memset(nazwa, 0x00, i);
		strncpy(nazwa, kopia + 1, i - 1);
		pozycja = kopia + i;
	
		i = 0;
		while (koniec == false)
		{
			/* kopiowanie atrybutow znacznika */
			if (pozycja[i] == ' ')
			{
				// znajdowanie nazwy atrybutu
				pozycja++;
				while (pozycja[i] != '=')
				{
					i++;
					if (i == strlen(pozycja))
						throw 1;
				}
				nazwa_atrybutu = new char[i + 1];
				memset(nazwa_atrybutu, 0x00, i + 1);
				strncpy(nazwa_atrybutu, pozycja, i);
				pozycja += i + 1;
				ogranicznik = ' ';
				if (pozycja[0] == '"')
				{
					ogranicznik = '"';
					pozycja++;				
				}
				if (pozycja[0] == '\'')
				{
					ogranicznik = '\'';
					pozycja++;
				}
				i = 0;			
				while ((pozycja[i] != ogranicznik) && !(ogranicznik == ' ' && (pozycja[i] == '>' || pozycja[i] == '/')))
				{
					i++;
					if (i == strlen(pozycja))
						throw 1;
				}
				tresc_atrybutu = new char[i + 1];
				memset(tresc_atrybutu, '\0', i + 1);
				strncpy(tresc_atrybutu, pozycja, i);
				Dodaj_atrybut(nazwa_atrybutu, tresc_atrybutu);
				pozycja += i;
				if (ogranicznik == '\"' || ogranicznik == '\'')
					pozycja++;
				delete[] nazwa_atrybutu;
				delete[] tresc_atrybutu;
				i = 0;
			}
			else
				koniec = true;
		}
		/* jesli znacznik nie jest lisciem */
		if (pozycja[i] != '/')
		{
			pozycja++;
			Usun_komentarz_XML(pozycja);			
			pozycja2 = strchr(pozycja, '<');
			if ((pozycja2 != NULL) && (pozycja2 != strstr(pozycja, "</")))
				dziecko = new Wezel(pozycja2, this, NULL);
			temp = new char[strlen(nazwa) + 5];
			sprintf(temp, "</%s>", nazwa);
			pozycja2 = strstr(pozycja, temp);
			if (pozycja2 == NULL)
				throw 1;
			else
				Usun_biale_znaki(pozycja2);
			tekst = new char[(pozycja2 - pozycja) + 3];
			memset(tekst, '\0', (pozycja2 - pozycja) + 3);
			strncpy(tekst, pozycja, pozycja2 - pozycja);
			Usun(xml, 0, (pozycja2 + strlen(temp) - xml));
			delete[] temp;
			Usun_biale_znaki(xml);
			Usun_komentarz_XML(xml);			
			if ((xml[0] == '<') && (xml[1] != '/'))
				nastepny = new Wezel(xml, rodzic, this);
		}
		else
		{
			pozycja += 2;
			Usun_biale_znaki(pozycja);
			Usun_komentarz_XML(pozycja);			
			temp = strchr(pozycja, '<');
			if ((temp != NULL) && temp[1] != '/')
			{
				nastepny = new Wezel(pozycja, rodzic, NULL);
			}
		}
		if (dziecko != NULL)
		{
			/* wezly z dziecmi nie maja tekstu */
			delete[] tekst;
			tekst = NULL;
		}
	}
	catch(int)
	{
		poprawny = false;
		throw;
	}
}

Wezel::Wezel(const Wezel& inny) : nastepny(NULL), dziecko(NULL), nazwa(NULL), tekst(NULL)
{
	if (inny.nazwa != NULL)
		nazwa = strdup(inny.nazwa);
	if (inny.tekst != NULL)
		tekst = strdup(inny.tekst);
	if (inny.nastepny != NULL)
		nastepny = inny.nastepny->Klonuj(inny.rodzic, this);
	if (inny.dziecko != NULL)
		dziecko = inny.dziecko->Klonuj(this, NULL);
	poprawny = inny.poprawny;
	rodzic = inny.rodzic;
	poprzedni = inny.poprzedni;
	id = inny.id;
	atrybuty = inny.atrybuty;
}

Wezel::~Wezel()
{
	if (nazwa != NULL)
		delete[] nazwa;
	if (tekst != NULL)
		delete[] tekst;
	if (nastepny != NULL)
		delete nastepny;
	if (dziecko != NULL)
		delete dziecko;
}

void Wezel::Dodaj_atrybut(const char* nazwa, const char* tresc)
{
	atrybuty[nazwa] = tresc;
}

void Wezel::Zmien_dziecko(Wezel* wezel)
{
	if (dziecko != NULL)
		delete dziecko;
	if (wezel != NULL)
	{
		dziecko = wezel->Klonuj(this, NULL);
		if (tekst != NULL)
		{
			delete[] tekst;
			tekst = NULL;
		}
	}
	else
		dziecko = NULL;
}

void Wezel::Zmien_nastepny(Wezel* wezel)
{
	if (nastepny != NULL)
		delete nastepny;
	if (wezel != NULL)
		nastepny = wezel->Klonuj(rodzic, this);
	else
		nastepny = NULL;
}

void Wezel::Dodaj_nastepny(const Wezel* nowy_nastepny)
{
	if (nowy_nastepny == NULL)
		return;
	if (nastepny != NULL)
		nastepny->Dodaj_nastepny(nowy_nastepny);
	else
		nastepny = nowy_nastepny->Klonuj(rodzic, this);
}

void Wezel::Dodaj_dziecko(const Wezel* nowe_dziecko)
{
	if (nowe_dziecko == NULL)
		return;
	if (dziecko != NULL)
		dziecko->Dodaj_nastepny(nowe_dziecko);
	else
		dziecko = nowe_dziecko->Klonuj(this, NULL);
}

void Wezel::Zmien_id(unsigned int& nowe_id)
{
	id = nowe_id;
	nowe_id++;
	if (dziecko != NULL)
	{
		dziecko->Zmien_id(nowe_id);
		nowe_id++;
	}
	if (nastepny != NULL)
	{
		nastepny->Zmien_id(nowe_id);
		nowe_id++;
	}
}

void Wezel::Zmien_tekst(const char* nowy_tekst)
{
	if (dziecko == NULL)
	{
		if (tekst != NULL)
			delete[] tekst;
		if (nowy_tekst != NULL)
			tekst = strdup(nowy_tekst);
		else
			tekst = NULL;
	}
}

Wezel* Wezel::Nastepny() const
{
	return nastepny;
}

Wezel* Wezel::Dziecko() const
{
	return dziecko;
}

Wezel* Wezel::Znajdz(const char* nazwa)
{
	Wezel* wynik = NULL;
	if (nazwa != NULL)
	{
		if (strcmp(nazwa, this->nazwa) == 0)
			return this;
	}
	if (dziecko != NULL)
		wynik = dziecko->Znajdz(nazwa);
	if (wynik != NULL)
		return wynik;	
	if (nastepny != NULL)
		wynik = nastepny->Znajdz(nazwa);
	if (wynik)
		return wynik;

	return NULL;
}

Wezel* Wezel::Znajdz(unsigned int id_wezla)
{
	Wezel* wynik = NULL;

	if (id_wezla < id)
	{
		if (rodzic != NULL)
			return rodzic->Znajdz(id_wezla);
	}
	if (id_wezla == id)
		return this;
	if (nastepny != NULL)
		wynik = nastepny->Znajdz(id_wezla);
	if (wynik)
		return wynik;
	if (dziecko != NULL)
		wynik = dziecko->Znajdz(id_wezla);
	if (wynik != NULL)
		return wynik;

	return NULL;
}

Wezel* Wezel::Znajdz_dziecko(const char* nazwa)
{
	if (dziecko == NULL)
		return NULL;
	else
		return dziecko->Znajdz(nazwa);
}

Wezel* Wezel::Znajdz_nastepny(const char* nazwa)
{
	if (nastepny == NULL)
		return NULL;
	else
		return nastepny->Znajdz(nazwa);
}

Wezel* Wezel::Klonuj(Wezel* nowy_rodzic, Wezel* pop) const
{
	Wezel* wynik = new Wezel(nazwa, tekst, NULL, NULL, NULL, NULL);
	
	wynik->poprzedni = pop;
	wynik->rodzic = nowy_rodzic;	
	wynik->atrybuty = atrybuty;
	if (dziecko != NULL)
		wynik->Zmien_dziecko(dziecko);
	if (nastepny != NULL)
		wynik->Zmien_nastepny(nastepny);
	return wynik;
}

void Wezel::Znajdz_wezly(std::list<Wezel*>& lista, const char* nazwa)
{
	if (poprawny == false)
		return;
	if (nazwa != NULL)
	{
		if (strcmp(nazwa, this->nazwa) == 0)
			lista.push_back(this);
	}
	if (nastepny != NULL)
		nastepny->Znajdz_wezly(lista, nazwa);
	if (dziecko != NULL)
		dziecko->Znajdz_wezly(lista, nazwa);
}

void Wezel::Znajdz_dzieci(std::list<Wezel*>& lista, const char* nazwa)
{
	if (dziecko == NULL)
		return;
	else
		dziecko->Znajdz_wezly(lista, nazwa);
}

void Wezel::Znajdz_nastepne(std::list<Wezel*>& lista, const char* nazwa)
{
	if (nastepny == NULL)
		return;
	else
		nastepny->Znajdz_wezly(lista, nazwa);
}

const char* Wezel::Pobierz_atrybut(std::string nazwa) const
{	
	std::map<std::string, std::string>::const_iterator it = atrybuty.find(nazwa);
	if (it != atrybuty.end())
		return it->second.c_str();
	else
		return NULL;
}

std::map<std::string, std::string> Wezel::Pobierz_atrybuty() const
{
	return atrybuty;
}

const char* Wezel::Pobierz_nazwe() const
{
	return nazwa;
}

const char* Wezel::Pobierz_tekst() const
{
	return tekst;
}

unsigned int Wezel::Pobierz_id() const
{
	return id;
}

bool Wezel::Czy_poprawny() const
{
	return poprawny;
}

std::string Wezel::Zamien_na_XML(int wciecie) const
{
	std::string wynik;
	int i;

	wynik.clear();
	if (poprawny == false)
		return wynik;
	/* wciecia dla znacznika otwierajacego */
	for (i = 0; i < wciecie; i++)
		wynik.append("\t");
	wynik += '<';
	wynik += nazwa;
	if (atrybuty.size() > 0)
	{
		std::map<std::string, std::string>::const_iterator it = atrybuty.begin();
		while (it != atrybuty.end())
		{
			wynik += ' ' + it->first + "=\'" + it->second + '\'';
			it++;
		}
	}
	if ((tekst == NULL) && (dziecko == NULL))
		wynik += "/>\n";
	else
	{
		wynik += '>';
		if (tekst != NULL)
			wynik += tekst;
		if (dziecko != NULL)
		{
			wynik += "\n" + dziecko->Zamien_na_XML(wciecie + 1);
			/* wciecia dla znacznika zamykajacego */
			for (i = 0; i < wciecie; i++)
				wynik.append("\t");					
		}\
		wynik += "</";
		wynik += nazwa;
		wynik += '>';
		wynik += '\n';
	}
	if (nastepny != NULL)
		wynik += nastepny->Zamien_na_XML(wciecie);
	return wynik;
}

char* Wezel::Omin_biale_znaki(char* wskaznik) const
{
	unsigned int i = 0;
	unsigned int k = 0;
	unsigned int j = 0;
	bool koniec = false;
	char biale_spacje[] = {' ', '\n', '\t', '\r', '\201', '\001'};

	if (wskaznik == NULL)
		return NULL;
	/* jedziemy od przodu */
	while (koniec == false)
	{
		koniec = true;
		for (j = 0; j < strlen(biale_spacje); j++)
			if (wskaznik[i] == biale_spacje[j])
				koniec = false;
		if (koniec == false)
			i++;
	}
	/* jedziemy od tylu */
	koniec = false;
	k = strlen(wskaznik) - 1;
	while (koniec == false)
	{
		koniec = true;
		for (j = 0; j < strlen(biale_spacje); j++)
			if (wskaznik[k] == biale_spacje[j])
			{
				wskaznik[k] = '\0';
				koniec = false;
			}
		k--;
	}
	return wskaznik + i;
}

void Wezel::Usun_biale_znaki(char* wskaznik) const
{
	unsigned int i = 0;
	int k = 0;
	unsigned int j = 0;
	bool koniec = false;
	char biale_spacje[] = {' ', '\n', '\t', '\r', '\201', '\001'};

	if (wskaznik == NULL)
		return;
	/* jedziemy od przodu */
	while ((koniec == false) && (i < strlen(wskaznik)))
	{
		koniec = true;
		for (j = 0; j < strlen(biale_spacje); j++)
			if (wskaznik[i] == biale_spacje[j])
				koniec = false;
		if (koniec == false)
			i++;
	}
	/* jedziemy od tylu */
	koniec = false;
	k = (strlen(wskaznik) == 0 ? 0 : strlen(wskaznik) - 1);
	while ((koniec == false) && (k >= 0))
	{
		koniec = true;
		for (j = 0; j < strlen(biale_spacje); j++)
			if (wskaznik[k] == biale_spacje[j])
			{
				wskaznik[k] = '\0';
				koniec = false;
			}
		k--;
	}
	Usun(wskaznik, 0, i);
}

void Wezel::Usun_komentarz_XML(char* xml) const
{
	/* omijamy komentarze */
	while (strstr(xml, "<!--") == xml)
	{
		char* pozycja = strstr(xml, "-->");
		if (pozycja == NULL)
			throw 1;
		else
		{
			Usun(xml, 0, (pozycja + strlen("-->")) - xml);
			Usun_biale_znaki(xml);
		}
	}
}

/*********************************Parser_XML***********************************/

Parser_XML::Parser_XML(const Wezel* nowy_korzen): bufor(NULL), nazwa_pliku(NULL),
	poprawny(true)
{
	korzen = nowy_korzen->Klonuj(NULL, NULL);
	Rozdziel_id_wezlom();
}

Parser_XML::Parser_XML(const char* xml): bufor(NULL), nazwa_pliku(NULL), poprawny(true),
										korzen(NULL)										
{
	Parsuj(xml);
}

Parser_XML::Parser_XML() : bufor(NULL), nazwa_pliku(NULL), poprawny(true), korzen(NULL)
{
}

Parser_XML::~Parser_XML()
{
	if (nazwa_pliku != NULL)
		delete[] nazwa_pliku;
	if (bufor != NULL)
		delete[] bufor;
	if (korzen != NULL)
		delete korzen;
}

void Parser_XML::Przeladuj_plik(const char* plik)
{
	if (plik != NULL)
	{
		if (nazwa_pliku != NULL)
			delete[] nazwa_pliku;
		nazwa_pliku = strdup(plik);
	}
	Wczytaj_plik();
	Zbuduj_drzewo_dokumentu();
}

void Parser_XML::Parsuj(const char* xml)
{
	if (bufor != NULL)
		delete[] bufor;
	bufor = strdup(xml);
	Zbuduj_drzewo_dokumentu();
}

void Parser_XML::Usun_wezel(unsigned int id_wezla)
{
	Wezel* wezel = korzen->Znajdz(id_wezla);
	Wezel* nastepny = NULL;
	Wezel* rodzic = NULL;

	if (wezel != NULL)
	{
		nastepny = wezel->nastepny;
		if (nastepny != NULL)
			nastepny->poprzedni = wezel->poprzedni;
		if (wezel->poprzedni != NULL)
			wezel->poprzedni->nastepny = nastepny;
		rodzic = wezel->rodzic;
		if (rodzic != NULL)
		{
			if (rodzic->dziecko == wezel)
				rodzic->dziecko = nastepny;
		}
	}
	if (korzen == wezel)
		korzen = NULL;
	wezel->nastepny = NULL;
	delete wezel;
	Rozdziel_id_wezlom();
}

const char* Parser_XML::Pobierz_tresc_atrybutu(const char* znacznik, const char* atrybut) const
{
	const Wezel* wezel = Znajdz_wezel(znacznik);
	if (wezel != NULL)
		return wezel->Pobierz_atrybut(atrybut);
	else
		return NULL;
}

const char* Parser_XML::Pobierz_tresc_elementu(const char* znacznik) const
{
	const Wezel* wezel = Znajdz_wezel(znacznik);
	if (wezel != NULL)
		return wezel->Pobierz_tekst();
	else
		return NULL;
	
}

Wezel* Parser_XML::Znajdz_wezel(const char* nazwa) const
{
	if (korzen != NULL)
		return korzen->Znajdz(nazwa);
	else
		return NULL;
}

void Parser_XML::Znajdz_wezly(std::list<Wezel*>& lista,
                              const char* nazwa) const
{
	lista.clear();
	korzen->Znajdz_wezly(lista, nazwa);
}

Wezel* Parser_XML::Pobierz_korzen() const
{
	return korzen;
}

void Parser_XML::Zmien_korzen(const Wezel* nowy_korzen)
{
	if (korzen != NULL)
		delete korzen;
	korzen = nowy_korzen->Klonuj(NULL, NULL);
	Rozdziel_id_wezlom();
}

bool Parser_XML::Czy_poprawny() const
{
	return poprawny;
}

std::string Parser_XML::Zamien_na_XML() const
{
	std::string wynik;

	wynik.clear();
	if (korzen != NULL)
		wynik = korzen->Zamien_na_XML(0);
	return wynik;
}

void Parser_XML::Zapisz_do_pliku(const char* nazwa_pliku)
{
	std::string xml = Zamien_na_XML();
	FILE* plik = fopen(nazwa_pliku, "w");

	if (plik != NULL)
		fputs(xml.c_str(), plik);
	fclose(plik);
}

void Parser_XML::Zbuduj_drzewo_dokumentu()
{
	if (korzen != NULL)
		delete korzen;
	try
	{
		if (bufor != NULL)	
			korzen = new Wezel(bufor, NULL, NULL);
		else
		{
			korzen = NULL;
			throw 1;
		}
		Rozdziel_id_wezlom();		
	}
	catch(int)
	{
		poprawny = false;
	}
}

void Parser_XML::Wczytaj_plik()
{
	FILE* plik = fopen(nazwa_pliku, "r");
	char temp[1024];

	if (bufor != NULL)
	{
		delete bufor;
		bufor = NULL;
	}	
	if (plik == NULL)
		return;
	else
	{
		while (fgets(temp, 1024, plik))
		{
			Polacz(&bufor, temp);
		}		
	}
	fclose(plik);
}

void Parser_XML::Rozdziel_id_wezlom()
{
	unsigned int id = 1;
	
	if (korzen != NULL)
		korzen->Zmien_id(id);
}

Wezel* Parser_XML::Znajdz(unsigned int id_wezla)
{
	if (korzen != NULL)
		return korzen->Znajdz(id_wezla);
	else
		return NULL;
}
