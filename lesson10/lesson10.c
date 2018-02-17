/*	for cc65, for NES
 *	simple platformer game
 *	doug fraker 2015
 *	feel free to reuse any code here
 */





#include "DEFINE.c"





void main (void) {
	All_Off(); // turn off screen
	Draw_Background();
	X1 = 0x80;
	Y1 = 0x70; // middle of screen
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
	if (direction == 0){ // right
		for (index = 0; index < 4; ++index ){
			SPRITES[index4] = MetaSprite_Y[index] + Y1; // relative y + master y
			++index4;
			SPRITES[index4] = MetaSprite_Tile_R[index + state4]; // tile numbers
			++index4;
			SPRITES[index4] = MetaSprite_Attrib_R[index]; // attributes, all zero here
			++index4;
			SPRITES[index4] = MetaSprite_X[index] + X1; // relative x + master x
			++index4;
		}
	}
	else { // left
		for (index = 0; index < 4; ++index ){
			SPRITES[index4] = MetaSprite_Y[index] + Y1; // relative y + master y
			++index4;
			SPRITES[index4] = MetaSprite_Tile_L[index + state4]; // tile numbers
			++index4;
			SPRITES[index4] = MetaSprite_Attrib_L[index]; // attributes, all zero here
			++index4;
			SPRITES[index4] = MetaSprite_X[index] + X1; // relative x + master x
			++index4;
		}
	}
}


void Collision_Down (void){
	if (NametableB == 0){ // first collision map
		temp = C_MAP[collision_Index];
		collision += PLATFORM[temp];
	}
	else { // second collision map
		temp = C_MAP2[collision_Index];
		collision += PLATFORM[temp];
	}
}


void move_logic (void) {
	if ((joypad1 & (RIGHT|LEFT)) == 0){ // no L or R
		walk_count = 0;
		// apply friction, if no L or R
		if (X_speed >= 0){ // if positive, going right
			if (X_speed >= 4){
				X_speed -= 4;
			}
			else {
				X_speed = 0;
			}
		}
		else { // going left
			if (X_speed <= (-4)){
				X_speed +=4;
			}
			else {
				X_speed = 0;
			}
		}
	}
	
	if ((joypad1 & RIGHT) != 0){
		++walk_count;
		direction = 0;
		if (X_speed >= 0){ // going right
			X_speed += 2;
		}
		else { // going left
			X_speed += 8; // double friction
		}
	}
	if ((joypad1 & LEFT) != 0){
		++walk_count;
		direction = 1;
		if (X_speed <= 0){ // going left
			X_speed -= 2;
		}
		else { // going right
			X_speed -= 8; // double friction
		}
	}
	
	
	// collision check, platform
	
	// first check the bottom left corner of character
	// which nametable am I in?
	NametableB = Nametable;
	Scroll_Adjusted_X = (X1 + Horiz_scroll + 3); // left
	high_byte = Scroll_Adjusted_X >> 8;
	if (high_byte != 0){ // if H scroll + Sprite X > 255, then we should use
		++NametableB;   // the other nametable's collision map
		NametableB &= 1; // keep it 0 or 1
	}
	// we want to find which metatile in the collision map this point is in...is it solid?
	collision_Index = (((char)Scroll_Adjusted_X>>4) + ((Y1+16) & 0xf0));
	collision = 0;
	Collision_Down(); // if on platform, ++collision
	
	// now check the bottom right corner of character
	// which nametable am I in?
	NametableB = Nametable;
	Scroll_Adjusted_X = (X1 + Horiz_scroll + 12);
	high_byte = Scroll_Adjusted_X >> 8;
	if (high_byte != 0){ // if H scroll + Sprite X > 255, then we should use
		++NametableB;   // the other nametable's collision map
		NametableB &= 1; // keep it 0 or 1
	}
	collision_Index = (((char)Scroll_Adjusted_X>>4) + ((Y1+16) & 0xf0)); // bottom right
	Collision_Down(); // if on platform, ++collision
	
	if ((Y1 & 0x0f) > 1) // only platform collide if nearly aligned to a metatile
		collision = 0;
	
	if (collision == 0){
		Y_speed += 2; // gravity
	}
	else {
		Y_speed = 0;	// collision = stop falling
		Y1 &= 0xf0; // align to the metatile
	}
	
	// Jump - we already figured if we are on a platform, only jump if on a platform
	if (collision > 0){
		if (((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)){
			Y_speed = -0x38; // 0xc8
		}
	}
	
	// max speeds
	if (X_speed >= 0){ // going right
		if (X_speed > 0x20)
			X_speed = 0x20;
	}
	else {
		if (X_speed < (-0x20))
			X_speed = (-0x20); // 0xe0
	}
		
	if (Y_speed >= 0){
		if (Y_speed > 0x20)
			Y_speed = 0x20;
	}
	
	// move player
	Horiz_scroll_Old = Horiz_scroll;
	Horiz_scroll += (X_speed >> 4); // use the high nibble
	if (X_speed >= 0){ // going right
		if (Horiz_scroll_Old > Horiz_scroll) // if pass 0, switch nametables
			++Nametable;
	}
	else { // going left
		if (Horiz_scroll_Old < Horiz_scroll)
			++Nametable; // if pass 0, switch nametables
	}
	Nametable &= 1; // keep it 1 or 0
	
	Y1 += (Y_speed >> 4); // use the high nibble

	if (walk_count > 0x1f) // walk_count forced 0-1f
		walk_count = 0;

	state = Walk_Moves[(walk_count >> 3)]; // if not jumping
	
	if (Y_speed < 0) // negative = jumping
		state = 3;
}


void Draw_Background(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(N1);	// uncompresses our data
	
	PPU_ADDRESS = 0x24; // address of nametable #1 = 0x2400
	PPU_ADDRESS = 0x00;
	UnRLE(N2);	// uncompresses our data
	
	// load collision maps to RAM
	memcpy (C_MAP, C1, 240);
	memcpy (C_MAP2, C2, 240);
}
