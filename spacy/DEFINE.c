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

enum {TITLE_MODE, RUN_GAME_MODE, PAUSE_MODE, GAME_OVER_MODE, VICTORY_MODE, BOSS_MODE};

enum {SOUND_SHOOT, SOUND_LASER, SOUND_BOOM, SOUND_BIG_BOOM};

enum {SONG_TITLE, SONG_GAME, SONG_PAUSE, SONG_GAME_OVER, SONG_VICTORY};


//Globals
//our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
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
unsigned char Lives;

unsigned char Sprite_Index;


#pragma bss-name(push, "OAM")
unsigned char SPRITES[256];
//OAM equals ram addresses 200-2ff

#pragma bss-name(push, "BSS")

struct ENEMY {
	unsigned char anime; // for drawing object to screen
	unsigned char dir;	// direction, for flipping 0 = L
	unsigned char Y;	// top side
	unsigned char X;	// left side
	unsigned char delay; // wait to start moves
	unsigned char type;	// type of object
	unsigned char move;	// which move to do
	unsigned char count; // how far along on the move are we
};

struct ENEMY dummy; // for non-enemies, sprites
struct ENEMY hero_ship;

// all sprite types, max 128
enum {ship_reg, ship_left, ship_right, Pause_Sprites, Game_O_Sprites, Vic_Sprites};


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


void Reset(void);
void Blank_sprite(void);
void __fastcall__ Play_Fx(unsigned char effect);
void Reset_Music(void);
void __fastcall__ Play_Music(unsigned char song);
void Music_Update(void);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);
void __fastcall__ Draw_Sprites(int address);