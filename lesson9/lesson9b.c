/*	for cc65, for NES
 *	scrolling example, horizontal mirror = vertical scrolling
 *	doug fraker 2015
 *	feel free to reuse any code here
 */



#include "DEFINE.c"

#include "BG/N0.h"
#include "BG/N2.h"




void main (void) {
	All_Off(); // turn off screen
	Draw_Background();
	X1 = 0x7f;
	Y1 = 0x77; // middle of screen
	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();
	All_On(); // turn on screen
	while (1){ // infinite loop
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
	state4 = state << 2; // shift left 2 = multiply 4
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
	
	
	// those red numbers on the screen are sprites
	Horiz_High_Nibble = Horiz_scroll >> 4;
	Horiz_Low_Nibble = Horiz_scroll & 0x0f;
	Vert_High_Nibble = Vert_scroll >> 4;
	Vert_Low_Nibble = Vert_scroll & 0x0f;
	
	if (Horiz_High_Nibble < 10){ 	// if 0-9
		Horiz_High_Nibble += 0x30;	// use the number tiles
	}
	else {
		Horiz_High_Nibble += 0x37;	// if a-f, use letter tiles
	}
	if (Horiz_Low_Nibble < 10){
		Horiz_Low_Nibble += 0x30;
	}
	else {
		Horiz_Low_Nibble += 0x37;
	}
	if (Vert_High_Nibble < 10){
		Vert_High_Nibble += 0x30;
	}
	else {
		Vert_High_Nibble += 0x37;
	}
	if (Vert_Low_Nibble < 10){
		Vert_Low_Nibble += 0x30;
	}
	else {
		Vert_Low_Nibble += 0x37;
	}
	// display the H scroll
	// sprites = 4 bytes = y, tile, palette, x
	SPRITES[0x10] = 0x20;
	SPRITES[0x11] = (char)'H'; // 0x48
	SPRITES[0x12] = 3; // use palette #3
	SPRITES[0x13] = 0x20;
	
	SPRITES[0x14] = 0x20;
	SPRITES[0x15] = Horiz_High_Nibble;
	SPRITES[0x16] = 3;
	SPRITES[0x17] = 0x30;
	
	SPRITES[0x18] = 0x20;
	SPRITES[0x19] = Horiz_Low_Nibble;
	SPRITES[0x1a] = 3;
	SPRITES[0x1b] = 0x38;
	
	// display the V scroll
	SPRITES[0x20] = 0x20; 
	SPRITES[0x21] = (char)'V'; // 0x56
	SPRITES[0x22] = 3;
	SPRITES[0x23] = 0x50;
	
	SPRITES[0x24] = 0x20;
	SPRITES[0x25] = Vert_High_Nibble;
	SPRITES[0x26] = 3;
	SPRITES[0x27] = 0x60;
	
	SPRITES[0x28] = 0x20;
	SPRITES[0x29] = Vert_Low_Nibble;
	SPRITES[0x2a] = 3;
	SPRITES[0x2b] = 0x68;
}


void move_logic (void) {
	if ((joypad1 & RIGHT) != 0){
		state = Going_Right;
		++Horiz_scroll;
	}
	if ((joypad1 & LEFT) != 0){
		state = Going_Left;
		--Horiz_scroll;
	}
	if ((joypad1 & DOWN) != 0){
		state = Going_Down;
		++Vert_scroll;
		if (Vert_scroll == 0xf0){
			Vert_scroll = 0;
			++Nametable;
		}
	}
	if ((joypad1 & UP) != 0){
		state = Going_Up;
		--Vert_scroll;
		if (Vert_scroll == 0xff){
			Vert_scroll = 0xef;
			++Nametable;
		}
	}
	Nametable &= 1; // keep it 1 or 0
}


void Draw_Background (void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 2000
	PPU_ADDRESS = 0x00;
	UnRLE(N0);	// uncompresses our data ///that's a zero
	
	PPU_ADDRESS = 0x28; // address of nametable #2 = 2800
	PPU_ADDRESS = 0x00;
	UnRLE(N2);	// uncompresses our data
}
	