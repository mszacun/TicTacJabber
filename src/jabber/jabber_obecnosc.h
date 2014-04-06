#include "jabber_strofa.h"
#include "funkcje.h"
#include <cstring>
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include "parser/parser_xml.h"

#ifndef JABBER_OBECNOSC_H
#define JABBER_OBECNOSC_H

namespace Jabber
{
	class Obecnosc : public Jabber::Strofa
	{
		public:
			enum TYP {NIEDOSTEPNY, PROSBA_O_SUBSKRYPCJE, ZASUBSKRYBOWANY, 
				USUN_SUBSKRYPCJE, KONIEC_SUBSKRYPCJI, PROBA, BLAD, NIEWAZNE};
			enum STATUS {DOSTEPNY, NIE_PRZESZKADZAC, ZARAZ_WRACAM, NIEWAZNE2};

			Obecnosc(const char* nowy_nadawca, const char* nowy_odbiorca,
			         STATUS nowy_status, const char* nowy_opis, int nowy_priorytet = 0);
			Obecnosc(const char* nowy_nadawca, const char* noway_odbiorca,
			         TYP nowy_typ, const char* nowy_opis);
			Obecnosc(const Obecnosc& inna);
			virtual ~Obecnosc();

			virtual char* Zamien_na_XML() const;
			
			static const char* Typ_na_tekst(TYP typ_do_zamiany);
			static TYP Tekst_na_typ(const char* tekst);
			static const char* Status_na_tekst(STATUS status_do_zamiany);
			static STATUS Tekst_na_status(const char* tekst);

			virtual void Zmien_typ(TYP nowy_typ);
			virtual void Zmien_status(STATUS nowy_status);
			virtual void Zmien_priorytet(int nowy_priorytet);
			virtual void Zmien_opis(const char* nowy_opis);

			virtual TYP Pobierz_typ() const;
			virtual STATUS Pobierz_status() const;
			virtual int Pobierz_priorytet() const;
			virtual const char* Pobierz_opis() const;

			/* operatory */
			virtual const Obecnosc& operator==(const Obecnosc& inna);

		private:
			TYP typ;
			STATUS status;
			char* opis;
			char priorytet;			
	};
}	


#endif
