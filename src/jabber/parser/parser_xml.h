#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <list>
#include <iostream>
#include <string>

#ifndef PARSER_XML_H
#define PARSER_XML_H

class Parser_XML;

class Wezel
{
	friend class Parser_XML;
	
	public:
		Wezel(const char* nowa_nazwa, const char* nowy_tekst, Wezel* nast, Wezel* nowe_dziecko,
		      Wezel* nowy_rodzic, Wezel* pop);
		Wezel(char* xml, Wezel* nowy_rodzic, Wezel* pop);		
		Wezel(const Wezel& inny);
		~Wezel();

		Wezel* Znajdz(const char* nazwa);
		Wezel* Znajdz(unsigned int id_wezla);
		Wezel* Znajdz_dziecko(const char* nazwa);
		Wezel* Znajdz_nastepny(const char* nazwa);
		Wezel* Klonuj(Wezel* nowy_rodzic, Wezel* pop) const;
		void Znajdz_wezly(std::list<Wezel*>& lista, const char* nazwa);
		void Znajdz_dzieci(std::list<Wezel*>& lista, const char* nazwa);
		void Znajdz_nastepne(std::list<Wezel*>& lista, const char* nazwa);

		void Zmien_dziecko(Wezel* nowe_dziecko);
		void Zmien_nastepny(Wezel* nowy_nastepny);
		void Dodaj_atrybut(const char* nazwa, const char* tresc);		
		void Dodaj_nastepny(const Wezel* nowy_nastepny);
		void Dodaj_dziecko(const Wezel* nowe_dziecko);
		void Zmien_id(unsigned int& nowe_id);
		void Zmien_tekst(const char* nowy_tekst);
		
		Wezel* Nastepny() const;
		Wezel* Dziecko() const;
		const char* Pobierz_atrybut(std::string nazwa) const;
		std::map<std::string, std::string> Pobierz_atrybuty() const;
		const char* Pobierz_nazwe() const;
		const char* Pobierz_tekst() const;
		unsigned int Pobierz_id() const;
		bool Czy_poprawny() const;
		
		std::string Zamien_na_XML(int wciecie) const;
	private:
		Wezel* nastepny;
		Wezel* dziecko;
		Wezel* rodzic;
		Wezel* poprzedni;
		std::map<std::string, std::string> atrybuty;
		char* nazwa;
		char* tekst;
		bool poprawny;
		unsigned int id;

		/* funkcje prywatne */
		char* Omin_biale_znaki(char* wskaznik) const;
		void Usun_biale_znaki(char* wskaznik) const;
		void Usun_komentarz_XML(char* xml) const;
};

class Parser_XML
{
	public:
		Parser_XML();
		Parser_XML(const Wezel* nowy_korzen);
		Parser_XML(const char* xml);
		~Parser_XML();

		void Przeladuj_plik(const char* plik = NULL);
		void Parsuj(const char* xml);
		void Usun_wezel(unsigned int id_wezla);
		
		const char* Pobierz_tresc_atrybutu(const char* znacznik, const char* atrybut) const;
		const char* Pobierz_tresc_elementu(const char* znacznik) const;
		Wezel* Znajdz_wezel(const char* nazwa) const;
		void Znajdz_wezly(std::list<Wezel*>& lista, const char* nazwa) const;
		Wezel* Pobierz_korzen() const;
		void Zmien_korzen(const Wezel* nowy_korzen);
		bool Czy_poprawny() const;
		std::string Zamien_na_XML() const;
		void Zapisz_do_pliku(const char* nazwa_pliku);
	private:
		char* bufor;
		char* nazwa_pliku;
		bool poprawny;
		Wezel* korzen;

		/* funkcje prywatne */
		void Zbuduj_drzewo_dokumentu();
		void Wczytaj_plik();
		void Rozdziel_id_wezlom();
		Wezel* Znajdz(unsigned int id_wezla);
};

#endif
