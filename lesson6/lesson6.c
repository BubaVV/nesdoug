/*	for cc65, for NES
 *	constructs a simple metasprite, gets input, and moves him around
 *	now there are 2 characters, A controlled by player, B auto-moved, and scoreboard
 *	which goes up on Sprite vs Sprite collision
 *	
 *	doug fraker 2015
 *	feel free to reuse any code here
 */



#include "DEFINE.c"

void Get_Input(void); 
/*	this calls an asm function, written in asm4c.s
 *	it will read both joypads and store their reads in joypad1 and joypad2
 *	The buttons come in the order of A, B, Select, Start, Up, Down, Left, Right
 */
 




void main (void) {
	All_Off(); 		// turn off screen
	A_X = 0x20; 	// starting x and y
	A_Y = 0x80;
	B_X = 0x8f;
	B_Y = 0x77; 
	PPU_Update ();	// start it off with all zeros
	Load_Palette();
	Reset_Scroll();
	All_On(); 		// turn on screen
	while (1){ 		// infinite loop
		while (NMI_flag == 0); // wait till NMI
		
		if (PPU_flag > 0) {
			PPU_Update(); // draw the scoreboard
			PPU_flag = 0;
			Reset_Scroll();
		}
		
		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();
		move_logic();
		update_Sprites();
		collision();
		
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

void every_frame (void) {
	OAM_ADDRESS = 0;
	OAM_DMA = 2;		// push all the sprite data from the ram at 200-2ff to the sprite memory
	PPU_CTRL = 0x90; 	// screen is on, NMI on
	PPU_MASK = 0x1e;
	SCROLL = 0;
	SCROLL = 0;			// just double checking that the scroll is set to 0
	Get_Input();
}



void update_Sprites (void) {
	
	// first the player sprites
	state4 = state << 2; // same as state * 4
	index4 = 0;
	for (index = 0; index < 4; ++index ){
		SPRITES[index4] = MetaSprite_Y[index] + A_Y; // relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + state4]; // tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index]; // attributes, all zero here
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + A_X; // relative x + master x
		++index4;
	}
	
	// now the auto-moved character sprites
	// index4 will keep going up
	B_state4 = B_state << 2; //shift left 2 = multiply 4
	for (index = 0; index < 4; ++index ){			// enemy sprites
		SPRITES[index4] = MetaSprite_Y[index] + B_Y; // relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + B_state4]; // tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index] + 1; // attributes, 1 = 2nd palette
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + B_X; // relative x + master x
		++index4;
	}
}

void move_logic (void) {
	if ((joypad1 & RIGHT) != 0){
		state = Going_Right;
		++A_X;
	}
	if ((joypad1 & LEFT) != 0){
		state = Going_Left;
		--A_X;
	}
	if ((joypad1 & DOWN) != 0){
		state = Going_Down;
		++A_Y;
	}
	if ((joypad1 & UP) != 0){
		state = Going_Up;
		--A_Y;
	}
	// now the enemy, who auto-moves
		if (B_state == Going_Right)
			++B_X;
		if (B_state == Going_Down)
			++B_Y;
		if (B_state == Going_Left)
			--B_X;
		if (B_state == Going_Up)
			--B_Y;
		++B_state_count;
		if (B_state_count == 20){ // do a move for 20 frames, then switch
			B_state_count = 0;
			++B_state;
		}
		if (B_state == 4) // keep it 0-3
			B_state = 0;
}


void collision (void) {
	// get object A sides
	A_left_side_X = A_X + 3;
	if (A_left_side_X < A_X) 	
		A_left_side_X = 255; // if overflow, set to max high
	
	A_right_side_X = A_X + 12;
	if (A_right_side_X < A_X) 	
		A_right_side_X = 255;
	
	A_top_Y = A_Y;
	
	A_bottom_Y = A_Y + 15;
	if (A_bottom_Y < A_Y) 		
		A_bottom_Y = 255;
	
	// get object B sides
	B_left_side_X = B_X + 3;
	if (B_left_side_X < B_X) 	
		B_left_side_X = 255;
	
	B_right_side_X = B_X + 12;
	if (B_right_side_X < B_X) 	
		B_right_side_X = 255;
	
	B_top_Y = B_Y;
	
	B_bottom_Y = B_Y + 15;
	if (B_bottom_Y < B_Y) 	
		B_bottom_Y = 255;
	

	// here's the actual collision check
	if (A_left_side_X <= B_right_side_X && A_right_side_X >= B_left_side_X && A_top_Y <= B_bottom_Y && A_bottom_Y >= B_top_Y){
		++score5;
		++PPU_flag; 
		// sets a flag, so at next v-blank it will draw new scoreboard to screen
	}

	// adjust scoreboard in RAM
	if (score5 > 9){
		++score4;
		score5 = 0;
	}
	if (score4 > 9){
		++score3;
		score4 = 0;
	}
	if (score3 > 9){
		++score2;
		score3 = 0;
	}
	if (score2 > 9){
		++score1;
		score2 = 0;
	}
	if (score1 > 9){
		score1 = 0;
		score2 = 0;
		score3 = 0;
		score4 = 0;
		score5 = 0;
	}
}

// draw the scoreboard on the screen
void PPU_Update (void) {
	PPU_ADDRESS = 0x20;
	PPU_ADDRESS = 0x8c;
	PPU_DATA = score1+1;//tile 0 = blank, tile 1 = "0", tile 2 = "1", etc
	PPU_DATA = score2+1;//so we have to add 1 to the digit to get the corresponding tile
	PPU_DATA = score3+1;
	PPU_DATA = score4+1;
	PPU_DATA = score5+1;
}