// let's define some stuff

#define DEBUG		*((unsigned char*)0xff)


#define NO_FLIP 0
#define H_FLIP 0x40
#define V_FLIP 0x80
#define BOTH_FLIP 0xc0

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)
#define APU_CTRL		*((unsigned char*)0x4015)




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

enum {SOUND_SHOOT, SOUND_LASER, SOUND_BOOM, SOUND_BIG_BOOM, SILENCE};

enum {SONG_TITLE, SONG_GAME, SONG_PAUSE, SONG_GAME_OVER, SONG_VICTORY};


//Globals
//our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char Frame_Count2;
unsigned char index;
unsigned char index4;

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
unsigned char song;	// which song
unsigned char Game_Mode; // see above for enum
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

unsigned char Init_Wave_Flag;
unsigned char Wave;
unsigned char Pattern; // from wave
unsigned char Master_Delay; // wait between waves

const unsigned char *p_Wave_Data;
unsigned char Current_Move;
unsigned char Current_Type;
unsigned char Save_Game_Mode;
int Big_Math;
unsigned char BOSS_health;
unsigned char BOSS_health_test;
unsigned char BOSS_eyes; // counts down
unsigned char Shake_Timer_1;
unsigned char Shake_Timer_2;
unsigned char Selection;
unsigned char Flag_Blink_Boss;

unsigned char X_compare;
unsigned char ENEMY_X;
unsigned char ENEMY_Y;

struct ENEMY *Enemy_Pointer;




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
	unsigned char bullet_type;	// type of bullet
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

struct ENEMY BOSS;
struct ENEMY LASER1;
struct ENEMY LASER2;
struct ENEMY LASER3;
struct ENEMY LASER4;

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
SAUCER1, SAUCER2, FLY_D, FLY_DL, FLY_L, FLY_UL, FLY_U,
enemy_expl, Pause_Sprites, Game_O_Sprites, Vic_Sprites,
BOSS1, BOSS2, LASER};


#include "BG/Title.h"
#include "BG/Stars.h"
#include "BG/Keep.h"



// Title Screen, and starting sprite colors
const unsigned char PALETTE[]={
0x0f, 2, 0x13, 0x24,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0x10, 0x20,
0x0f, 0x01, 0x28, 0x22,  0, 0x16, 0x27, 0x38,  0, 2, 0x13, 0x33,  0, 0x17, 0x1a, 0x39};

	

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

void Draw_Bullets(void);
void Move_Enemies(void);
void Draw_Enemies2(void);
void collision_Test(void);
void collision_Test2(void);
void Adjust_Scoreboard (void);
void Draw_Explosion (void);

void Check_Wave_Over(void);
void Init_New_Wave(void);
void Clear_Enemies(void);
void Spawn_Bullet_E(void);
void Init_Boss_Level(void);
void collision_Test3(void);
void collision_Test4(void);
void Move_Boss(void);
void Check_Injury(void);
void Spawn_Lasers(void);
void Move_Lasers(void);
void SHAKE_AND_BLINK(void);
void Keep_Loop(void);
void Randomly_Shoot(void);


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
void draw_enemies(void);
void Hitbox(void);
void Circle_Math(void);
void Reset_Keep_Score(void);


void __fastcall__ memcpy (void* dest, const void* src, int count);

