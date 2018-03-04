// let's define some stuff

#define DEBUG		*((unsigned char*)0xff)

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80
#define A_B_BUTTON	0xc0

enum {TITLE_MODE, RUN_GAME_MODE, PAUSE_MODE, GAME_OVER_MODE, VICTORY_MODE, BOSS_MODE};

enum {SOUND_SHOOT, SOUND_LASER, SOUND_BOOM, SOUND_BIG_BOOM};

enum {SONG_TITLE, SONG_GAME, SONG_PAUSE, SONG_GAME_OVER, SONG_VICTORY};


//Globals
//our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char Frame_Count2;
unsigned char index;
unsigned char index4;



unsigned char X_old;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test; 
unsigned char joypad2; 
unsigned char joypad2old;
unsigned char joypad2test;
//Hscroll always zero
unsigned char Vert_scroll;
unsigned char Vert_scroll2;
signed char X_speed; //signed char = -128 to 127
signed char Y_speed; //signed char = -128 to 127
unsigned char collision;
unsigned char temp;
unsigned char temp2;
unsigned char temp3;
unsigned char song;	//which song
unsigned char Game_Mode; //00 = title, 01 = Game, 02 = Pause, 03 = Game Over, 04 = Victory
unsigned char Wait;
unsigned char Wait2;
unsigned char Score1;
unsigned char Score2;
unsigned char Score3;
unsigned char Score4;
unsigned char Lives;

unsigned char Sprite_Index;
unsigned char bullet_Wait; // how many frames before we can shoot again

unsigned char objectA_x;
unsigned char objectA_y;
unsigned char objectA_width;
unsigned char objectA_height;
unsigned char objectB_x;
unsigned char objectB_y;
unsigned char objectB_width;
unsigned char objectB_height;
unsigned char injury_Timer;
unsigned char invincible;
unsigned char Death_Flag;


#pragma bss-name(push, "OAM")
unsigned char SPRITES[256];
//OAM equals ram addresses 200-2ff

#pragma bss-name(push, "BSS")

struct ENEMY {
	unsigned char anime; // for drawing object to screen
	unsigned char dir;	// direction, for flipping 0 = L
	unsigned char Y;	// top side, if = 0 = off
	unsigned char X;	// left side
	unsigned char delay; // wait to start moves
	unsigned char type;	// type of object
	unsigned char move;	// which move to do
	unsigned char count; // how far along on the move are we
};

struct ENEMY dummy; // for texts that are sprites
struct ENEMY hero_ship;
struct ENEMY ENEMY1;
struct ENEMY ENEMY2;
struct ENEMY ENEMY3;
struct ENEMY ENEMY4;
struct ENEMY ENEMY5;
struct ENEMY ENEMY6;
struct ENEMY ENEMY7;
struct ENEMY ENEMY8;

struct ENEMY Explode1; // assuming I won't need 8 at once
struct ENEMY Explode2; // for enemy explosions
struct ENEMY Explode3;
struct ENEMY Explode4;

struct BULLET {
	unsigned char Y; // y = 0 = off
	unsigned char Y_sub;
	unsigned char tile;
	unsigned char attrib;
	unsigned char X;
	unsigned char X_sub;
	unsigned char Y_speed; // high nibble = speed, low = sub
	unsigned char X_speed;
};

struct BULLET hero_bullet1; // drawn on the screen as 2 sprites
struct BULLET hero_bullet2;
struct BULLET hero_bullet3;
struct BULLET hero_bullet4;
struct BULLET hero_bullet5;
struct BULLET hero_bullet6;
struct BULLET hero_bullet7;
struct BULLET hero_bullet8;

struct BULLET enemy_bullet1; // drawn on the screen as 1 sprite
struct BULLET enemy_bullet2;
struct BULLET enemy_bullet3;
struct BULLET enemy_bullet4;
struct BULLET enemy_bullet5;
struct BULLET enemy_bullet6;
struct BULLET enemy_bullet7;
struct BULLET enemy_bullet8;


// all sprite types, max 128
enum {ship_reg, ship_left, ship_right, ship_expl_1, ship_expl_2, ship_expl_3,
enemy_1, enemy_2, enemy_3, enemy_4, enemy_expl, Pause_Sprites, Game_O_Sprites, Vic_Sprites};


#include "BG/Title.h"
#include "BG/Stars.h"


//Title Screen, and starting sprite colors
const unsigned char PALETTE[]={
0x0f, 2, 0x13, 0x24,  0, 0, 0, 0x03,  0, 0, 0, 0x24,  0, 0, 0x10, 0x20,
0x0f, 0x01, 0x28, 0x22,  0, 8, 0x18, 0x28,  0, 4, 0x14, 0x24,  0, 0x0a, 0x1a, 0x2a};

	

const unsigned char HUD[] = {"SCORE:"};

const unsigned char HUD2[] = {"LIVES:"};



// prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Rotate_Palette(void);
void Load_Palette(void);
void Draw_Hero (void);
void move_logic(void);
void Draw_Title(void);
void Draw_Background(void);
void Turn_On_Pause_Sprites(void);
void Turn_On_Game_Over_Sprites(void);
void Turn_On_Victory_Sprites(void);
void Set_Sprite_Zero(void);

void shooting(void);
void spawn_bullets(void);
void Spawn_Enemies(void);
void Draw_Bullets(void);
void Draw_Enemies(void);
void collision_Test(void);
void collision_Test2(void);
void Adjust_Scoreboard (void);
void Draw_Explosion (void);
void Spawn_Bullets(void);

void Reset(void);
void Blank_sprite(void);
void __fastcall__ Play_Fx(unsigned char effect);
void Reset_Music(void);
void __fastcall__ Play_Music(unsigned char song);
void Music_Update(void);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);
void __fastcall__ Draw_Sprites(struct ENEMY * sprite_address);
void __fastcall__ move_bullets(struct BULLET * bullet_address);
void __fastcall__ move_bullets2(struct BULLET * bullet_address);
void __fastcall__ move_enemies(struct ENEMY * sprite_address);
void Hitbox(void);