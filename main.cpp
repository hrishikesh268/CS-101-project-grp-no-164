#include <allegro5\allegro.h>
#include <allegro5\allegro_native_dialog.h>
#include <conio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_image.h>
#include<iostream>
#include<fstream>
#include<sstream>

using namespace std;
//Object IDS
enum IDS{PLAYER, BULLET, ENEMY};

	int imageWidth = 0;
	int imageHeight = 0;
	int level=1;            //Game Level(1 OR 2)
	bool firstscreen=true;  //Level 2 has 2 screens,So if it is first screen firstscreen = true
	int bricksize=92;       //number of bricks

void makeBrick(int x , int y);
struct rectangle
{
    int x;
    int y;
    rectangle()
    {
        x=0;
        y=0;
    }
};

//Our Player
struct SpaceShip
{
	int ID;
	int x;
	int y;
	int lives;
	int speed;
	int jump;
	int time;
	int boundx;
	int boundy;
	int score;
};

struct Bullet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
};

struct Comet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int boundx;
	int boundy;
};

int s[5];
void highscores(int score)                                  //Function for checking high score
{
    fstream fin,fout;
    fin.open("high.txt",ios::in);
    for(int i=0;i<5;i++)
        fin.read((char*)&s[i],sizeof(int));
    fin.close();
    for(int i=0;i<5;i++)
        if(score>=s[i])
        {
            for(int j=5;j>i;j--)
            s[j]=s[j-1];
            s[i]=score;
            break;
        }

    fout.open("high.txt",ios::out);

    for(int i=0;i<5;i++)
        fout.write((char*)&s[i],sizeof(int));
    fout.close();
}
/*void resethigh()                                              //Backup for resetting high scores
{
    int s[5]={0,0,0,0,0};
    fstream g;
    g.open("high.dat",ios::out);
    for(int i=0;i<5;i++)
    g.write((char*)&s[i],sizeof(int));
    g.close();
    cout<<"High scores have been reset\n";
}*/
void showhigh()                                                  //Function for displaying high scores
{
    ALLEGRO_FONT *fun=al_load_font("ARCADECLASSIC.ttf",60,NULL);
    ALLEGRO_FONT *fun2=al_load_font("ARCADECLASSIC.ttf",30,NULL);
    fstream fin;
    //cout<<"High scores - \n";
    fin.open("high.txt",ios::in);
    for(int i=0;i<5;i++)
    {
        fin.read((char*)&s[i],sizeof(int));
        ostringstream oss;
        oss<<s[i];
        string k=oss.str();
        al_draw_text(fun2,al_map_rgb(0,255,0),400,200+40*i,ALLEGRO_ALIGN_CENTER,k.data());
    }
    al_draw_text(fun,al_map_rgb(0,0,255),400,100,ALLEGRO_ALIGN_CENTER,"HIGH SCORES");
}

//GLOBALS==============================

const int WIDTH = 599;
const int HEIGHT = 599;
const int h = 60;
const int w = 30;
const int NUM_BULLETS = 5;
const int NUM_COMETS=10;
int score1 = 1800;
int score2 = 5400;

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
const int sFPS = 30;

ALLEGRO_BITMAP *image = NULL;   //ALLEGRO_BITMAP *image will create pointer named 'image' corresponding to it's bitmap
ALLEGRO_BITMAP *fireball = NULL;
ALLEGRO_BITMAP *lost = NULL;
int fireballwidth;
int fireballheight;

//PROTOTYPES

void InitShip(SpaceShip &ship);
void MoveShipUp(SpaceShip &ship,rectangle brick[]);
void MoveShipDown(SpaceShip &ship,rectangle brick[]);
void MoveShipLeft(SpaceShip &ship,rectangle brick[]);
void MoveShipRight(SpaceShip &ship,rectangle brick[]);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip &ship);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize);

void InitComet(Comet comets[], int size);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size,rectangle brick[]);
void CollideComet(Comet comets[], int cSize, SpaceShip &ship,rectangle brick[]);

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   MAIN    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

int main(void)
{
    ALLEGRO_DISPLAY *dis;
    if(!al_init())
        al_show_native_message_box(NULL,NULL,NULL,"ASDF",NULL,NULL);
        begin:
    dis=al_create_display(800,600);
    al_set_window_position(dis,270,80);
    al_set_window_title(dis,"DANGEROUS DAVE ");
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_COLOR eblue=al_map_rgb(44,117,255);
    ALLEGRO_COLOR pc=eblue;
    al_flip_display();
    //Loading font with different sizes
    ALLEGRO_FONT *font=al_load_font("ARCADECLASSIC.ttf",100,NULL);
    ALLEGRO_FONT *fon=al_load_font("ARCADECLASSIC.ttf",60,NULL);
    ALLEGRO_FONT *fon2=al_load_font("ARCADECLASSIC.ttf",30,NULL);
    al_install_mouse();

//************* MENU SCREEN ************
    begin2:
        score1 = 1800;
        score2 = 5400;
        level = 1;
        firstscreen = true;
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font,al_map_rgb(0, 255, 0),400,70,ALLEGRO_ALIGN_CENTER,"DANGEROUS DAVE");
        al_draw_text(fon,eblue,400,220,ALLEGRO_ALIGN_CENTER,"PLAY");
        al_draw_text(fon,eblue,400,280,ALLEGRO_ALIGN_CENTER,"HELP");
        al_draw_text(fon,eblue,400,340,ALLEGRO_ALIGN_CENTER,"HIGHSCORES ");
        al_draw_text(fon,eblue,400,400,ALLEGRO_ALIGN_CENTER,"EXIT");

        al_draw_rectangle(200,230,600,270,eblue,1.0);
        al_draw_rectangle(200,290,600,330,eblue,1.0);
        al_draw_rectangle(200,350,600,390,eblue,1.0);
        al_draw_rectangle(200,410,600,450,eblue,1.0);
        al_flip_display();

        bool don=false;
        int x=10,y=10,movespeed=5;

        ALLEGRO_TIMER *timer=al_create_timer(1.0/60.0);
        ALLEGRO_EVENT_QUEUE *event_queue=al_create_event_queue();
        al_register_event_source(event_queue,al_get_timer_event_source(timer));
        al_register_event_source(event_queue,al_get_display_event_source(dis));
        al_register_event_source(event_queue,al_get_mouse_event_source());

        al_start_timer(timer);

        bool a=false;
        while(!don)
        {
            checked:

                ALLEGRO_EVENT events;
                al_wait_for_event(event_queue,&events);

                if(events.type==ALLEGRO_EVENT_DISPLAY_CLOSE)
                    don=true;
                else if(events.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
                {
                    x=events.mouse.x;
                    y=events.mouse.y;
                    goto check;
                }
        }
                check:
                    {
                        if(x>200&&x<600)
                        {
                            if(y>410&&y<450)
                            {
                                don=true;
                            }
                            else if(y>230&&y<270)
                            {
                                al_clear_to_color(al_map_rgb(0,0,0));
                                al_flip_display();

        //primitive variable
        bool done = false;
        bool redraw = true;
        const int FPS = 60;
        int i=0;

        int sourceX=0;
        int sourceY=0;
        int shipfps=5;
        int timedelay=shipfps;

        //object variables
        SpaceShip ship;
        Bullet bullets[NUM_BULLETS];
        Comet comets[NUM_COMETS];

        //Allegro variables
        ALLEGRO_DISPLAY *display = NULL;
        ALLEGRO_EVENT_QUEUE *event_queue = NULL;
        ALLEGRO_TIMER *timer = NULL;


        if(!al_init())										//initialize Allegro
            return -1;

        al_destroy_display(dis);

        display = al_create_display(WIDTH, HEIGHT);			//create our display object

        if(!display)										//test display object
            return -1;

        al_init_primitives_addon();
        al_install_keyboard();
        al_init_image_addon();
        ALLEGRO_BITMAP *player1=NULL;
        ALLEGRO_BITMAP *fire=NULL;
        ALLEGRO_BITMAP *comet1=NULL;
        ALLEGRO_BITMAP *door=NULL;

        image = al_load_bitmap("images.jpg");       // loading images to corresponding bitmaps
        fireball=al_load_bitmap("firball6.png");
        player1=al_load_bitmap("sp3.png");
        fire=al_load_bitmap("fireline3.jpg");
        comet1=al_load_bitmap("comet.png");
        door=al_load_bitmap("door.png");
        lost = al_load_bitmap("lost.png");

        imageWidth = al_get_bitmap_width(image);
        imageHeight = al_get_bitmap_height(image);
        fireballheight=al_get_bitmap_height(fireball);
        fireballwidth=al_get_bitmap_width(fireball);

        rectangle brick[92];        //brick[] will store the positions of the bricks
        brick[1].x = 30*3  +w/2 ;
        brick[1].y = 60*3+ h/2;
        brick[2].x = 30*7 +w/2 ;
        brick[2].y = 60*3+ h/2 ;
        brick[3].x = 30*11 +w/2 ;
        brick[3].y = 60*3+ h/2 ;
        brick[4].x = 30*15 +w/2 ;
        brick[4].y = 60*3 + h/2;
        brick[5].x = 30*8 +w/2 ;
        brick[5].y = 60*4 + h/2;
        brick[6].x = 30*1 +w/2;
        brick[6].y = 60*5+ h/2;
        brick[7].x = 30*5 +w/2 ;
        brick[7].y = 60*5+ h/2 ;
        brick[8].x = 30*9 +w/2 ;
        brick[8].y = 60*5+ h/2 ;
        brick[9].x = 30*13 +w/2;
        brick[9].y = 60*5+ h/2;
        brick[10].x = 30*17 +w/2 ;
        brick[10].y = 60*5+ h/2;
        brick[11].x = 30*10  +w/2;
        brick[11].y = 60*6+ h/2 ;
        brick[12].x = 30*4  +w/2;
        brick[12].y = 60*7+ h/2;
        brick[13].x = 30*5  +w/2;
        brick[13].y = 60*7+ h/2 ;
        brick[14].x = 30*6  +w/2;
        brick[14].y = 60*7+ h/2;
        brick[15].x = 30*7 +w/2;
        brick[15].y = 60*7+ h/2 ;
        brick[16].x = 30*11 +w/2;
        brick[16].y = 60*7+ h/2;
        brick[17].x = 30*12 +w/2;
        brick[17].y = 60*7+ h/2;
        brick[18].x = 30*13 +w/2;
        brick[18].y = 60*7+ h/2  ;
        brick[19].x = 30*14   +w/2;
        brick[19].y = 60*7+ h/2;
        brick[20].x = 30*15  +w/2 ;
        brick[20].y = 60*7+ h/2;
        brick[21].x = 30*16  +w/2 ;
        brick[21].y = 60*7+ h/2;
        brick[0].x = 30*11 +w/2;
        brick[0].y = 60*8+ h/2;
        int j=0;

        for (i=22;i<42;i++)
        {
            brick[i].x = 30*j + w/2;
            brick[i].y =  h/2  ;
            j++;
        }
        j=0;
        for (i=42;i<62;i++)
        {
           brick[i].x = 30*j + w/2;
           brick[i].y =  540 + h/2 ;
           j++;
        }
        j=0;
        for (i=62;i<72;i++)
        {
           brick[i].x =  w/2 ;
           brick[i].y = 60 * j + h/2  ;
            j++;
        }
        j=0;
        for (i=72;i<82;i++)
        {
           brick[i].x =  570 + w/2;
           brick[i].y = 60 * j + h/2 ;
           j++;
        }
        j=0;

        event_queue = al_create_event_queue();
        timer = al_create_timer(1.0 / FPS);
    //INITIALISATIONS OF STATE OF GAME
        srand(time(NULL));
        InitShip(ship);
        InitBullet(bullets, NUM_BULLETS);
        InitComet(comets, NUM_COMETS);

        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
        al_register_event_source(event_queue, al_get_display_event_source(display));

        switch(level)
        {
        case 1:
            InitShip(ship);
            InitBullet(bullets, NUM_BULLETS);
            InitComet(comets, NUM_COMETS);
            bricksize=92;
            al_start_timer(timer);
            while(!done&&level==1)
            {
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);

                if(ev.type == ALLEGRO_EVENT_TIMER)
                {
                    if(timedelay<=0)timedelay=shipfps;
                    redraw = true;
                    if(keys[UP])
                    {
                        MoveShipUp(ship,brick);
                    }
                    if(keys[LEFT])
                        MoveShipLeft(ship,brick);
                    if(keys[RIGHT])
                        MoveShipRight(ship,brick);

                    MoveShipDown(ship,brick);
                    UpdateBullet(bullets, NUM_BULLETS);
                    StartComet(comets, NUM_COMETS);
                    UpdateComet(comets, NUM_COMETS,brick);
                    CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS);
                    CollideComet(comets, NUM_COMETS, ship, brick);
                    score1--;
                }
                else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                {
                    done = true;
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
                {
                    switch(ev.keyboard.keycode)
                    {
                    case ALLEGRO_KEY_ESCAPE:
                        done = true;
                        break;
                    case ALLEGRO_KEY_UP:
                        keys[UP] = true;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        keys[DOWN] = true;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        keys[LEFT] = true;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        keys[RIGHT] = true;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        keys[SPACE] = true;
                        FireBullet(bullets, NUM_BULLETS, ship);
                        break;
                    }
                }
                else if(ev.type == ALLEGRO_EVENT_KEY_UP)
                {
                    switch(ev.keyboard.keycode)
                    {
                    case ALLEGRO_KEY_ESCAPE:
                        done = true;
                        break;
                    case ALLEGRO_KEY_UP:
                        keys[UP] = false;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        keys[DOWN] = false;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        keys[LEFT] = false;
                        sourceX=0;
                        sourceY=0;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        keys[RIGHT] = false;
                        sourceX=0;
                        sourceY=0;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        keys[SPACE] = false;
                        break;
                    }
                }
                if(redraw && al_is_event_queue_empty(event_queue))
                {
                    redraw = false;
                    if(keys[RIGHT])
                    {
                        sourceY=81.5;
                        if(timedelay==2)sourceX+=28.16;
                        if(sourceX>310)sourceX=0;
                        timedelay--;
                    }
                    if(keys[LEFT])
                    {
                        sourceY=40.75;
                        if(timedelay==shipfps)sourceX+=28.16;
                        if(sourceX>310)sourceX=0;
                        timedelay--;
                    }
                    if(!ship.lives||score1 == 0)
                    {
                        done=0;

                        al_flip_display();
                        al_draw_bitmap(lost, 140, 200, 0);

                        ostringstream l;
                        l<<"Your score is "<<0;
                        string l1 = l.str();
                        al_draw_text(fon2,al_map_rgb(255,255,255),300,440,ALLEGRO_ALIGN_CENTER,l1.data());
                        al_flip_display();

                         al_rest(3);
                        al_destroy_display(display);
                        goto begin;
                    }
                    if((ship.x+ship.boundx<=30*13&&ship.x-ship.boundx>=30*12)&&
                    (ship.y+ship.boundy<=60*9&&ship.y-ship.boundy>=60*8))
                        level=2;

                    al_draw_bitmap_region(player1,sourceX,sourceY,30,40,ship.x-15,ship.y-20,0);

                      ostringstream os;
                      os<<score1;
                      string s = os.str();
                    for (i=0;i<82;i++)
                    {
                        makeBrick( brick[i].x - w/2,  brick[i].y - h/2);
                    }
                      al_draw_text(fon2,al_map_rgb(255,255,255),540,30,ALLEGRO_ALIGN_RIGHT,s.data());

                    DrawBullet(bullets, NUM_BULLETS);
                    for(int i = 0; i <NUM_COMETS; i++)
                    {
                        if(comets[i].live)
                        {
                            al_draw_bitmap_region(comet1,75*((int)(comets[i].x/20)%4),75,75,75,comets[i].x- 37.5,comets[i].y - 37.5, 0);//
                        }
                    }
                    al_draw_scaled_bitmap(door,0,0,450,563,30*12,60*8,30,60,0);

                    al_flip_display();
                    al_clear_to_color(al_map_rgb(0,0,0));
                }
            }

            case 2:
                //INITIALISATION FOR LEVEL 2
                InitBullet(bullets, NUM_BULLETS);
                InitComet(comets, NUM_COMETS);
                InitShip(ship);
                al_clear_to_color(al_map_rgb(0,0,0));
                int sourceF=250,firedir=1;
                if (firstscreen)
                    bricksize=83;

                bool copied=false;
                //brickl21 brickl22 has position of bricks of 1st and 2nd screen respectively
                rectangle brickl21[73];
                int t=0;
                for(int i=0;i<20;i++){
                    brickl21[t].x=i*30;
                    brickl21[t].y=0;
                    t++;
                    brickl21[t].x=i*30;
                    brickl21[t].y=9*60;
                    t++;
                }
                for(int i=1;i<9;i++)
                {
                    brickl21[t].x=0;
                    brickl21[t].y=i*60;
                    t++;
                }
                int btemp[]={12,16,24,31,32,33,34,35,36,37,44,55,67,77,73,82,92,102,112,122,132,142,125,135,197},btemp2[]={2,5,12,15,22,77,85,91,92,93,105,113,122,123,124,125,126,141,143,144,146,151,153,154,156,166};
                for(int i=0;i<25;i++){
                    brickl21[t].x=btemp[i]/10*30;
                    brickl21[t].y=btemp[i]%10*60;
                    t++;
                }
                rectangle brickl22[74];
                t=0;
                for(int i=0;i<20;i++){
                    brickl22[t].x=i*30;
                    brickl22[t].y=0;
                    t++;
                    brickl22[t].x=i*30;
                    brickl22[t].y=9*60;
                    t++;
                }
                for(int i=1;i<9;i++)
                {
                    brickl22[t].x=570;
                    brickl22[t].y=i*60;
                    t++;
                }
                for(int i=0;i<25;i++){
                    brickl22[t].x=btemp2[i]/10*30;
                    brickl22[t].y=btemp2[i]%10*60;
                    t++;
                }
                int score = score1 + score2;
                    al_start_timer(timer);
                while(!done)
                {
                    score = score1 + score2;
                    ALLEGRO_EVENT ev;
                    al_wait_for_event(event_queue, &ev);
                    if (ship.x>560 && firstscreen)
                    {
                        firstscreen=false;
                        ship.x-=12*30;
                        copied =false;
                    }
                    if(!copied && !firstscreen)
                    {   int i=0,j=8;
                        for( i=0;i<74;i++)
                        {
                            brick[i].x=brickl22[i].x+15;
                            brick[i].y=brickl22[i].y+30;
                        }
                        for(i=74;i<84;i++)
                        {
                            brick[i].x=j*30+15;
                            brick[i].y=8*60+30;
                            j++;
                        }
                        brick[84].x=0+15;
                        brick[85].x=45;
                        brick[86].x=75;
                        brick[87].x=105;
                        for(i=0;i<4;i++)
                        {
                            brick[i+84].y=8*60+30;
                        }
                        bricksize=88;
                        copied=true;
                    }


                    if(!firstscreen && ship.x<30)
                    {
                        firstscreen=true;
                        ship.x+=12*30;
                        copied =false;
                    }
                    if(!copied && firstscreen)
                    {
                        int i=0;j=6;
                        for(i=0;i<73;i++)
                        {
                            brick[i].x=brickl21[i].x+15;
                            brick[i].y=brickl21[i].y+30;
                        }
                        for(i=73;i<83;i++)
                        {
                            brick[i].x=j*30+15;
                            brick[i].y=8*60+30;
                            j++;
                        }

                        bricksize=83;
                        copied=true;
                    }
                    //if (ship.x<10)firstscreen=true;

                    if(ev.type == ALLEGRO_EVENT_TIMER)
                    {
                        if(timedelay<=0)timedelay=shipfps;
                        redraw = true;
                        if(keys[UP])
                        {
                            MoveShipUp(ship,brick);
                        }
                        if(keys[LEFT])
                            MoveShipLeft(ship,brick);
                        if(keys[RIGHT])
                            MoveShipRight(ship,brick);

                        MoveShipDown(ship,brick);
                        UpdateBullet(bullets, NUM_BULLETS);
                        StartComet(comets, NUM_COMETS);
                        UpdateComet(comets, NUM_COMETS,brick);
                        CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS);
                        CollideComet(comets, NUM_COMETS, ship,brick);
                        score2--;

                        if((!firstscreen)&&
                        (ship.x+ship.boundx>=540&&ship.x-ship.boundx<=570)&&
                        (ship.y+ship.boundy<=120&&ship.y-ship.boundy>=60))
                            {
                                al_flip_display();
                                al_clear_to_color(al_map_rgb(0,0,0));
                                ostringstream l;
                                l<<"Your score is "<<score;
                                string l2 = l.str();
                                al_draw_text(fon2,al_map_rgb(255,255,255),300,300,ALLEGRO_ALIGN_CENTER,"YOU HAVE WON!!!!!");
                                al_draw_text(fon2,al_map_rgb(255,255,255),300,340,ALLEGRO_ALIGN_CENTER,l2.data());
                                al_flip_display();
                                al_rest(3);
                                done++;
                                highscores(score);
                                al_destroy_display(display);
                                goto begin;
                            }

                        if(!ship.lives||score2 == 0)
                            {
                                al_flip_display();
                                al_clear_to_color(al_map_rgb(0,0,0));
                                ostringstream l;
                                l<<"Your score is "<<score1;
                                string l2 = l.str();
                                al_draw_bitmap(lost, 140, 200, 0);
                                al_draw_text(fon2,al_map_rgb(255,255,255),300,440,ALLEGRO_ALIGN_CENTER,l2.data());
                                al_flip_display();
                                al_rest(3);
                                highscores(score1);
                                done++;
                                al_destroy_display(dis);
                                goto begin;
                            }
                    }
                    else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    {
                        done = true;
                    }
                    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:
                            done = true;
                            break;
                        case ALLEGRO_KEY_UP:
                            keys[UP] = true;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            keys[DOWN] = true;
                            break;
                        case ALLEGRO_KEY_LEFT:
                            keys[LEFT] = true;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            keys[RIGHT] = true;
                            break;
                        case ALLEGRO_KEY_SPACE:
                            keys[SPACE] = true;
                            FireBullet(bullets, NUM_BULLETS, ship);
                            break;
                        }
                    }
                    else if(ev.type == ALLEGRO_EVENT_KEY_UP)
                    {
                        switch(ev.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ESCAPE:
                            done = true;
                            break;
                        case ALLEGRO_KEY_UP:
                            keys[UP] = false;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            keys[DOWN] = false;
                            break;
                        case ALLEGRO_KEY_LEFT:
                            keys[LEFT] = false;
                            sourceX=0;
                            sourceY=0;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            keys[RIGHT] = false;
                            sourceX=0;
                            sourceY=0;
                            break;
                        case ALLEGRO_KEY_SPACE:
                            keys[SPACE] = false;
                            break;
                        }
                    }

                    if(redraw && al_is_event_queue_empty(event_queue))
                    {
                        redraw = false;
                        if(keys[RIGHT])
                    {
                        sourceY=81.5;
                        if(timedelay==shipfps)sourceX+=28.16;
                        if(sourceX>310)sourceX=0;
                        timedelay--;
                    }
                    if(keys[LEFT])
                    {
                        sourceY=40.75;
                        if(timedelay==shipfps)sourceX+=28.16;
                        if(sourceX>310)sourceX=0;
                        timedelay--;
                    }
                    if (sourceF>140)sourceF=95;
                    sourceF+=1;
                    al_draw_bitmap_region(player1,sourceX,sourceY,30,40,ship.x-15,ship.y-20,0);
                    if(firstscreen)al_draw_bitmap_region(fire,0,sourceF,300,60,180,480,0);
                    if(!firstscreen)
                    {
                        al_draw_bitmap_region(fire,0,sourceF,300,60,180-12*30,480,0);
                        al_draw_bitmap_region(fire,0,sourceF-20,300,60,8*30,480,0);
                    }

                        if(firstscreen)for (i=0;i<73;i++)
                        {
                            makeBrick( brick[i].x-w/2,  brick[i].y-h/2);
                        }
                         if(!firstscreen)for (i=0;i<74;i++)
                        {
                            makeBrick( brick[i].x-w/2,  brick[i].y-h/2);
                        }
                        if(!firstscreen)
                             al_draw_scaled_bitmap(door,0,0,450,563,30*18,60,30,60,0);

                        ostringstream o;
                        o<<score2;
                        string s2 = o.str();
                        al_draw_text(fon2,al_map_rgb(255,255,255),540,30,ALLEGRO_ALIGN_RIGHT,s2.data());
                        DrawBullet(bullets, NUM_BULLETS);
                        DrawComet(comets, NUM_COMETS);
                        for(int i = 4; i <NUM_COMETS; i++)
                        {
                            if(comets[i].live)
                            {
                                al_draw_bitmap_region(comet1,75*((int)(comets[i].x/20)%4),75,75,75,comets[i].x- 37.5,comets[i].y - 37.5, 0);//
                            }
                        }
                        al_flip_display();
                        al_clear_to_color(al_map_rgb(0,0,0));
                    }
                }

            break;
        }
        //DESTOYING DISPLAY OBJECTS
            al_destroy_bitmap(image);
            al_destroy_bitmap(door);
            al_destroy_bitmap(fireball);
            al_destroy_bitmap(player1);
            al_destroy_bitmap(fire);
            al_destroy_bitmap(comet1);
            al_destroy_bitmap(lost);
            al_destroy_event_queue(event_queue);
            al_destroy_timer(timer);
            al_destroy_display(display);

    }
        //******* HELP *******
        else if(y>290&&y<330)
        {
            al_clear_to_color(al_map_rgb(0,0,0));
            al_flip_display();
            al_draw_filled_circle(90, 85, 5, al_map_rgb(0, 255, 0));
            al_draw_filled_circle(90, 210, 5, al_map_rgb(0, 255, 0));
            //al_draw_filled_circle(90, 270, 5, al_map_rgb(0, 255, 0));
            al_draw_filled_circle(90, 170, 5, al_map_rgb(0, 255, 0));
            al_draw_text(fon2,eblue,100,70,ALLEGRO_ALIGN_LEFT,"THE OBJECTIVE OF THE GAME IS  TO DODGE ALL THE ");
            al_draw_text(fon2,eblue,100,95,ALLEGRO_ALIGN_LEFT,"COMETS AND MONSTERS TO REACH THE EXIT IN THE ");
            al_draw_text(fon2,eblue,100,120,ALLEGRO_ALIGN_LEFT,"MINIMUM TIME POSSIBLE THUS SCORING MAXIMUM");

            al_draw_text(fon2,eblue,100,155,ALLEGRO_ALIGN_LEFT,"SCORE IS SHOWN ON TOP RIGHT OF SCREEN");

            al_draw_text(fon2,eblue,100,195,ALLEGRO_ALIGN_LEFT,"USE THE ARROW KEYS AS CONVENTION TO MOVE ");
            al_draw_text(fon ,al_map_rgb(0, 255, 0),400,260,ALLEGRO_ALIGN_CENTER,"CREDITS");

            al_draw_text(fon2,eblue,100,320,ALLEGRO_ALIGN_LEFT,"        14D170009               HRISHIKESH    KULKARNI ");
            al_draw_text(fon2,eblue,100,350,ALLEGRO_ALIGN_LEFT,"        140110032               KARTHIK    SANKARAN ");
            al_draw_text(fon2,eblue,100,380,ALLEGRO_ALIGN_LEFT,"        14D170012               ONKAR     GADADE ");
            al_draw_text(fon2,eblue,100,410,ALLEGRO_ALIGN_LEFT,"        14D110009               MAHESH    AMBEKAR ");

            al_draw_text(fon2,eblue,100,220,ALLEGRO_ALIGN_LEFT,"HIT SPACE TO USE BULLETS");
            al_draw_text(fon ,al_map_rgb(0, 255, 0),400,500,ALLEGRO_ALIGN_CENTER,"BACK");
            al_flip_display();
            while(!a)
            {
                ALLEGRO_EVENT event;

                al_wait_for_event(event_queue,&event);

                if(event.type==ALLEGRO_EVENT_DISPLAY_CLOSE)
                    a=true;
                else if(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
                {
                    x=event.mouse.x;
                    y=event.mouse.y;
                    if(x>250&&x<530)
                        if(y>500&&y<600)
                            goto begin2;
                }
            }
        }
        //***** HIGH SCORE *****
        else if(y>350&&y<390)
        {
            al_clear_to_color(al_map_rgb(0,0,0));
            al_flip_display();

            showhigh();
            //al_draw_text(fon2 ,al_map_rgb(255,0,0),400,260,ALLEGRO_ALIGN_CENTER,"COMING SOON ....");
            al_draw_text(fon ,al_map_rgb(0,0,255),400,500,ALLEGRO_ALIGN_CENTER,"BACK");
            al_flip_display();
            while(!a)
            {
                ALLEGRO_EVENT event;

                al_wait_for_event(event_queue,&event);

                if(event.type==ALLEGRO_EVENT_DISPLAY_CLOSE)
                    a=true;
                else if(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
                {
                    x=event.mouse.x;
                    y=event.mouse.y;
                    if(x>250&&x<530)
                        if(y>500&&y<600)
                            goto begin2;
                }
            }
        }
        else
            goto begin2;
        }
        else
        {
            goto begin2;
        }
    }
    //DESTROYING MENU LOADED ENTITIES
    al_destroy_font(font);
    al_destroy_font(fon);
    al_destroy_display(dis);

return 0;
}

void InitShip(SpaceShip &ship)
{
    if(level==1)
    {
        ship.x = 35;
        ship.y = 520;
    }
    else if(level==2)
    {
        ship.x = 55;
        ship.y = 100;
    }
	ship.ID = PLAYER;
	ship.lives = 1;         //lives given  to player
	ship.speed = 3;
	ship.jump = 1;          //allows to jump
	ship.time = sFPS*3;
	ship.boundx = 5;        //bound stands for boundary distance from centre
	ship.boundy = 20;
	ship.score = 0;
}
void MoveShipUp(SpaceShip &ship, rectangle brick[])
{
    for(int i=0;i<bricksize;i++)
    {
        //Condition to move up
         if((ship.jump==1)&&(ship.time == sFPS*3&&(brick[i].y-h/2==ship.y+ship.boundy&&
            (( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x-w/2 )
            ||( ship.x+ship.boundx >= brick[i].x+w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )
            ||( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )))))
         {
             ship.time -=2;
             ship.y-=ship.speed*2/3;
             ship.jump=0;
         }
    }
    if(ship.y < 0)
		ship.y = 0;
}
void MoveShipDown(SpaceShip &ship, rectangle brick[92])
{
    for(int i=0;i<bricksize&&(ship.jump==0);i++)
    {
            //Condition to move ship down
            if((ship.time>=0)&&(brick[i].y+h/2==ship.y-ship.boundy)&&
            (( ship.x+ship.boundx > brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x-w/2 )
            ||( ship.x+ship.boundx >= brick[i].x+w/2 && ship.x-ship.boundx < brick[i].x+w/2 )
            ||( ship.x+ship.boundx > brick[i].x-w/2 && ship.x-ship.boundx < brick[i].x+w/2 )))
            {
                ship.time=-1*ship.time;
                ship.time--;
                break;
            }
            else if((brick[i].y-h/2==ship.y+ship.boundy)&&(ship.time<=0)&&
            (( ship.x+ship.boundx > brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x-w/2 )
            ||( ship.x+ship.boundx >= brick[i].x+w/2 && ship.x-ship.boundx < brick[i].x+w/2 )
            ||( ship.x+ship.boundx > brick[i].x-w/2 && ship.x-ship.boundx < brick[i].x+w/2 )))
            {
                if(level==2&&firstscreen&&(i>=73&&i<=82))
                    ship.lives--;
                else if(level==2&&!firstscreen&&(i>=74&&i<=87))
                    ship.lives--;
                ship.time=sFPS*3;
                ship.jump=1;

                break;
            }

    }
    if((ship.jump==0)&&(ship.time<sFPS*3&&ship.time>=0))
    {
        ship.y-=ship.speed*2/3;
        ship.time--;
    }

    else if((ship.jump==0)&&(ship.time<0))
    {
        ship.y+=ship.speed*2/3;
        ship.time--;
    }
}
void MoveShipLeft(SpaceShip &ship,rectangle brick[92])
{
    int flag=0;
	ship.x -= ship.speed;
	if(ship.jump)
    {
        for(int i=0;i<bricksize;i++)
        {
            //Precondition to move ship left
            if(ship.time==sFPS*3&&brick[i].y-h/2==ship.y+ship.boundy&&
            (( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x-w/2 )
            ||( ship.x+ship.boundx >= brick[i].x+w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )
            ||( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )))
            {
                flag++;
                break;
            }
        }

        if(!flag)
        {
            ship.time = -1*sFPS;
            ship.jump=0;
        }
    }
	for(int i = 0; i<bricksize ; i++)
    {
        if((ship.x-ship.boundx<=brick[i].x+w/2&&ship.x+ship.boundx>=brick[i].x+w/2)&&
        ((ship.y + ship.boundy > brick[i].y + h/2 && ship.y - ship.boundy < brick[i].y + h/2)
        ||(ship.y + ship.boundy > brick[i].y- h/2 && ship.y - ship.boundy < brick[i].y - h/2)
        ||(ship.y + ship.boundy <= brick[i].y + h/2 && ship.y - ship.boundy >= brick[i].y - h/2)))
        {
            if(level==2&&firstscreen&&(i>=73&&i<=82))
                ship.lives--;
            else if(level==2&&!firstscreen&&(i>=74&&i<=87))
                ship.lives--;
            else
                ship.x = brick[i].x+w/2+ship.boundx;
        }
    }
}
void MoveShipRight(SpaceShip &ship,rectangle brick[92])
{
	int flag=0;
	ship.x += ship.speed;
	if(ship.jump)
	{
        for(int i=0;i<bricksize;i++)
        {
            //Precondition to move ship right
            if(ship.time==sFPS*3&&brick[i].y-h/2==ship.y+ship.boundy&&
            (( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x-w/2 )
            ||( ship.x+ship.boundx >= brick[i].x+w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )
            ||( ship.x+ship.boundx >= brick[i].x-w/2 && ship.x-ship.boundx <= brick[i].x+w/2 )))
            {
                flag++;
                break;
            }
        }
        if(!flag)
        {
            ship.time = -1*sFPS;
            ship.jump=0;
        }
	}
	for(int i=0;i<bricksize;i++)
    {
        if((ship.x+ship.boundx>=brick[i].x-w/2&&ship.x-ship.boundx<=brick[i].x-w/2)&&
        ((ship.y + ship.boundy > brick[i].y + h/2 && ship.y - ship.boundy < brick[i].y + h/2)
        ||(ship.y + ship.boundy > brick[i].y- h/2 && ship.y - ship.boundy < brick[i].y - h/2)
        ||(ship.y + ship.boundy <= brick[i].y + h/2 && ship.y - ship.boundy >= brick[i].y - h/2)))
		{
		    if(level==2&&firstscreen&&(i>=73&&i<=82))
                    ship.lives--;
            else if(level==2&&!firstscreen&&(i>=74&&i<=87))
                    ship.lives--;
            else
                ship.x = brick[i].x-w/2-ship.boundx;
		}
    }
}
void InitBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;     //Bullet is available to fire
	}
}
void DrawBullet(Bullet bullet[], int size)
{
	for( int i = 0; i < size; i++)
	{
		if(bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));//Showing bullet on screen
	}
}
void FireBullet(Bullet bullet[], int size, SpaceShip &ship)
{
	for( int i = 0; i < size; i++)
	{
		if(!bullet[i].live)
		{
			bullet[i].x = ship.x + 17;
			bullet[i].y = ship.y;
			bullet[i].live = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].x += bullet[i].speed;
			if(bullet[i].x > WIDTH)
				bullet[i].live = false;
		}
	}
}
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bullet[i].live)
		{
			for(int j =0; j < cSize; j++)
			{
				if(comets[j].live)
				{
				    //COLLISION DETECTION BETWEEN BULLET AND COMET
					if(bullet[i].x > (comets[j].x - comets[j].boundx) &&
						bullet[i].x < (comets[j].x + comets[j].boundx) &&
						bullet[i].y > (comets[j].y - comets[j].boundy) &&
						bullet[i].y < (comets[j].y + comets[j].boundy))
					{
						bullet[i].live = false;
						comets[j].live = false;
					}
				}
			}
		}
	}
}

void InitComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		comets[i].ID = ENEMY;
		comets[i].live = false;
		comets[i].speed = 5;
		comets[i].boundx = 35;
		comets[i].boundy = 30;
	}
}
void DrawComet(Comet comets[], int size)
{
    if (firstscreen)
    {
        for(int i = 0; i < 3; i++)
        {
            if(comets[i].live)
            {
                al_draw_scaled_bitmap(fireball,0,0,fireballwidth,fireballheight,comets[i].x-15, comets[i].y-30,40,60,0);
            }
        }
    }
    if (!firstscreen)
    {
        for(int i = 0; i <4; i++)
        {
            if(comets[i].live)
            {
                 al_draw_scaled_bitmap(fireball,0,0,fireballwidth,fireballheight,comets[i].x-15, comets[i].y-30,40,60,0);
            }
        }
    }

}
void StartComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(!comets[i].live)
		{
			if( rand() % 500 == 0)
			{
				comets[i].live = true;
				comets[i].x = WIDTH;
				comets[i].y = 100 + rand() % (20);
				break;
			}
		}
	}
}
void UpdateComet(Comet comets[], int size,rectangle brick[])
{
    int i=4;
    if(level==1)i=0;
        for( i ; i < size; i++)
        {
            if(comets[i].live)
            {
                comets[i].x -= comets[i].speed;
            }
        }
	if(level==2)
    {
        int t;
        if(comets[0].live )
        {
            if (firstscreen)
                t=0;
            else
                t=12*30;
            //FOR circulating MONSTER
            if(comets[0].x>10*30+15-t&&comets[0].y==4*60+30)
                comets[0].x -= comets[0].speed/2;
            if(comets[0].x<10*30+15-t)
                comets[0].x=10*30+15-t;
            if(comets[0].x==10*30+15-t&&comets[0].y<6*60+30)
                comets[0].y += comets[0].speed/2;
            if(comets[0].y>6*60+30)
                comets[0].y = 6*60+30;
            if(comets[0].x<15*30+15-t&&comets[0].y==6*60+30)
                comets[0].x += comets[0].speed/2;
            if(comets[0].x>15*30+15-t)
                comets[0].x=15*30+15-t;
            if(comets[0].x==15*30+15-t&&comets[0].y>4*60+30)
                comets[0].y -= comets[0].speed/2;
            if(comets[0].y<4*60+30)
                comets[0].y = 4*60+30;

        }
        //FOR other monsters(moving up and down)
        if(comets[1].live)
        {
            if (firstscreen)
                comets[1].x=18*30+15;
            if (!firstscreen)
                comets[1].x=18*30+15-12*30;
            if(comets[1].y<90)
            {
                comets[1].y=90;
                comets[1].speed*=-1;
            }
            if(comets[1].y>4*60+30)
            {
                comets[1].y=270;
                comets[1].speed*=-1;
            }
            if (comets[1].y<=4*60+30&&comets[1].y>=90)comets[1].y+=comets[1].speed;
        }
         if(comets[2].live)
        {
            if (firstscreen)
                comets[2].x=18*30+5;
            if (!firstscreen)
                comets[2].x=18*30+5-12*30;
            if(comets[2].y<330)
            {
                comets[2].y=330;
                comets[2].speed*=-1;
            }
            if(comets[2].y>8*60+30)
            {
                comets[2].y=510;
                comets[2].speed*=-1;
            }
            if (comets[2].y<=8*60+30&&comets[2].y>=330)
                comets[2].y+=comets[2].speed;
        }
        if(comets[3].live)
        {
            comets[3].x=18*30+15;
            if(comets[3].y<90)
            {
                comets[3].y=90;
                comets[3].speed*=-1;
            }
            if(comets[3].y>4*60+30)
            {
                comets[3].y=270;
                comets[3].speed*=-1;
            }
            if (comets[3].y<=4*60+30&&comets[3].y>=90)comets[3].y+=comets[3].speed;
        }
    }

}
void CollideComet(Comet comets[], int cSize, SpaceShip &ship, rectangle brick[])
{
    for(int i = 0; i < cSize; i++)
	{
		if(comets[i].live)
		{
		    //DETECTION OF COLLISION WITH COMET
			if(comets[i].x - comets[i].boundx < ship.x + ship.boundx &&
				comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
				comets[i].y - comets[i].boundy < ship.y + ship.boundy &&
				comets[i].y + comets[i].boundy > ship.y - ship.boundy)
			{
				ship.lives--;
				comets[i].live = false;
			}
    else if(comets[i].x < -390)
			{
				comets[i].live = false;
			}
		}
	}
}
void makeBrick(int x , int y) // Printing brick on the screen
{
    al_draw_scaled_bitmap(image, 0, 0, imageWidth, imageHeight, x, y,30 , 60, 0);
}
