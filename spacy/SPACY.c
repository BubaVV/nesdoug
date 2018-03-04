/*	for cc65, for NES
 *	complete space shooter game, pt 1
 *	doug fraker 2015
 *	feel free to reuse any code here
 */
 
#include "DEFINE.c"




void main (void) {
	All_Off();
	Draw_Title();
	
	joypad1 = 0xff; // fix a bug, reset is wiping joypad1old
	Load_Palette();
	Reset_Scroll();
	
	Reset_Music(); // note, this is famitone init, and I added the music data address. see famitone2.s
	// song = SONG_TITLE // it already is zero
	Play_Music(song);
	
	Wait_Vblank();
	All_On();
	while (1){ // infinite loop
		while (Game_Mode == TITLE_MODE){ // Title Screen
			while (NMI_flag == 0); // wait till v-blank
			
			Rotate_Palette();
			Reset_Scroll();
			
			Get_Input();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				NMI_flag = 0;
				while (NMI_flag == 0); // wait till v-blank
				// init game mode
				All_Off();
				Game_Mode = RUN_GAME_MODE;
				hero_ship.X = 0x78; // starting point
				hero_ship.Y = 0xd0; // bottom of screen
				Lives = 3;
				
				Set_Sprite_Zero();
				Draw_Background();
				
				Reset_Music();
				song = SONG_GAME;
				Play_Music(song);
				
				Wait_Vblank();
				//All_On(); //slight change...
					PPU_CTRL = 0x91;
					
			}
			Music_Update();
			
			NMI_flag = 0;
		}
		
		
		while (Game_Mode == RUN_GAME_MODE){ // Game Mode
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			Blank_sprite();
			Set_Sprite_Zero();
			

			move_logic();
			Draw_Hero();
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){ // start Pause Mode
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Pause_Sprites();
				Reset_Music();
				song = SONG_PAUSE;
				Play_Music(song);
				Game_Mode = PAUSE_MODE;
			}
			if (((joypad1old & SELECT) == 0)&&((joypad1 & SELECT) != 0)){ // testing Game Over
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = SONG_GAME_OVER;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
			
			// test sound fx
			if (((joypad1old & UP) == 0)&&((joypad1 & UP) != 0)){
				Play_Fx(SOUND_SHOOT);
			}
			if (((joypad1old & DOWN) == 0)&&((joypad1 & DOWN) != 0)){
				Play_Fx(SOUND_LASER);
			}
			if (((joypad1old & B_BUTTON) == 0)&&((joypad1 & B_BUTTON) != 0)){
				Play_Fx(SOUND_BOOM);
			}
			if (((joypad1old & A_BUTTON) == 0)&&((joypad1 & A_BUTTON) != 0)){
				Play_Fx(SOUND_BIG_BOOM);
			}
			
			NMI_flag = 0;
		}

		
		while (Game_Mode == PAUSE_MODE){ // Pause Mode
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){ // Go back to Game Mode
				Blank_sprite();
				Set_Sprite_Zero();
				
				Reset_Music();
				song = SONG_GAME;
				Play_Music(song);
				Game_Mode = RUN_GAME_MODE;
			}
			
			NMI_flag = 0;
		}
		
		
		while (Game_Mode == GAME_OVER_MODE){ // Game Over
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			
			
			Music_Update();
			if (((joypad1old & SELECT) == 0)&&((joypad1 & SELECT) != 0)){ // testing Victory
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Victory_Sprites();
				Reset_Music();
				song = SONG_VICTORY;
				Play_Music(song);
				Wait =  0; // if no input for a few seconds
				Wait2 = 0; // go back to title screen
				Game_Mode = VICTORY_MODE; // just testing it
			}
			else{
				++Wait;
				if (Wait == 0)
					++Wait2;
				if (Wait2 == 2){
					All_Off();
					Reset(); // go back to title, by resetting the game see reset.s _Reset
				}
			}	
			
			NMI_flag = 0;
		}
		
		
		while (Game_Mode == VICTORY_MODE){ // Victory
			while (NMI_flag == 0); // wait till NMI
			
			Get_Input();
			
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				All_Off();
				Reset(); // go back to title, by resetting the game see reset.s _Reset
			}	
			NMI_flag = 0;
		}
	}
}
	
// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
	
	
	
	
void All_Off(void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}
	
void All_On(void) {
	PPU_CTRL = 0x90; // screen is on, NMI on
	PPU_MASK = 0x1e; 
}		
	
void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}
	
void Load_Palette(void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}


const unsigned char Palette_Fade []={ // for title screen
	0x24, 0x14, 0x04, 0x14
};

void Rotate_Palette(void){ // for title screen
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x0b;
	PPU_DATA = Palette_Fade[(Frame_Count >> 2) & 0x03];
}


void Draw_Hero (void) {
	
	hero_ship.anime = ship_reg;
	if ((joypad1 & (LEFT)) != 0) hero_ship.anime = ship_left;
	if ((joypad1 & (RIGHT)) != 0) hero_ship.anime = ship_right;
	hero_ship.dir = 0; // never flipped
	++DEBUG;
	Draw_Sprites((int)&hero_ship);
	++DEBUG; // 931 cycles
}



void move_logic(void) {
	if ((joypad1 & (RIGHT|LEFT)) == 0){ // no L or R
		// apply friction, if no L or R
		if (X_speed > 0) // if positive, going right
			--X_speed;

		if (X_speed < 0) // going left
			++X_speed;
	}
	
	if ((joypad1 & RIGHT) > 0){
		if (X_speed >= 0){ // going right
			X_speed += 2;
		}
		else { // going left
			X_speed += 4; // double friction
		}
	}
	if ((joypad1 & LEFT) > 0){
		if (X_speed < 0){ // going left
			X_speed -= 2;
		}
		else { // going right
			X_speed -= 4; // double friction
		}
	}
	
	// physics here
	// max speeds
	if (X_speed >= 0){ // going right
		if (X_speed > 0x34)
			X_speed = 0x34;
	}
	else {
		if (X_speed < (-0x24))
			X_speed = (-0x24); // 0xe0
	}
		

	// move
	X_old = hero_ship.X;
	hero_ship.X += X_speed>>4;
	if (hero_ship.X > 0xf0){ // too far right or left
		hero_ship.X = X_old;
		X_speed = 7; // was 0, but I was getting a pause on Right
	}
	
	// scroll up
	--Vert_scroll;
	if (Vert_scroll > 0xef){
		Vert_scroll = 0xef; // screen is only 240 pixels high
	}
}


void Draw_Title(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Title);	// uncompresses our data
	
	PPU_ADDRESS = 0x24; // draw the HUD on opposite nametable
	PPU_ADDRESS = 0x64; 
	for(index=0;index < sizeof(HUD);++index){
		PPU_DATA = HUD[index];
	}
	PPU_ADDRESS = 0x24;
	PPU_ADDRESS = 0x74;
	for(index=0;index < sizeof(HUD);++index){
		PPU_DATA = HUD2[index];
	}
	// attribute table HUD
	PPU_ADDRESS = 0x27;
	PPU_ADDRESS = 0xc0;
	for(index=0;index < 8;++index){
		PPU_DATA = 0xff;
	}
}


void Draw_Background(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Stars);	// uncompresses our data
	PPU_ADDRESS = 0x3f; // address of BG#0 Palette
	PPU_ADDRESS = 0x01;
	PPU_DATA = 3;
	PPU_DATA = 0x27;
	PPU_DATA = 5;
}


void Turn_On_Pause_Sprites(void){
	dummy.anime = Pause_Sprites;
	dummy.dir = 0;
	dummy.X = 0x6c;
	dummy.Y = 0x70;
	++DEBUG;
	Draw_Sprites((int)&dummy);
	++DEBUG; // 785 cycles
}

// note 657 cycles per 4 tile object
//10,000 / 657 = 15 estimate of how many objects I can have on screen



void Turn_On_Game_Over_Sprites(void){
	dummy.anime = Game_O_Sprites;
	dummy.dir = 0;
	dummy.X = 0x5c;
	dummy.Y = 0x70;
	++DEBUG;
	Draw_Sprites((int)&dummy);
	++DEBUG; // 1169 cycles
}




void Turn_On_Victory_Sprites(void){
	dummy.anime = Vic_Sprites;
	dummy.dir = 0;
	dummy.X = 0x60;
	dummy.Y = 0x70;
	Draw_Sprites((int)&dummy);

}


void Set_Sprite_Zero(void){
	SPRITES[0] = 0x1d;
	SPRITES[1] = 0xff; // the last tile in the set
	SPRITES[2] = 0x20;
	SPRITES[3] = 0xb9;
}