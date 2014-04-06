#include <gtk/gtk.h>
#include <jabber.h>
#include <vector>
#include <cstdio>
#include "gra_przez_jabbera.h"

#ifndef GUI_H
#define GUI_H

enum KOLUMNY
{
	GRACZ_1,
	GRACZ_2,
	STAN_GRY_KOLUMNA,
	ILOSC_KOLUMN
};

enum
{
	NADAWCA_KOLUMNA,
	TRESC_KOLUMNA,
	ILOSC
};

class Gui
{
	public:
		Gui(std::vector<Gra*>* nowa_lista);
		~Gui();
		void Dodaj_wiadomosc_do_paska_statusu(const char* wiadomosc);
		void Dodaj_wiadomosc(const Jabber::Wiadomosc& wiadomosc);
		void Pokaz();
		void Pokaz_plansze(GtkTreePath* path);
		void Przygotuj_widok_listy_gier();
		void Zaaktualizuj_liste_gier();
	private:
		GtkWidget* informacje_o_grze;
		GtkWidget* ostatnie_wiadomosci;
		GtkWidget* okno;
		GtkWidget* okno_ze_scrollem;
		GtkWidget* hbox1;
		GtkWidget* vbox1;
		GtkWidget* vbox2;	
		GtkWidget* lista_gier;
		GtkWidget* pasek_stanu;
		std::vector<Gra*>* lista;
};
#endif