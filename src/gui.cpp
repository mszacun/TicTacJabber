#include "gui.h"

void Zamknij_okno()
{
	gtk_main_quit();
}

void row_activated_signal(GtkTreeView* tree_view, GtkTreePath* path, GtkTreeViewColumn* column, gpointer dane)
{
	Gui* gui = (Gui*) dane;
	gui->Pokaz_plansze(path);
	gtk_main_iteration_do(false);
}

Gui::Gui(std::vector<Gra*>* nowa_lista) : lista(nowa_lista)
{
	gtk_init(NULL, NULL);
	okno = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	okno_ze_scrollem = gtk_scrolled_window_new(NULL, NULL);
	hbox1 = gtk_hbox_new(FALSE, 5);
	vbox1 = gtk_vbox_new(FALSE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);	
	informacje_o_grze = gtk_text_view_new();
	ostatnie_wiadomosci = gtk_text_view_new();
	pasek_stanu = gtk_statusbar_new();
	Przygotuj_widok_listy_gier();

	gtk_box_pack_start(GTK_BOX(hbox1), vbox1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), lista_gier, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(okno_ze_scrollem), ostatnie_wiadomosci);
	gtk_box_pack_start(GTK_BOX(vbox1), okno_ze_scrollem, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), informacje_o_grze, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), pasek_stanu, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(okno), vbox2);

	g_signal_connect (G_OBJECT (okno), "delete_event",
		      G_CALLBACK (Zamknij_okno), this);
	g_signal_connect(G_OBJECT(lista_gier), "row-activated", G_CALLBACK(row_activated_signal), this);
	
	gtk_text_view_set_editable(GTK_TEXT_VIEW(ostatnie_wiadomosci), FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(informacje_o_grze), FALSE);	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(okno_ze_scrollem), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(okno_ze_scrollem, 400, 300);
}

Gui::~Gui()
{
}

void Gui::Przygotuj_widok_listy_gier()
{
	GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
	
	lista_gier = gtk_tree_view_new();

	/* pierwsz kolumna */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Gracz 1");
	gtk_tree_view_append_column(GTK_TREE_VIEW(lista_gier), col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", GRACZ_1);

	/* druga kolumna */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Gracz 2");
	gtk_tree_view_append_column(GTK_TREE_VIEW(lista_gier), col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", GRACZ_2);

	/* trzecia kolumna */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Aktualny stan gry");
	gtk_tree_view_append_column(GTK_TREE_VIEW(lista_gier), col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", STAN_GRY_KOLUMNA);	

}

void Gui::Pokaz_plansze(GtkTreePath* path) 
{
	GtkTreeIter iter;
    GtkTreeModel* model = NULL;
	char* gracz_1 = NULL;
	char* plansza_tekst = NULL;
	GtkTextBuffer* bufor;
	int pozycja;

	/* znalezienie odpowiedniej planszy */
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(lista_gier));
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, GRACZ_1, &gracz_1, -1);
	pozycja = Znajdz_gre(*lista, gracz_1);
	if (pozycja == -1)
		return;
	Gra* gra = lista->operator[](pozycja);

	plansza_tekst = gra->Pobierz_plansze().Rysuj();

	/* dodanie widoku planszy do okienka tekstowego */
	bufor = gtk_text_view_get_buffer(GTK_TEXT_VIEW(informacje_o_grze));
	gtk_text_buffer_set_text(bufor, plansza_tekst, -1);
	delete[] plansza_tekst;
	gtk_main_iteration_do(false);
}

void Gui::Pokaz()
{
	gtk_window_resize(GTK_WINDOW(okno), 500, 50);
	gtk_widget_show_all(okno);
	gtk_widget_show(okno);
	gtk_main_iteration_do(false);
}

void Gui::Zaaktualizuj_liste_gier()
{
	GtkListStore *liststore = NULL;
	GtkTreeIter   iter;
	char stan_gry[50];
	unsigned int i;

    liststore = gtk_list_store_new(ILOSC_KOLUMN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	for (i = 0; i < lista->size(); i++)
	{
		Gra* gra = lista->operator[](i);
		STAN_GRY stan = gra->Pobierz_stan_gry();
		if (stan == BLAD)
			strcpy(stan_gry, "Wystapil blad");
		if (stan == OCZEKIWANIE)
			strcpy(stan_gry, "Oczekiwanie na akceptacje zaproszenia");
		if (stan == WYSZUKIWANIE_GRACZA)
			strcpy(stan_gry, "Wyszkuje gracza nr 2");
		if (stan == RUCH_1)
			strcpy(stan_gry, "Ruch gracza nr 1");
		if (stan == RUCH_2)
			strcpy(stan_gry, "Ruch gracza nr 2");		
		/* tworzenie modelu */
		gtk_list_store_append(liststore, &iter);
		gtk_list_store_set (liststore, &iter,
	                        GRACZ_1, gra->Pobierz_gracza1(),
	                        GRACZ_2, gra->Pobierz_gracza2(),
		                    STAN_GRY_KOLUMNA, stan_gry,
	                        -1);
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW(lista_gier), GTK_TREE_MODEL(liststore));
	gtk_widget_show(lista_gier);
	g_object_unref(liststore);
	gtk_main_iteration_do(false);
}

void Gui::Dodaj_wiadomosc(const Jabber::Wiadomosc& wiadomosc)
{
	GtkTextBuffer* bufor;
	GtkTextIter iter;
	char tekst[500];

	bufor = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ostatnie_wiadomosci));
	gtk_text_buffer_get_end_iter(bufor, &iter);

	sprintf(tekst, "Otzymano wiadomosc od: %s - %s\n", wiadomosc.Pobierz_nadawce(), wiadomosc.Pobierz_tresc());
	gtk_text_buffer_insert(bufor, &iter, tekst, -1);
	gtk_main_iteration_do(false);
}

void Gui::Dodaj_wiadomosc_do_paska_statusu(const char* wiadomosc)
{
	gtk_statusbar_pop(GTK_STATUSBAR(pasek_stanu), 0);
	gtk_statusbar_push(GTK_STATUSBAR(pasek_stanu), 0, wiadomosc);
	gtk_main_iteration_do(false);
}
