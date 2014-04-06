#ifndef JABBER_WIADOMOSC_H
#define JABBER_WIADOMOSC_H

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "jabber_strofa.h"

namespace Jabber
{

	class Wiadomosc : public Strofa
	{
		public:
			Wiadomosc(const char* nowy_nadawca, const char* nowy_odbiorca, const char* nowy_tytul, const char* nowa_tresc);
			Wiadomosc(const Wiadomosc& inna);
			virtual ~Wiadomosc();
			virtual bool Czy_poprawna() const;
			virtual char* Zamien_na_XML() const;
	
			/* akcesory */
			virtual void Zmien_tresc(const char* nowa_tresc);
			virtual const char* Pobierz_tresc() const;

			virtual const Wiadomosc& operator=(const Wiadomosc& inna);
		private:
			char* tytul;
			char* tresc;
	};
}
#endif

