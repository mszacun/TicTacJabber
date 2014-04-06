#include "jabber.h"

const int jabber_port = 5222;

/**
*   @brief Konstruktor klasy Jabber::Klient
*   @param serwer Serwer, z ktorym bedziemy sie laczyc
* 	@param uzytkonik Nazwa uzytkownika na serwerz (NIE JID!)
*/
Jabber::Klient::Klient(const char* serwer, const char* uzytkownik): nazwa_uzytkownika(NULL),
adres_serwera(NULL), sock(0), szyfrowanie(PLAIN), jid(NULL), zalogowany(false),
polaczony(false)
{
	if (uzytkownik != NULL)
		nazwa_uzytkownika = strdup(uzytkownik);
	if (serwer != NULL)
		adres_serwera = strdup(serwer);
	odebrano_wiadomosc = NULL;
	odebrano_obecnosc = NULL;
	zatrzymaj_funkcje_do_obslugi_pakietow = false;
	dane_wiadomosc = NULL;
	dane_obecnosc = NULL;
}

Jabber::Klient::Klient(const Klient& inny)
{
	if (inny.nazwa_uzytkownika != NULL)
		nazwa_uzytkownika = strdup(inny.nazwa_uzytkownika);
	else
		nazwa_uzytkownika = NULL;
	if (inny.adres_serwera != NULL)
		adres_serwera = strdup(inny.adres_serwera);
	else
		adres_serwera = NULL;
	szyfrowanie = inny.szyfrowanie;
	if (inny.jid != NULL)
		jid = strdup(inny.jid);
	else
		jid = NULL;
	watek_obslugujacy = inny.watek_obslugujacy;
	dane_wiadomosc = inny.dane_wiadomosc;
	dane_obecnosc = inny.dane_obecnosc;
	zalogowany = inny.zalogowany;
	polaczony = inny.polaczony;
	zatrzymaj_funkcje_do_obslugi_pakietow = inny.zatrzymaj_funkcje_do_obslugi_pakietow;
	odebrano_wiadomosc = inny.odebrano_wiadomosc;
	parsery_wiadomosci = inny.parsery_wiadomosci;
	parsery_obecnosci = inny.parsery_obecnosci;
	parsery_iq = inny.parsery_iq;
}

/**
* @brief Destruktor klasy Jabber::Klient
*/
Jabber::Klient::~Klient()
{
	if (zalogowany == true)
		Wyloguj();
	if (nazwa_uzytkownika != NULL)
		delete[] nazwa_uzytkownika;
	if (adres_serwera != NULL)
		delete[] adres_serwera;
	if (jid != NULL)
		delete[] jid;
}

/**
*   @brief Funkcja, ktora laczy klienta z serwerem
*
*   Ta funkcja laczy klienta z serweram, za pomoca protokol TCP/IP.
*   Kolejnym krokiem po polaczeniu jest logowanie. Mozna to zrobic za pomoca funkcji Jabber::Klient::Zalogouj()
*   @return OK gdy sie uda, ERROR w przeciwnym wypadku
*/
enum Jabber::WYNIK Jabber::Klient::Polacz_z_serwerem()
{
	struct hostent *server;
	struct sockaddr_in serv_addr;	
	
	char wiadomosc_do_wyslania[bufor_rozmiar];
	char wiadomosc_odebrana[bufor_rozmiar];
	
	int wynik;
	char metoda_szyfrowania[50];

	memset(metoda_szyfrowania, '\0', 50);
	#ifdef WINDOWS
	WORD wVersionRequested;
    WSADATA wsaData;
    
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
    #endif

	server = gethostbyname(adres_serwera);
	if (server == NULL)
		return Jabber::ERROR2;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5222);

	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

	
	wynik = connect(sock,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
	
	if (wynik != 0)
		return Jabber::ERROR2;

	/* pakiet powitalny i utowrzenie nowego strumienia(stream) */
	sprintf(wiadomosc_do_wyslania,
	       "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' to='%s' version='1.0'>",
	        adres_serwera);
	Wyslij_do_serwera(wiadomosc_do_wyslania);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar - 1, 0);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar - 1, 0);

	/* wybranie odpowiedniego mechanizmu uwierzytelniania */
	/* FIXME: Dodac nowe sposoby uwierzytelniania */
	if (strstr(wiadomosc_odebrana, "DIGEST-MD5") != NULL)
		szyfrowanie = MD5;
	else
		return Jabber::ERROR2;
	polaczony = true;
	return Jabber::OK1;
}


/**
* @brief Funkcja logujec naszego uzytkownika i binduje mu zasob
*
* Funkcja loguje naszego uzytkownika i rezerwuje mu zasob. Jesli chcesz, aby zasob zostal wygenerowany prze serwer,
* nalezy jako zasob przekazac NULL
* @param klient Klient, ktory zostanie zalogowany
* @param haslo Haslo naszego uzytkownika
* @param zasob Zasob, ktory chcemy zbindowac klientowi, moze byc NULL
*/
enum Jabber::WYNIK Jabber::Klient::Zaloguj(const char* haslo, const char* zasob)
{
	md5 _md5;
	int i = 0;
	char* pozycja = NULL;
	char realm[25];
	char nonce[25];
	char cnonce[CNONCE_LEN*8 + 1];
	unsigned char a1_h[16], a1[33], a2[33], response_value[33];
	char *response, *response_coded;
	char* odkodowane_wyzwanie;
	const char* wyzwanie = NULL;
	char wiadomosc_do_wyslania[bufor_rozmiar];
	char wiadomosc_odebrana[bufor_rozmiar];
	Parser_XML parser;

	if (polaczony == false)
		return Jabber::ERROR2;

	memset(realm, '\0', 25);
	memset(wiadomosc_odebrana, '\0', bufor_rozmiar);
	memset(nonce, '\0', 25);
	
	if (szyfrowanie == MD5)
	{
		strcpy(wiadomosc_do_wyslania, "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>");
		Wyslij_do_serwera(wiadomosc_do_wyslania);
	}
	else
		return Jabber::ERROR2;
	recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);

	/* czas na challange */
	if (strstr(wiadomosc_odebrana, "failure"))
	    return Jabber::ERROR2;
	parser.Parsuj(wiadomosc_odebrana);
	wyzwanie = parser.Pobierz_tresc_elementu("challenge");
	odkodowane_wyzwanie = base64_decode(wyzwanie);
	if ((pozycja = strstr(odkodowane_wyzwanie, "realm=")))
	{
		pozycja += strlen("realm=\"");
		i = 0;
		while (pozycja[i] != '"')
		{
			Jabber::Dodaj_znak(realm, pozycja[i]);
			i++;
		}
	}
	else
		strcpy(realm, adres_serwera);
	if ((pozycja = strstr(odkodowane_wyzwanie, "nonce")))
	{
		pozycja += strlen("nonce=\"");
		i = 0;
		while (pozycja[i] != '"')
		{
			Jabber::Dodaj_znak(nonce, pozycja[i]);
			i++;
		}
	}
	else
		return Jabber::ERROR2;

	for (i = 0; i < CNONCE_LEN; ++i)
	sprintf (cnonce + i*8, "%08x", rand());

	_md5.Hash((const unsigned char*)nazwa_uzytkownika, strlen(nazwa_uzytkownika), 0);
	_md5.Hash((const unsigned char*)":", 1, 0);
	_md5.Hash((const unsigned char*)realm, strlen(realm), 0);
	_md5.Hash((const unsigned char*)":", 1, 0);
	_md5.Hash((const unsigned char*)haslo, strlen(haslo), 1);
	_md5.Digest(a1_h);

	_md5.Reset();
	_md5.Hash((const unsigned char*)a1_h, 16, 0);
	_md5.Hash((const unsigned char*)":", 1, 0);
	_md5.Hash((const unsigned char*)nonce, strlen(nonce), 0);
	_md5.Hash((const unsigned char*)":", 1, 0);
	_md5.Hash((const unsigned char*)cnonce, strlen(cnonce), 1);
	_md5.Print((char*)a1);

	_md5.Reset();
	_md5.Hash((const unsigned char*)"AUTHENTICATE:xmpp/", 18, 0);
	_md5.Hash((const unsigned char*)adres_serwera, strlen(adres_serwera), 1);
	_md5.Print((char*)a2);

	_md5.Reset();
	_md5.Hash((const unsigned char*)a1, 32, 0);
	_md5.Hash((const unsigned char*)":", 1, 0);
	_md5.Hash((const unsigned char*)nonce, strlen(nonce), 0);
	_md5.Hash((const unsigned char*)":00000001:", 10, 0);
	_md5.Hash((const unsigned char*)cnonce, strlen(cnonce), 0);
	_md5.Hash((const unsigned char*)":auth:", 6, 0);
	_md5.Hash((const unsigned char*)a2, 32, 1);
	_md5.Print((char*)response_value);

	i = strlen(nazwa_uzytkownika) + strlen (realm) +
		strlen(nonce) + strlen(adres_serwera) +
		CNONCE_LEN*8 + 136;
	response = new char[i];
	if (!response) return Jabber::ERROR2;

	sprintf (response, "username=\"%s\",realm=\"%s\",nonce=\"%s\""
		",cnonce=\"%s\",nc=00000001,qop=auth,digest-uri=\""
		"xmpp/%s\",response=%s,charset=utf-8",
		nazwa_uzytkownika, realm, nonce, cnonce,
		adres_serwera, response_value);

	response_coded = base64_encode (response, 0);
	if (response_coded) 
	{
		sprintf(wiadomosc_do_wyslania, "%s%s%s", "<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>", response_coded,
		        "</response>");
	}
	else
		return Jabber::ERROR2;
	Wyslij_do_serwera(wiadomosc_do_wyslania);
	memset(wiadomosc_odebrana, '\0', bufor_rozmiar);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
	delete[] response_coded;	
	delete[] response;
	delete[] odkodowane_wyzwanie;
	pozycja = strchr(wiadomosc_odebrana, '>');
	if (strstr(wiadomosc_odebrana, "failure"))
		return Jabber::ERROR2;
	parser.Parsuj(wiadomosc_odebrana);
	wyzwanie = parser.Pobierz_tresc_elementu("challenge");
	odkodowane_wyzwanie = base64_decode(wyzwanie);
	if (strstr(odkodowane_wyzwanie, "rspauth") == NULL)
		return Jabber::ERROR2;
	strcpy(wiadomosc_do_wyslania, "<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
	Wyslij_do_serwera(wiadomosc_do_wyslania);
	memset(wiadomosc_odebrana, '\0', bufor_rozmiar);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
	if (strstr(wiadomosc_odebrana, "success"))
	{
		if (Binduj_zasob(zasob) == Jabber::OK1)
		{
			if (Stworz_sesje() == Jabber::OK1)
			{
				if (Uruchom() == Jabber::OK1)
				{
					zalogowany = true;
					Wyslij_do_serwera("<presence/>");
					return Jabber::OK1;
				}
				else
					return Jabber::ERROR2;
			}
			else
				return Jabber::ERROR2;
		}
		else
			return Jabber::ERROR2;
	}
	else
		return Jabber::ERROR2;
}

/**
* @brief Wylogowywuje klienta
*
* Ta funkcja wylogowywuje klienta i ustawia jego status na niedostepny. Gdy klient nie jest ci juz potrzebny wywolaj Jabber_klient_zniszcz.
*/
void Jabber::Klient::Wyloguj()
{
	Wyslij_do_serwera("</stream>");
	close(sock);
	zalogowany = false;
	polaczony = false;
	
	#ifdef LINUX
	       pthread_cancel(watek_obslugujacy);
    #endif
    #ifdef WINDOWS
           TerminateThread(watek_obslugujacy, 0);
    #endif
}

/**
* @brief Funkcja Wysyla wiadomosc
* @param wiadomosc Wiadomosc, ktora chcemy wyslac
* @return OK, gdyw wwysylanie sie powiedzie, ERROR w przeciwynm wypadku
*/
enum Jabber::WYNIK Jabber::Klient::Wyslij_wiadomosc(const Jabber::Strofa& strofa)
{
	char* wiadomosc_do_wyslania;
	
	if (zalogowany == false)
		return Jabber::ERROR2;

	wiadomosc_do_wyslania = strofa.Zamien_na_XML();
	if (wiadomosc_do_wyslania == NULL)
		return Jabber::ERROR2;
	Wyslij_do_serwera(wiadomosc_do_wyslania);
	delete[] wiadomosc_do_wyslania;
	return Jabber::OK1;
}

/**
* @brief Funkcja zmienia status uzytkownika
* 
* Funkcja zmienia status wybranego uzytkownika. Jesli opis jest ustawiony na NULL po zmiane satusu nie bedziemy mieli opisu
* @param klient Klient, ktoremu chcemy zmienic status
* @param nowy_status Statu, jaki chcemy ustawic
* @param opis Opis, jaki chcemy ustawic, moze byc NULL
* @return OK, jesli uda sie zmienic status, ERROR w przypadku bledu
*/
enum Jabber::WYNIK Jabber::Klient::Zmien_status(enum Jabber::Obecnosc::STATUS nowy_status, const char* opis)
{
	Obecnosc obecnosc(NULL, NULL, nowy_status, opis);
	
	Wyslij_wiadomosc(obecnosc);
	return Jabber::OK1;
}

/**
* @brief Funkcja rejestruje funkcje do odbiru wiadomosci
* 
* Funkcja rejestruje funkcje, ktora jest uzywana do odboiru wiadomosci do naszego klienta. Gdy zostanie odebrana wiadomosc,
* funkcja zajmujaca sie obsluga pakietow od serwera wywola ustawiona przez nasz funkcje do odbioru wiadomosciz nastepujacymi parametrami:
* @n klient- klient, ktory otrzymal wiadomosc
* @n wiadomosc - wiadomosc, ktora otrzymal uzytkownik
* @n dane - dane, ktore zostaly ustawione ta funkcja 
* @param funkcja Funkcja, ktorej chcemy uzyc do odbierania wiadomosci
* @param dane Dane, ktore sa przekazywane do funkcji odbierajacej wiadomosci jako trzeci parametr
*/
void Jabber::Klient::Podlacz_funkcje_odbioru_wiadomosci(Jabber::Klient::callback_wiadomosc funkcja, void* dane)
{
	odebrano_wiadomosc = funkcja;
	this->dane_wiadomosc = dane;
}

void Jabber::Klient::Podlacz_funkcje_odbioru_obecnosci(Jabber::Klient::callback_obecnosc funkcja, void* dane)
{
	odebrano_obecnosc = funkcja;
	dane_obecnosc = dane;
}

void Jabber::Klient::Dodaj_parser(Jabber::Parser_wiadomosci<Wiadomosc>* parser)
{
	parsery_wiadomosci.push_back(parser);
}

void Jabber::Klient::Dodaj_parser(Jabber::Parser_wiadomosci<Obecnosc>* parser)
{
	parsery_obecnosci.push_back(parser);
}

void Jabber::Klient::Dodaj_parser(Jabber::Parser_wiadomosci<IQ>* parser)
{
	parsery_iq.push_back(parser);
}

/**
* @brief Funkcja zwraca jid klienta
*
* JID zwrocone przez ta funkcje to identyfikator naszego uzytkownika w sieci. Wyglada on tak:
* @code
*   nazwa_uzytkownika@serwer/zasob
* @endcode
* @return jid naszego klienta
*/
const char* Jabber::Klient::Pobierz_jid() const
{
	return jid;
}

/**
* @brief Pozostawia wlaczonego klienta
*
* Ta funkcja powinna byc wywolana, aby utrzymac dzialanie watku, ktory obsluguje pakiety. Po prostu, jesli nie masz gui, ani twoj program nie jest niczym
* zajety, to wywolaj ta funkcje, aby program nie zakonczyl dzialania
*/
void Jabber::Klient::Dzialaj()
{
    #ifdef LINUX
	pthread_join(watek_obslugujacy, NULL);
	#endif
	#ifdef WINDOWS
	WaitForSingleObject(watek_obslugujacy, INFINITE);
	#endif
}

/*!
*	@biref Funkcja, ktora wysyla wiadomosc do serwera
*	@param tekst Wiadomosc(tekst), ktora chcemy wyslac do serwera
*/
void Jabber::Klient::Wyslij_do_serwera(const char* tekst) const
{
	send(sock, tekst, strlen(tekst), 0);
}

/*!
*   @brief Funkcja, sluzaca do tworzenia nowej sesji z serwerem
*	@param klient Klient, ktory wykorzystywany jest do utworznia sesj
*	@return OK, jesli udalo sie stworzyc sesje, lub ERROR w razie wystapienia bledu
*	Ta funkcja wysyla do serwera pakiet o tresci:
*	@code
*		<iq to='adres_serwara' type='set' id='sess_1'><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>
*	@endcode
*/	

enum Jabber::WYNIK Jabber::Klient::Stworz_sesje()
{
	IQ zapytanie(NULL, adres_serwera, Jabber::IQ::SET1);
	char wiadomosc_odebrana[bufor_rozmiar];
	if (polaczony == false)
		return Jabber::ERROR2;

	Wezel* temp = new Wezel("session", NULL, NULL, NULL, NULL, NULL);
	temp->Dodaj_atrybut("xmlns", "urn:ietf:params:xml:ns:xmpp-session");
	zapytanie.Pobierz_korzen()->Dodaj_dziecko(temp);
	delete temp;

	char* xml = zapytanie.Zamien_na_XML();
	Wyslij_do_serwera(xml);
	// delete[] xml;
	
	int ilosc_bajtow = recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
	wiadomosc_odebrana[ilosc_bajtow] = '\0';
	if (strstr(wiadomosc_odebrana, "<error>") == NULL)
		return Jabber::OK1;
	else
		return Jabber::ERROR2;
}

void Jabber::Klient::Zapytaj_o_mozliwosci(const char* odbiorca)
{
	IQ zapytanie(NULL, odbiorca, Jabber::IQ::GET2);
	Wezel* temp = new Wezel("query", NULL, NULL, NULL, NULL, NULL);
	
	temp->Dodaj_atrybut("xmlns", "http://jabber.org/protocol/disco#info");	                    
	zapytanie.Pobierz_korzen()->Dodaj_dziecko(temp);
	delete temp;	                    
	                    
	Wyslij_wiadomosc(zapytanie);
}

void Jabber::Klient::Przeanalizuj_otrzymany_pakiet(const char* tekst)
{
	const char* odbiorca = NULL;
	const char* nadawca = NULL;
	Parser_XML parser(tekst);

	if (parser.Czy_poprawny() == false)
	{
		printf("OSTRZEZENIE: Nie udalo sie przeanalizowac wiadomosci: %s\n", tekst);
		return;
	}
	nadawca = parser.Pobierz_korzen()->Pobierz_atrybut("from");
	odbiorca = parser.Pobierz_korzen()->Pobierz_atrybut("to");	
	if (strcmp(parser.Pobierz_korzen()->Pobierz_nazwe(), "message") == 0)
	{
		/* odebrano wiadomosc */
		const char* tytul = NULL;
		const char* tresc = NULL;
		std::list<Parser_wiadomosci<Wiadomosc>* >::iterator it = parsery_wiadomosci.begin();

		tresc = parser.Znajdz_wezel("body")->Zamien_na_XML(0).c_str();
		Jabber::Wiadomosc wiadomosc(nadawca, odbiorca, tytul, tresc);

		while (it != parsery_wiadomosci.end())
		{
			Parser_wiadomosci<Wiadomosc>* temp = *it;
			if (temp->Parsuj(this, wiadomosc))
				break;
			it++;
		}
		if ((odebrano_wiadomosc != NULL) && (wiadomosc.Czy_poprawna() == true))
			odebrano_wiadomosc(this, wiadomosc, dane_wiadomosc);
	}
	/* strofy typu PRESENCE */
	if (strcmp(parser.Pobierz_korzen()->Pobierz_nazwe(), "presence") == 0)
	{
		const char* typ_tekst = NULL;
		const char* opis = parser.Pobierz_tresc_elementu("status");
		Obecnosc* obecnosc;
		std::list<Parser_wiadomosci<Obecnosc>* >::iterator it = parsery_obecnosci.begin();
		
		typ_tekst = parser.Pobierz_tresc_atrybutu("presence", "type");
		if (typ_tekst != NULL)
			obecnosc = new Obecnosc(nadawca, odbiorca, Obecnosc::Tekst_na_typ(typ_tekst), opis);
		else
		{
			const char* status = parser.Pobierz_tresc_elementu("show");
			const char* priorytet = parser.Pobierz_tresc_elementu("priority");
			obecnosc = new Obecnosc(nadawca, odbiorca, Obecnosc::Tekst_na_status(status),
			                  opis, priorytet != NULL ? atoi(priorytet) : 0);
		}
		while (it != parsery_obecnosci.end())
		{
			Parser_wiadomosci<Obecnosc>* temp = *it;
			if (temp->Parsuj(this, *obecnosc))
				break;
			it++;
		}
		if (odebrano_obecnosc != NULL)
			odebrano_obecnosc(this, *obecnosc, dane_obecnosc);
		delete obecnosc;
	}	
	/* strofy typu IQ */
	if (strcmp(parser.Pobierz_korzen()->Pobierz_nazwe(), "iq") == 0)
	{		
		char* tmp = strdup(tekst);
		IQ iq(nadawca, odbiorca, tmp);
		std::list<Parser_wiadomosci<IQ>* >::iterator it = parsery_iq.begin();
		while (it != parsery_iq.end())
		{
			Parser_wiadomosci<IQ>* temp = *it;
			if (temp->Parsuj(this, iq))
				return;
			it++;
		}
		printf("IQ: %s\n", iq.Zamien_na_XML());
		delete[] tmp;
	}
}

/*!
*
*	@brief Funkcja zajmujaca sie odbieraniem pakietow od serwera
*	@param jabber_klient Klient poloczony z serwerm, ktory bedzie nasluchiwal pakietow
*	@return NULL
*	Ta funkcja, to watek, ktory dziala przez caly czas, kiedy jestesmy polaczeni z serwerem
*	i odbiera pakiety. Nastepnie wywoluje funkcje, ktora analizuje otrzymany pakiet.
*/
#ifdef LINUX
void* Jabber::Obsulga_pakietow_linux(void *jabber_klient)
{
	Jabber::Klient* klient = (Jabber::Klient*) jabber_klient;
	char wiadomosc_odebrana[bufor_rozmiar];
	
	while (klient->polaczony == true)
	{
		if (klient->zatrzymaj_funkcje_do_obslugi_pakietow == false)
		{
			int ilosc_bajtow = recv(klient->sock, wiadomosc_odebrana, bufor_rozmiar, 0);
			wiadomosc_odebrana[ilosc_bajtow] = '\0';
			/* rozpoznanie pakietu... */
			klient->Przeanalizuj_otrzymany_pakiet(wiadomosc_odebrana);
		}
	}
	return NULL;
}
#endif

#ifdef WINDOWS
DWORD Jabber::Obsluga_pakietow_windows(LPVOID jabber_klient)
{
	Jabber::Klient* klient = (Jabber::Klient*) jabber_klient;
	char wiadomosc_odebrana[bufor_rozmiar];
	
	while (klient->polaczony == true)
	{
		if (klient->zatrzymaj_funkcje_do_obslugi_pakietow == false)
		{
			int ilosc_bajtow = recv(klient->sock, wiadomosc_odebrana, bufor_rozmiar, 0);
			wiadomosc_odebrana[ilosc_bajtow] = '\0';
			/* rozpoznanie pakietu... */
			klient->Przeanalizuj_otrzymany_pakiet(wiadomosc_odebrana);
		}
	}
	return 0;
}
#endif

/*!
*	@brief Funkcja, ktora uruchamia klienta
*	@param klient Klient do uruchomienia
*	@return ERROR, jesli nie uda sie uruchomic klienta, OK jesli sie uda.
*	Ta funkcja zajumje sie uruchomieniem klienta. Tak naprawde uruchamia ona glowny watek, ktory zajmujes sie odbieraniem pakietow od serwera
*/
enum Jabber::WYNIK Jabber::Klient::Uruchom()
{
    #ifdef LINUX
	pthread_create(&watek_obslugujacy, NULL, Jabber::Obsulga_pakietow_linux, this);
	#endif
	#ifdef WINDOWS
	watek_obslugujacy = CreateThread(NULL, 0, Jabber::Obsluga_pakietow_windows, this, 0, NULL);
	#endif
	return Jabber::OK1;
}

/*!
*   @brief Funkcja uzywana do bindowania zasobu
*   @param klient Klient, dla ktorego bindujemy zasob
*   @param zasob Zasob, ktory chcmey posiadac. Jesli chcesz, aby to serwer wygenerowal zasob, mozesz dac NULL
*   @return OK jesli uda sie bindowanie zasobu, ERROR w przeciwnym wypadku
*/
enum Jabber::WYNIK Jabber::Klient::Binduj_zasob(const char* zasob)
{
	char wiadomosc_odebrana[bufor_rozmiar];
	char wiadomosc_do_wyslania[bufor_rozmiar];
	char* pozycja = NULL;
	int i;

	if (polaczony == false)
		return Jabber::ERROR2;
	sprintf(wiadomosc_do_wyslania,
	       "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' to='%s' version='1.0'>",
	        adres_serwera);
	Wyslij_do_serwera(wiadomosc_do_wyslania);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
	recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
	if (strstr(wiadomosc_odebrana, "bind"))
	{
		if (zasob != NULL)
			sprintf(wiadomosc_do_wyslania, "<iq type='set' id='bind_2'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>%s</resource></bind></iq>",
			  	    zasob);
		else
			strcpy(wiadomosc_do_wyslania, "<iq type='set' id='bind_1'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'/></iq>");
		Wyslij_do_serwera(wiadomosc_do_wyslania);
		memset(wiadomosc_odebrana, '\0', bufor_rozmiar);
		recv(sock, wiadomosc_odebrana, bufor_rozmiar, 0);
		/* wyluskanie otrzymanego jid */
		if ((pozycja = strstr(wiadomosc_odebrana, "<jid>")))
		{
			pozycja += strlen("<jid>");
			/* obliczenie ilosci znakow w jid */
			i = 0;
			while (pozycja[i] != '<')
				i++;
			/* skopiowanie jid do zmiennej */
			jid = Kopiuj(pozycja, i);
		}
		else
			return Jabber::ERROR2;
	}
	return Jabber::OK1;
}
