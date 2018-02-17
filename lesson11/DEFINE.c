// let's define some things



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


// Globals
// our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index2;
unsigned char index3;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char state;
unsigned char state4;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test; 
unsigned char joypad2; 
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char Room;
unsigned char RoomB;
unsigned char RoomPlus;
const unsigned char *Room_Address; // is an int pointer, points to chars
unsigned char Horiz_scroll; 
unsigned char Horiz_scroll_Old; 
unsigned char Horiz_scroll_Plus; 
unsigned int Scroll_Adjusted_X;
// got rid of Vscroll, now always 0
unsigned char Nametable;
unsigned char NametableB;
unsigned char Nametable_Plus;
unsigned char walk_count;
signed char X_speed; // signed char = -128 to 127
signed char Y_speed; // signed char = -128 to 127
unsigned char direction; // 0 = R, 1 = L
unsigned char collision_Index;
unsigned char high_byte;
unsigned char collision;
unsigned char temp;
unsigned char temp2;
unsigned char temp3;
unsigned char temp4;
unsigned char PPU_ADDRESS_High;
unsigned char PPU_ADDRESS_Low;
unsigned char A;
unsigned char PPU_flag;
unsigned char PPU_flag2;

unsigned char dummy;


#pragma bss-name(push, "OAM")
unsigned char SPRITE_ZERO[4];
unsigned char SPRITES[252];
// OAM equals ram addresses 200-2ff

#pragma bss-name(push, "MAP")
unsigned char C_MAP[256];
unsigned char C_MAP2[256];
// MAP equals ram addresses 300-4ff, collision map, metatiles

#pragma bss-name(push, "BSS")
unsigned char BUFFER1[32];	// left column 1
unsigned char BUFFER2[32];	// right column 1
unsigned char BUFFER3[32];	// left column 2
unsigned char BUFFER4[32];	// right column 2

unsigned char BUFFER5[8];	// 1/2 bits of attribute table
unsigned char BUFFER6[8];	// 1/2 bits of attribute table
unsigned char BUFFER7[8];	// final attribute table buffer

#include "BG/A1.csv"
#include "BG/A2.csv"
#include "BG/A3.csv"
#include "BG/A4.csv"

const unsigned char * const ROOMS[]={A1, A2, A3, A4};

// collision maps called A1-A4
// now their value is 0-11, which will index to this array...

const unsigned char PLATFORM[]={ // which metatiles act like platforms
	0, 1, 1, 1, 1, 1, 
	0, 0, 0, 0, 0, 0};

// tl, tr, bl, br = tiles for each metatile
const unsigned char METATILES[]={
	0, 0, 0, 0, // 0 sky
	2, 2, 2, 2, // 1 ground
	0x14, 0x14, 0x14, 0x14, // 2 brick
	5, 6, 0x15, 0x16, // 3 hill tl
	6, 6, 0x16, 0x16, // 4 hill tm
	6, 7, 0x16, 0x17, // 5 hill tr
	0x15, 0x16, 0x15, 0x16, // 6 hill l
	0x16, 0x16, 0x16, 0x16, // 7 hill m
	0x16, 0x17, 0x16, 0x17, // 8 hill r
	0x15, 0x16, 0x18, 0x19, // 9 hill bl
	0x16, 0x16, 0x19, 0x19, // a hill bm
	0x16, 0x17, 0x19, 0x1a  // b hill br
};

// color palette for each metatile
const unsigned char MT_color[]={
	0, 3, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1};
	
const unsigned char HUD[]={"TEST SPRITE ZERO HERE  "}; // add tile 3 after this
// note, use palette 2 = 0xaa for first 8

const unsigned char PALETTE[]={
0x22, 0x16, 0x36, 0x0f,  0, 8, 0x18, 0x39,  0, 0, 0x10, 0x20,  0, 0x0a, 0x1a, 0x2a,
0x22, 0x37, 0x16, 0x0f,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 5, 0x15};

const unsigned char Walk_Moves[] = {
	0, 1, 0, 2}; // just a way to reuse the #0 state tiles
	
const unsigned char MetaSprite_Y[] = {0, 0, 8, 8}; // relative y coordinates

const unsigned char MetaSprite_Tile_Right[] = { // tile numbers, right
	0, 1, 0x10, 0x11, 	// walk 0, 2
	2, 3, 0x12, 0x13, 	// walk 1
	4, 5, 0x14, 0x15, 	// walk 3
	6, 7, 0x16, 0x17};	// jump

const unsigned char MetaSprite_Attrib_Right[] = {0, 0, 0, 0}; // attributes = not flipped

const unsigned char MetaSprite_X[] = {0, 8, 0, 8}; // relative x coordinates
// we are using 4 sprites, each one has a relative position from the top left sprite

const unsigned char MetaSprite_Tile_Left[] = { // tile numbers, left
	1, 0, 0x11, 0x10, 	// walk 0, 2
	3, 2, 0x13, 0x12, 	// walk 1
	5, 4, 0x15, 0x14, 	// walk 3
	7, 6, 0x17, 0x16}; 	// jump

const unsigned char MetaSprite_Attrib_Left[] = {0x40, 0x40, 0x40, 0x40}; // attributes = H flipped


// Prototypes
void All_Off (void);
void All_On (void);
void Reset_Scroll (void);
void Load_Palette (void);
void update_Sprites (void);
void Collision_Down (void);
void move_logic (void);
void Do_Buffer (void);
void Do_Buffer2 (void);
void Do_Buffer3 (void);
void Draw_Background (void);
void Set_Sprite_Zero (void);
void Load_HUD (void);
void Should_We_Buffer (void);
void New_Room (void);

void __fastcall__ memcpy (void* dest, const void* src, int count);
void Wait_Vblank(void);
void Get_Input(void);
void Sprite_Zero(void);
void Super_Fast_Write_PPU(void);
void Super_Fast_Write_PPU2(void);
