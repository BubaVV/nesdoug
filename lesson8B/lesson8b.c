/*	for cc65, for NES
 *	start = change background
 *	now collide with background
 *	doug fraker 2015
 *	feel free to reuse any code here
 */



#include "DEFINE.c"


// these are the 4 backgrounds
// generated with NES Screen Tool, with RLE option on
#include "BG/1.h" // called n1
#include "BG/2.h" // called n2
#include "BG/3.h" // called n3
#include "BG/4.h" // called n4

const unsigned char * const All_Backgrounds[]={n1,n2,n3,n4};
// pointers to the address of each background

// collision data, made by exporting csv from Tiled, and slight modification
#include "BG/1.csv" // called c1
#include "BG/2.csv" // called c2
#include "BG/3.csv" // called c3
#include "BG/4.csv" // called c4

const unsigned char * const All_Collision_Maps[] = {c1,c2,c3,c4};





void main (void) {
	All_Off(); // turn off screen
	X1 = 0x7f; // starting position
	Y1 = 0x77; // middle of screen
	Load_Palette();
	Reset_Scroll();
	All_On(); // turn on screen
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI
		
		if (new_background > 0) {
			Draw_Background();
			new_background = 0;
		}
		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();
		move_logic();
		check_Start();
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
}


void check_Start (void){
	if ( ((joypad1 & START) != 0) && ((joypad1old & START) == 0) ){
		++new_background;
	}
}


void four_Sides (void){
	if (X1 < (255-3)){	// find the left side, the first 3 pixels are blank
		X1_Left_Side = X1 + 4;
	}
	else {
		X1_Left_Side = 255;
	}
	if (X1 < (255-12)){	// find the right side, the last 3 pixels are blank
		X1_Right_Side = X1 + 11;
	}
	else {
		X1_Right_Side = 255;
	}
	Y1_Top = Y1+9;	// our top is the same as the master Y
	
	if (Y1 < (255-15)){ // find the bottom side
		Y1_Bottom = Y1 + 15;
	}
	else {
		Y1_Bottom = 255;
	}
}

	
void collide_Check_LR (void){
	four_Sides();	// set the L R bottom top variables
	
	if ((joypad1 & RIGHT) != 0){ 	// first check right
		corner = ((X1_Right_Side & 0xf0) >> 4) + (Y1_Top & 0xf0); // top right
		if (C_MAP[corner] != 0)
			X1 = (X1 & 0xf0) + 4; // if collision, realign

		corner = ((X1_Right_Side & 0xf0) >> 4) + (Y1_Bottom & 0xf0); // bottom right
		if (C_MAP[corner] != 0)
			X1 = (X1 & 0xf0) + 4; // if collision, realign
	}
	else if ((joypad1 & LEFT) != 0){ // check left
		corner = ((X1_Left_Side & 0xf0) >> 4) + (Y1_Top & 0xf0); // top left
		if (C_MAP[corner] != 0)
			X1 = (X1 & 0xf0) + 12; // if collision, realign

		corner = ((X1_Left_Side & 0xf0) >> 4) + (Y1_Bottom & 0xf0); // bottom left
		if (C_MAP[corner] != 0)
			X1 = (X1 & 0xf0) + 12; // if collision, realign
	}
}


void collide_Check_UD (void){
	four_Sides();
	if ((joypad1 & DOWN) != 0){ // down first
		corner = ((X1_Right_Side & 0xf0) >> 4) + (Y1_Bottom & 0xf0); // bottom right
		if (C_MAP[corner] != 0)
			Y1 = (Y1 & 0xf0) ; // if collision, realign

		corner = ((X1_Left_Side & 0xf0) >> 4) + (Y1_Bottom & 0xf0); // bottom left
		if (C_MAP[corner] != 0)
			Y1 = (Y1 & 0xf0); // if collision, realign
	}
	else if ((joypad1 & UP) != 0) { //or up
		corner = ((X1_Right_Side & 0xf0) >> 4) + (Y1_Top & 0xf0); // top right
		if (C_MAP[corner] != 0)
			Y1 = (Y1 & 0xf0) + 7; // if collision, realign

		corner = ((X1_Left_Side & 0xf0) >> 4) + (Y1_Top & 0xf0); // top left
		if (C_MAP[corner] != 0)
			Y1 = (Y1 & 0xf0) + 7; // if collision, realign
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
	
	collide_Check_LR();
	
	if ((joypad1 & DOWN) != 0){
		state = Going_Down;
		++Y1;
	}
	if ((joypad1 & UP) != 0){
		state = Going_Up;
		--Y1;
	}
	
	collide_Check_UD();
}


void Draw_Background(void) {
	All_Off();
	PPU_ADDRESS = 0x20;
	PPU_ADDRESS = 0x00;
	UnRLE(All_Backgrounds[which_BGD]);	// uncompresses our data

	// now load the collision map to 300-3ff
	p_C_MAP = All_Collision_Maps[which_BGD];
	memcpy (C_MAP, p_C_MAP, 240);
	// 4805 cycles = 9 times faster
	
	Wait_Vblank();	// don't turn on screen until in v-blank
	All_On();
	Reset_Scroll();
	++which_BGD;
	if (which_BGD == 4) // shuffles between 0-3
		which_BGD = 0;
}
