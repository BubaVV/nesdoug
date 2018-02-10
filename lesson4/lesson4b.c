/*	for cc65, for NES
 *	constructs a simple metasprite and moves him around
 *	doug fraker 2015
 *	feel free to reuse any code here
 */

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


//	Globals
//	our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char move;
unsigned char move_count;
unsigned char move4;

#pragma bss-name(push, "OAM")
unsigned char SPRITES[256];
//	OAM equals ram addresses 200-2ff




const unsigned char PALETTE[]={
0x19, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0x19, 0x37, 0x24, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};

const unsigned char MetaSprite_Y[] = {0, 0, 8, 8}; // relative y coordinates

const unsigned char MetaSprite_Tile[] = { //tile numbers
	2, 3, 0x12, 0x13, //right
	0, 1, 0x10, 0x11, //down
	6, 7, 0x16, 0x17, //left
	4, 5, 0x14, 0x15}; //up

const unsigned char MetaSprite_Attrib[] = {0, 0, 0, 0}; // attributes = flipping, palette

const unsigned char MetaSprite_X[] = {0, 8, 0, 8}; // relative x coordinates
// we are using 4 sprites, each one has a relative position from the top left sprite





// Prototypes
void All_Off (void);
void All_On (void);
void Reset_Scroll (void);
void Load_Palette (void);
void update_Sprites (void);




void main (void) {
	All_Off();	// turn off screen
	X1 = 0x7f;	// starting position of the top left sprite
	Y1 = 0x77;	// near the middle of screen
	Load_Palette();
	Reset_Scroll();
	All_On();	// turn on screen
	while (1){	// infinite loop
		while (NMI_flag == 0);	// wait till NMI
		
		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		
		if (move == 0)
			++X1;
		if (move == 1)
			++Y1;
		if (move == 2)
			--X1;
		if (move == 3)
			--Y1;

		++move_count;
		if (move_count == 20){ // do a move for 20 frames, then switch
			move_count = 0;
			++move;
		}
		if (move == 4)	// keep it 0-3
			move = 0;

		update_Sprites();
		
		NMI_flag = 0;
	}
}
	
// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
	
	
	
void All_Off (void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}

	
void All_On (void) {
	PPU_CTRL = 0x90; // screen is on, NMI on
	PPU_MASK = 0x1e; 
}

	
void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}

	
void Load_Palette (void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}


void update_Sprites (void) {
	move4 = move << 2; // same as move * 4
	index4 = 0;
	for (index = 0; index < 4; ++index ){
		SPRITES[index4] = MetaSprite_Y[index] + Y1; // relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + move4]; // tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index]; // attributes, all zero here
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + X1; // relative x + master x
		++index4;
	}
}


