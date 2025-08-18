#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#define SCREEN_W 1024
#define SCREEN_H 768
#define FPS 60
#define GRASS_H 60
#define CT_H 80
#define CT_W 40
#define TR_H 40
#define TR_W 60
#define LIN 1
#define COLS 1

typedef struct personagem_ct 
{
    float x;
    float vel;
    int dir, esq;
    ALLEGRO_COLOR cor_personagem;
} personagem;

typedef struct personagem_tr 
{
    float x, y;
    float x_vel;
    float y_vel;
    int vivo;
    ALLEGRO_COLOR cor_tr;
} terrorista;

typedef struct tiro
{
    float x, y;
    float vel;
    int ativo;
} tiro;

void info_personagem(personagem *p) 
{
    p->x = SCREEN_W / 2;
    p->cor_personagem = al_map_rgb(0, 0, 254);
    p->vel = 5;
    p->dir = 0;
    p->esq = 0;
}

void info_terroristas(terrorista trs[LIN][COLS], personagem p) 
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            trs[i][j].x = j * (TR_W + 10);
            trs[i][j].y = i * (TR_H + 10);
            trs[i][j].x_vel = p.vel / 2;
            trs[i][j].y_vel = TR_H;
            trs[i][j].vivo = 1;
            trs[i][j].cor_tr = al_map_rgb(255, 178, 102);
        }
    }
}

void draw_personagem(personagem p) 
{
    float y_base = SCREEN_H - GRASS_H / 2;
    al_draw_filled_triangle(p.x, y_base - CT_H, p.x - CT_W / 2, y_base, p.x + CT_W / 2, y_base, p.cor_personagem);
}

void update_personagem(personagem *p) 
{
    if (p->dir) {
        p->x -= p->vel;
        if (p->x <= 0) p->x = 0;
    }
    if (p->esq) {
        p->x += p->vel;
        if (p->x >= SCREEN_W) p->x = SCREEN_W;
    }
}

void draw_terroristas(terrorista trs[LIN][COLS]) 
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo)
                al_draw_filled_rectangle(trs[i][j].x, trs[i][j].y, trs[i][j].x + TR_W, trs[i][j].y + TR_H, trs[i][j].cor_tr);
        }
    }
}

void update_terroristas(terrorista trs[LIN][COLS]) 
{
    int mudar_linha = 0;

    // verifica se algum terrorista vai bater
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!trs[i][j].vivo) continue;
            float prox_x = trs[i][j].x + trs[i][j].x_vel;

            if (prox_x < 0 || prox_x + TR_W > SCREEN_W) {
                mudar_linha = 1;
                break;
            }
        }
        if (mudar_linha) break;
    }

    // atualiza movimento
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!trs[i][j].vivo) continue;
            if (mudar_linha) {
                trs[i][j].y += trs[i][j].y_vel;
                trs[i][j].x_vel *= -1;
            }
            trs[i][j].x += trs[i][j].x_vel;
        }
    }
}

int invasao_terrorista(terrorista trs[LIN][COLS]) //verifica se os terroristas invadiram
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo && trs[i][j].y + TR_H >= SCREEN_H - GRASS_H)
                return 1;
        }
    }
    return 0;
}

int todos_mortos(terrorista trs[LIN][COLS]) 
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo) return 0;
        }
    }
    return 1;
}

void draw_scenario() 
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(0, 240, 0));
}

void tela_fim_derrota()
{

}

void inicializa_tiro(tiro *t) 
{
    t->x = 0;
    t->y = 0;
    t->vel = 6;
    t->ativo = 0;
}

void atualiza_tiro(tiro *t) 
{
    if (t->ativo) {
        t->y -= t->vel;
        if (t->y < 0) t->ativo = 0;
    }
}

void desenha_tiro(tiro t) 
{
    if (t.ativo)
        al_draw_filled_rectangle(t.x - 2, t.y, t.x + 2, t.y + 10, al_map_rgb(255, 255, 255));
}

void checa_colisao(tiro *t, terrorista trs[LIN][COLS]) 
{
    if (!t->ativo) return;

    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo &&
                t->x >= trs[i][j].x &&
                t->x <= trs[i][j].x + TR_W &&
                t->y >= trs[i][j].y &&
                t->y <= trs[i][j].y + TR_H) {
                trs[i][j].vivo = 0;
                t->ativo = 0;
                return;
            }
        }
    }
}



int main() 
{
    //objeto tela
    ALLEGRO_DISPLAY *display = NULL;
    //objeto da fila de eventos
    ALLEGRO_EVENT_QUEUE *event_queue = NULL; //em ponteiro para ser utilizado mais de uma vez
	//objeto do temporizador
	ALLEGRO_TIMER *timer = NULL;

    //objeto de imagem
    ALLEGRO_BITMAP *fundo_derrota = NULL;
    ALLEGRO_BITMAP *fundo_vitoria = NULL;


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

    //inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon())
    {
		fprintf(stderr, "failed to initialize image module!\n");
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

    //inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
    //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *font = al_load_font("ARIAL.TTF", 36, 0);   
	if(font == NULL)
    {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

    //inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon())
    {
		fprintf(stderr, "failed to load tff font module!\n");
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
    terrorista trs[LIN][COLS];

    info_personagem(&p_ct);
    info_terroristas(trs, p_ct);

    tiro t_ct;
    inicializa_tiro(&t_ct);

    int playing = 1;
    int game_over = 0;
    int victory = 0;

    al_start_timer(timer);

    while (playing)
	{
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER)
		{
            if (!game_over && !victory)
			{
                draw_scenario();
                draw_personagem(p_ct);
                draw_terroristas(trs);
                desenha_tiro(t_ct);
                update_personagem(&p_ct);
                update_terroristas(trs);

                atualiza_tiro(&t_ct);
                checa_colisao(&t_ct, trs);

                if (invasao_terrorista(trs))
				{
                    game_over = 1;
                }
				else if (todos_mortos(trs))
				{
                    victory = 1;
                }
            }
			else
			{
    			draw_scenario();
    			if (game_over)
                {
                    fundo_derrota = al_load_bitmap("img/terrorist-wins.jpg");
                    al_draw_bitmap(fundo_derrota, 0, 0, 0);
                }
    			else if (victory)
                {
        		    al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "VOCÊ VENCEU!");
                    fundo_vitoria = al_load_bitmap("img/you-win.jpg");
                    al_draw_bitmap(fundo_vitoria, 0, 0, 0);
                }
   				al_flip_display();

    			// Espera o usuário apertar uma tecla ou fechar a janela
    			while (1) 
				{
        			ALLEGRO_EVENT ev2;
        			al_wait_for_event(event_queue, &ev2);

        			if (ev2.keyboard.keycode == 67 || ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
					{
            		playing = 0;
            		break;
        			}
    			}
			}
            al_flip_display();
        }
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
            playing = 0;
        }	//SE A TECLA FOR SOLTA
    	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			if(ev.keyboard.keycode == 1 || ev.keyboard.keycode == 82)
				p_ct.dir = 1;
			
			if(ev.keyboard.keycode == 4 || ev.keyboard.keycode == 83)
				p_ct.esq = 1;

            if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE && !t_ct.ativo) 
            {
                t_ct.x = p_ct.x;
                t_ct.y = SCREEN_H - GRASS_H - CT_H;
                t_ct.ativo = 1;
            }

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
	

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font);
    al_destroy_bitmap(fundo_derrota);

    return 0;
}
