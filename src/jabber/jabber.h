#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <list>

// SOCKETS
#define LINUX

#ifdef LINUX

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#endif
#ifdef WINDOWS

#include <windows.h>
#include <winsock2.h>

#endif

#include "funkcje.h"
#include "md5.h"
#include "base64.h"
#include "jabber_wiadomosc.h"
#include "jabber_obecnosc.h"
#include "jabber_iq.h"
#include "parser/parser_xml.h"
#include "parser_wiadomosci.h"

#define CNONCE_LEN 4
#define bufor_rozmiar 2048


#ifndef JABBER_H
#define JABBER_H

namespace Jabber
{

enum WYNIK {OK1 = 0, ERROR2 = 1};
enum METODA_SZYFROWANIA {PLAIN = 0, MD5 = 1};
enum STAN_WYSYLANIA {OCZEKIWANIE_NA_ZGODE, ROZPOCZYNANIE_WYSYLANIA, WYSYLANIE, OCZEKIWANIE_NA_POTWIERDZENIE};

#ifdef LINUX
void* Obsulga_pakietow_linux(void *jabber_klient);
#endif
#ifdef WINDOWS
DWORD WINAPI Obsluga_pakietow_windows(
  LPVOID jabber_klient
);
#endif

/* Klasa klienta Jabbera */
class Klient
{
    #ifdef LINUX
	friend void* Jabber::Obsulga_pakietow_linux(void *jabber_klient);
	#endif
	#ifdef WINDOWS
	friend DWORD Obsluga_pakietow_windows(LPVOID jabber_klient);
	#endif
	
	public:
		typedef void (*callback_wiadomosc) (Jabber::Klient* klient, Wiadomosc& wiadomosc, void* dane);
		typedef void (*callback_obecnosc) (Jabber::Klient* klient, Obecnosc& obecnosc, void* dane);
		
		Klient(const char* serwer, const char* uzytkownik);
		Klient(const Klient& inny);
		~Klient();

		enum Jabber::WYNIK Polacz_z_serwerem();
		enum Jabber::WYNIK Zaloguj(const char* haslo, const char* zasob);
		void Wyloguj();	
		enum Jabber::WYNIK Wyslij_wiadomosc(const Strofa& strofa);
		enum Jabber::WYNIK Zmien_status(enum Jabber::Obecnosc::STATUS status, const char* opis);
		void Podlacz_funkcje_odbioru_wiadomosci(callback_wiadomosc funkcja, void* dane);
		void Podlacz_funkcje_odbioru_obecnosci(callback_obecnosc funckja, void* dane);
		void Dodaj_parser(Parser_wiadomosci<Wiadomosc>* parser);
		void Dodaj_parser(Parser_wiadomosci<Obecnosc>* parser);
		void Dodaj_parser(Parser_wiadomosci<IQ>* parser);
		const char* Pobierz_jid() const;
		void Dzialaj();
		//void Wyslij_plik(const char* odbiorca, const char* opis, const char* sciezka);
		void Wyslij_do_serwera(const char* tekst) const;
		void Zapytaj_o_mozliwosci(const char* odbiorca);		 
		
	private:
		char* nazwa_uzytkownika; /*! Sama nazwa uzytkownika(NIE JID!) */
		char* adres_serwera; /*! Adres serwera np. jabber.snc.ru */
		int sock; /*! Socket wykorzystywany do komunikacji */
		enum Jabber::METODA_SZYFROWANIA szyfrowanie; /*! Metoda szyfrowania uzywana do autoryzacji */
		char* jid; /*! Jid zalogowanego uzytkownika */
		void* dane_wiadomosc; /*! Dane przekazywane do funkcji typu "callback" */
		void* dane_obecnosc; /*! dane dla funkcje odebrano_obecnosc */
		bool zalogowany; /*! Okresla, czy uzywany klient jest zalogowany */
		bool polaczony; /*! Okresla, czy klient jest polaczony z serwerem */
		bool zatrzymaj_funkcje_do_obslugi_pakietow; /*! Sluzy do chwilowego zatrzymania funkcji do obslugi pakietow, uzywane przez funkcje do wysylania wiadomosci */
		std::list<Parser_wiadomosci<Wiadomosc>* > parsery_wiadomosci; /* parsery sluzace do parsowania wiadomosci */
		std::list<Parser_wiadomosci<Obecnosc>* > parsery_obecnosci;
		std::list<Parser_wiadomosci<IQ>* > parsery_iq;
		
		#ifdef LINUX
		pthread_t watek_obslugujacy; /*! Watek, ktory obsluguje pakiety przychodzace od serwera */
		#endif
		#ifdef WINDOWS
		HANDLE watek_obslugujacy;
		#endif
	
		/* callbacks */
		Jabber::Klient::callback_wiadomosc odebrano_wiadomosc; /*! Funkcja, ktora jest wywolywana, gdy klient odbierze od serwera wiadomosc od innego uzytkownika */		
		Jabber::Klient::callback_obecnosc odebrano_obecnosc; /*! Funkcja, ktora jest wywolywana, gdy klient otrzyma informacje o obecnosci innego uzytkownika, lub prosbe o subskrypcje */

		/* prywatne funkcje */
		enum Jabber::WYNIK Stworz_sesje();
		void Przeanalizuj_otrzymany_pakiet(const char* tekst);
		enum Jabber::WYNIK Binduj_zasob(const char* zasob);
		enum Jabber::WYNIK Uruchom();
};

}
#endif
