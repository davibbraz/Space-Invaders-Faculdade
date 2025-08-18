#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 1024
#define SCREEN_H 768
#define FPS 60
#define GRASS_H 60
#define CT_H 80
#define CT_W 40
#define TR_H 40
#define TR_W 60
#define MAX_TIROS 1
int LIN;
int COLS;
int dif;
int *l, *c;


void dificuldade(int *dif) //altera dificuldade
{
    if(*dif == 1)
    {
        *l = 4;
        *c = 5;
    }
    else if (*dif == 2)
    {
        *l = 6;
        *c = 7;
    }
    else if (*dif == 3)
    {
        *l = 7;
        *c = 10;
    }
}

typedef struct personagem_ct //struct do personagem ct
{
    float x;
    float vel;
    int dir, esq;
    ALLEGRO_COLOR cor_personagem;
} personagem;

typedef struct personagem_tr  //struct do terrorista
{
    float x, y;
    float x_vel;
    float y_vel;
    int lider;
    int vivo;
    ALLEGRO_COLOR cor_tr;
} terrorista;

typedef struct tiro //struct do tiro
{
    float x, y;
    float vel;
    int ativo;
} tiro;

void info_personagem(personagem *p) //atualiza as informações do personagem
{
    p->x = SCREEN_W / 2;
    p->cor_personagem = al_map_rgb(0, 0, 254);
    p->vel = 5;
    p->dir = 0;
    p->esq = 0;
}

void info_terroristas(terrorista trs[LIN][COLS], personagem p) //atualiza as informações dos terroristas
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            trs[i][j].x = j * (TR_W + 30);
            trs[i][j].y = i * (TR_H + 30);
            trs[i][j].x_vel = p.vel / 2;
            trs[i][j].y_vel = TR_H;
            trs[i][j].vivo = 1;
            trs[i][j].cor_tr = al_map_rgb(255, 178, 102);
        }
    }
}

void draw_personagem(personagem p) //desenha personagem ct
{
    float y_base = SCREEN_H - GRASS_H / 2;
    al_draw_filled_triangle(p.x, y_base - CT_H, p.x - CT_W / 2, y_base, p.x + CT_W / 2, y_base, p.cor_personagem);
}

void update_personagem(personagem *p) //atualiza o personagem ct
{
    if (p->dir) {
        p->x -= p->vel;
        if (p->x <= 0) 
            p->x = SCREEN_W;
    }
    if (p->esq) {
        p->x += p->vel;
        if (p->x >= SCREEN_W)
            p->x = 0;
    }
}

void draw_terroristas(terrorista trs[LIN][COLS]) //desenha o terrorista 
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo)
                al_draw_filled_rectangle(trs[i][j].x, trs[i][j].y, trs[i][j].x + TR_W, trs[i][j].y + TR_H, trs[i][j].cor_tr);
        }
    }
}

void update_terroristas(terrorista trs[LIN][COLS]) //atualiza o terrorista
{
    int mudar_linha = 0;

    // verifica se algum terrorista vai bater na borda e muda a linha caso bata na borda
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

int todos_mortos(terrorista trs[LIN][COLS]) //verifica se todos os terroristas 
{
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (trs[i][j].vivo) return 0;
        }
    }
    return 1;
}

void draw_scenario() //desenha cenario
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(160, 160, 160));
}

void gera_tiros(tiro tiros[]) //muda os dados dos tiros
{
    for (int i = 0; i < MAX_TIROS; i++) 
    {
        tiros[i].x = 0;
        tiros[i].y = 0;
        tiros[i].vel = 15;
        tiros[i].ativo = 0;
    }
}


void update_tiros(tiro tiros[]) //atualiza os dados dos tiros
{
    for (int i = 0; i < MAX_TIROS; i++) 
    {
        if (tiros[i].ativo) 
        {
            tiros[i].y -= tiros[i].vel;
            if (tiros[i].y < 0) tiros[i].ativo = 0;
        }
    }
}

void desenha_tiros(tiro tiros[]) //desenha os tiros
{
    for (int i = 0; i < MAX_TIROS; i++) 
    {
        if (tiros[i].ativo)
            al_draw_filled_rectangle(tiros[i].x - 2, tiros[i].y, tiros[i].x + 2, tiros[i].y + 10, al_map_rgb(255, 255, 255));
    }
}

void checa_colisoes_tiros(tiro tiros[], terrorista trs[LIN][COLS], int *pontuacao) //veridica se o tiro atingiu um terrorista
{
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;

        for (int j = 0; j < LIN; j++) {
            for (int k = 0; k < COLS; k++) {
                if (trs[j][k].vivo &&
                    tiros[i].x >= trs[j][k].x &&
                    tiros[i].x <= trs[j][k].x + TR_W &&
                    tiros[i].y >= trs[j][k].y &&
                    tiros[i].y <= trs[j][k].y + TR_H) {
                    
                    trs[j][k].vivo = 0;
                    tiros[i].ativo = 0;
                    *pontuacao += 10;
                    break;
                }
            }
        }
    }
}

int colisao_ct_terrorista(terrorista trs[LIN][COLS], personagem p) //verifica se o terrorista encostou no ct 
{
    //separa os pontos do triangulo que representa o personagem
    float y_ct_topo = SCREEN_H - GRASS_H / 2 - CT_H;
    float y_ct_base = SCREEN_H - GRASS_H / 2;
    float x_ct_esq = p.x - CT_W / 2;
    float x_ct_dir = p.x + CT_W / 2;

    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!trs[i][j].vivo) 
                continue;

            float x_tr = trs[i][j].x;
            float y_tr = trs[i][j].y;
            float x_tr_end = x_tr + TR_W;
            float y_tr_end = y_tr + TR_H;

            // colisão simples
            if (x_tr_end >= x_ct_esq && x_tr <= x_ct_dir &&
                y_tr_end >= y_ct_topo && y_tr <= y_ct_base) 
                {
                return 1; // colisão ocorreu
            }
        }
    }
    return 0;
}




int main() 
{
    //declaração dos ponteiros para mudar a dificuldade
    l = &LIN;
    c = &COLS;
    //objeto tela
    ALLEGRO_DISPLAY *display = NULL;
    //objeto da fila de eventos
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	//objeto do temporizador
	ALLEGRO_TIMER *timer = NULL;

    //objeto de imagem
    ALLEGRO_BITMAP *fundo_derrota = NULL;
    ALLEGRO_BITMAP *fundo_vitoria = NULL;
    ALLEGRO_BITMAP *menu_inicial = NULL;
    ALLEGRO_BITMAP *menu_dificuldade = NULL;

    //sons
    ALLEGRO_SAMPLE *som_vitoria = NULL;
    ALLEGRO_SAMPLE *som_derrota = NULL;


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

    //instala o audio
    if (!al_install_audio())
    {
        fprintf(stderr, "failed to initialize audio!\n");
        return -1;
    }
    //inicializa os codecs de áudio, permite mais formatos do que o padrão
    if (!al_init_acodec_addon()) 
    {
        fprintf(stderr, "failed to initialize audio codecs!\n");
    return -1;
    }
    //reserva espaço na fila de reprodução de sons
    if (!al_reserve_samples(2)) 
    {
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }

    //inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

    //inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon())
    {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
    //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho(segundo parametro)
    ALLEGRO_FONT *font = al_load_font("font/ALLSTAR4.TTF", 30, 0);   
	if(font == NULL)
    {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
    

    //registra na fila os eventos de tela (ex: clicar no X na janela)
 	al_register_event_source(event_queue, al_get_display_event_source(display));
    //registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));


    int menu = 1;
    int sel_dif = 0;
    int playing = 0;
    int game_over = 0;
    int victory = 0;
    int pontuacao = 0;

    //carrega o som
    som_derrota = al_load_sample("sons/TERRORIST-WIN-_Sound-Effect_-_CounterStrike-_1_.wav");
    som_vitoria = al_load_sample("sons/COUNTER-TERRORIST-WIN-_Sound-Effect_-_CounterStrike.wav");

    //inicia timer
    al_start_timer(timer);
    //carrega imagem do menu
    menu_inicial = al_load_bitmap("img/menu_counter_invaders.jpg");

    al_draw_bitmap(menu_inicial, 0, 0, 0);
    al_flip_display();

    while(menu) //tela de menu
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            menu = 0;
            sel_dif = 1;
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            menu = 0;
            
        }
    }

    al_destroy_bitmap(menu_inicial);
    menu_dificuldade = al_load_bitmap("img/menu_dificuldade.jpg");

    al_draw_bitmap(menu_dificuldade, 0, 0, 0);
    al_flip_display();

    while (sel_dif) //tela de selecionar a dificuldade
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev.keyboard.keycode == 28) 
            {
                dif = 1;
            }
            else if(ev.keyboard.keycode == 29) 
            {
                dif = 2;
            }
            else if(ev.keyboard.keycode == 30) 
            {
                dif = 3;
            }

            if (dif > 0)
            {
                dificuldade(&dif);
                playing = 1;
                sel_dif = 0;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sel_dif = 0;
        }
    }

    al_destroy_bitmap(menu_dificuldade);

    //declaração das structs no main
    personagem p_ct;
    terrorista trs[LIN][COLS];

    info_personagem(&p_ct);
    info_terroristas(trs, p_ct);

    tiro tiros[MAX_TIROS];
    gera_tiros(tiros);
    //buffer para os textos
    char buffer[50];

    while (playing) //inicia o jogo
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
                desenha_tiros(tiros);
                update_personagem(&p_ct);
                update_terroristas(trs);
                update_tiros(tiros);
                //placar da pontuação
                sprintf(buffer, "pontuacao: %d", pontuacao);
                al_draw_text(font, al_map_rgb(255, 255, 255), 10, 710, 0, buffer);
                
                checa_colisoes_tiros(tiros, trs, &pontuacao);


                if (invasao_terrorista(trs) || colisao_ct_terrorista(trs, p_ct))
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
                    sprintf(buffer, "sua pontuacao foi: %d", pontuacao);
                    al_draw_text(font, al_map_rgb(255, 255, 255), 353, 80, 0, buffer);
                    al_play_sample(som_derrota, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
    			else if (victory)
                {
                    fundo_vitoria = al_load_bitmap("img/you-win.jpg");
                    al_draw_bitmap(fundo_vitoria, 0, 0, 0);
                    sprintf(buffer, "sua pontuacao foi: %d", pontuacao);
                    al_draw_text(font, al_map_rgb(255, 255, 255), 353, 80, 0, buffer);
                    al_play_sample(som_vitoria, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
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
        }

    	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
			if(ev.keyboard.keycode == 1 || ev.keyboard.keycode == 82)
				p_ct.dir = 1;
			
			if(ev.keyboard.keycode == 4 || ev.keyboard.keycode == 83)
				p_ct.esq = 1;

            if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) 
            {
                for (int i = 0; i < MAX_TIROS; i++) 
                {
                    if (!tiros[i].ativo) 
                    {
                        tiros[i].x = p_ct.x;
                        tiros[i].y = SCREEN_H - GRASS_H - CT_H;
                        tiros[i].ativo = 1;
                        break;
                    }
                }
            }

		}

	else if(ev.type == ALLEGRO_EVENT_KEY_UP) 
		{
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
    al_destroy_bitmap(fundo_vitoria);
    al_destroy_sample(som_vitoria);
    al_destroy_sample(som_derrota);
    

    return 0;
}