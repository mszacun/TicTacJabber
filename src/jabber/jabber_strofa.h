#ifndef JABBER_STROFA_H
#define JABBER_STROFA_H

#define DLUGOSC_ID 10

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>

#include "funkcje.h"

namespace Jabber
{
	class Strofa
	{
		public:
			Strofa(const char* nowy_nadawca, const char* nowy_odbiorca);
			Strofa(const Strofa& inna);
			virtual ~Strofa();

			virtual char* Zamien_na_XML() const	= 0;

			virtual const char* Pobierz_nadawce() const;
			virtual const char* Pobierz_odbiorce() const;
			virtual const char* Pobierz_id() const;
			virtual void Zmien_nadawce(const char* nowy_nadawca);
			virtual void Zmien_odbiorce(const char* nowy_odbiorca);

			virtual const Strofa& operator=(const Strofa& inna);
		protected:
			char* nadawca;
			char* odbiorca;
			char* id;

			virtual void Wygeneruj_id();
	};
}

#endif
