#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

//tamanho do display

const int SCREEN_W = 928;
const int SCREEN_H = 522;
const float FPS = 200;
const int GRASS_H = 60;
const int ct_h= 80;
const int ct_w = 40;
const int tr_h = 40;
const int tr_w = 60;

typedef struct personagem_ct
{
	float x;
	float vel;
	int dir, esq;
	ALLEGRO_COLOR cor_personagem;
} personagem;

typedef struct pesonagem_tr
{
	float x, y;
	float x_vel;
	float y_vel;
	ALLEGRO_COLOR cor_tr;
} terrorista;

void info_personagem(personagem *p) //passa por ponteiro para conseguir atualizar os parametros
{
	
	p->x = SCREEN_W/2;
	p->cor_personagem = al_map_rgb(0,0,254); //azul
	p->vel = 5;
	p->dir = 0;
	p->esq = 0;
}

void info_terrorista(terrorista *tr, personagem p)
{
	tr->x = 0;
	tr->y = 0;
	tr->x_vel = p.vel/3;
	tr->y_vel = tr_h;
	tr->cor_tr = al_map_rgb(255, 178, 102);
}

void draw_personagem(personagem p)
{
	float y_base = SCREEN_H - GRASS_H/2;
	al_draw_filled_triangle(p.x, y_base - ct_h, p.x - ct_w/2 ,y_base,p.x + ct_w/2,y_base, p.cor_personagem);
}

void update_personagem(personagem *p) // muda a posição do personagem
{
	if(p->dir)
	{
		p->x -= p->vel;
		if(p->x <= 0)
			p->x = 0;
	}
	if(p->esq)
	{
		p->x += p->vel;
		if(p->x >= SCREEN_W)
			p->x = SCREEN_W;
	}
}

void draw_terrorista(terrorista tr)
{
	al_draw_filled_rectangle(tr.x, tr.y, tr_w + tr.x, tr_h+tr.y, tr.cor_tr);
}

void update_terrorista(terrorista *tr)
{
	if(tr->x + tr_w + tr->x_vel > SCREEN_W || tr->x + tr->x_vel < 0)
	{
		tr->y += tr->y_vel;
		tr->x_vel *= -1;
	}
	tr->x += tr->x_vel;
}

int invasao_terrorista(terrorista tr)
{
	if(tr.y + tr_h > SCREEN_H - GRASS_H)
		return 1;
	return 0;
}

void draw_scenario()
{
	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_filled_rectangle( 0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(0, 240, 0));
}

int main()
{
    //objeto tela
    ALLEGRO_DISPLAY *display = NULL;
    //objeto da fila de eventos
    ALLEGRO_EVENT_QUEUE *event_queue = NULL; //em ponteiro para ser utilizado mais de uma vez
	//objeto do temporizador
	ALLEGRO_TIMER *timer = NULL;
//----------------------------------------------------------------------------------------------------\\
    //inicialização do allegro
    if(!al_init()) 
    {
	    fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	

    //cria a tela
    display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	//cria o temporizador
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

    //instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

    //cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) 
    {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
    }
    //registra na fila os eventos de tela (ex: clicar no X na janela)
 	al_register_event_source(event_queue, al_get_display_event_source(display));
    //registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));


	personagem p_ct;

	terrorista tr;

	info_personagem(&p_ct);

	info_terrorista(&tr, p_ct);

    int playing = 1;
	//inicia o temporizador
	al_start_timer(timer);	
    while (playing)
    {
	//cria variavel de evento
    ALLEGRO_EVENT ev;
	//espera por um evento e o armazena na variavel de evento ev
	al_wait_for_event(event_queue, &ev);
    
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
	if(ev.type == ALLEGRO_EVENT_TIMER)
		{
		draw_scenario();

		draw_personagem(p_ct);

		draw_terrorista(tr);
		
		al_flip_display();

		update_personagem(&p_ct);

		update_terrorista(&tr);
			
		if(al_get_timer_count(timer)%(int)FPS == 0)
			printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
    //se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
        {
	        playing = 0;
        }
    //se o tipo de evento for um clique de mouse
	else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}
	//SE A TECLA FOR SOLTA
    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			if(ev.keyboard.keycode == 1 || ev.keyboard.keycode == 82)
				p_ct.dir = 1;
			
			if(ev.keyboard.keycode == 4 || ev.keyboard.keycode == 83)
				p_ct.esq = 1;
		}

	else if(ev.type == ALLEGRO_EVENT_KEY_UP) 
		{
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			if(ev.keyboard.keycode == 1 || ev.keyboard.keycode == 82)
				p_ct.dir = 0;
			
			if(ev.keyboard.keycode == 4 || ev.keyboard.keycode == 83)
				p_ct.esq = 0;
		}
	}
	
    //procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
   
 

return 0;
}