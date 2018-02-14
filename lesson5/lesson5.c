/*	for cc65, for NES
 *	constructs a simple metasprite, gets input, and moves him around
 *	doug fraker 2015
 *	feel free to reuse any code here
 */



#include "DEFINE.c"
//defines variables, constants, and function prototypes

void Get_Input (void); 
/*	this calls an asm function, written in asm4c.s
 *	it will read both joypads and store their reads in joypad1 and joypad2
 *	The buttons come in the order of A, B, Select, Start, Up, Down, Left, Right
 */





void main (void) {
	All_Off();		// turn off screen
	X1 = 0x7f;		// set the starting position of the top left sprite
	Y1 = 0x77; 		// middle of screen
	Load_Palette();
	Reset_Scroll();
	All_On(); 		// turn on screen
	while (1){ 		// infinite loop
		while (NMI_flag == 0); // wait till NMI
		
		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();
		move_logic();
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
	state4 = state << 2; // same as state * 4
	index4 = 0;
	for (index = 0; index < 4; ++index ){
		SPRITES[index4] = MetaSprite_Y[index] + Y1; // relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + state4]; // tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index]; // attributes, all zero here
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + X1; // relative x + master x
		++index4;
	}
}


void move_logic (void) {
	if ((joypad1 & RIGHT) != 0){
		state = Going_Right;
		++X1;
	}
	if ((joypad1 & LEFT) != 0){
		state = Going_Left;
		--X1;
	}
	if ((joypad1 & DOWN) != 0){
		state = Going_Down;
		++Y1;
	}
	if ((joypad1 & UP) != 0){
		state = Going_Up;
		--Y1;
	}
}
