#include <stdio.h>
#include <allegro5/allegro.h>

//tamanho do display
const int SCREEN_W = 960;
const int SCREEN_H = 540;

int main()
{
    //objeto tela
    ALLEGRO_DISPLAY *display = NULL;

    //inicialização do allegro
    if(!al_init()) 
    {
	    fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
    //cria a tela
    display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

    al_clear_to_color(al_map_rgb(0, 102, 204));
    //atualiza a tela (quando houver algo para mostrar)
	al_flip_display();
    al_rest(5);



return 0;
}