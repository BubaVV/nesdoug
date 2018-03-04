/*	for cc65, for NES
 *	complete space shooter game, pt 2
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
	
// TITLE	
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
				// Lives = 3; // moved to reset.s, so continuing doesn't give you 3 lives
				Wave = 0;
				Master_Delay = 50;
				Clear_Enemies();
				
				Set_Sprite_Zero();
				Draw_Background();
				
				Reset_Music();
				song = SONG_GAME;
				Play_Music(song);
				
				Wait_Vblank();
				// was All_On(); changed to...
					PPU_CTRL = 0x91;
					
			}
			Music_Update();
			
			NMI_flag = 0;
		}
		
// GAME		
		while (Game_Mode == RUN_GAME_MODE){ // Game Mode
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			Blank_sprite();
			Set_Sprite_Zero();
			
			
			if (Master_Delay != 0){
				--Master_Delay;
				if (Master_Delay == 0){
					++Init_Wave_Flag;
				}
			}
			
			if (Init_Wave_Flag != 0) {
				Init_Wave_Flag = 0;
				Init_New_Wave();
			}
			
			Check_Wave_Over();
			
			if (injury_Timer == 0){
				shooting();
			}
			Check_Injury();
			
			move_logic();
			
			
			collision_Test(); // hit enemy
			
			Randomly_Shoot(); // a second enemy shooting function
			
			
			Draw_Bullets(); // and move them
			Draw_Hero();
			Draw_Enemies2(); // and move them
			//Draw_Explosion(); // moved to draw_enemies function
			
			Set_Sprite_Zero(); // redundant...just double checking
			Music_Update();
			Adjust_Scoreboard();
			
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){ // start Pause Mode
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Pause_Sprites();
				Reset_Music();
				song = SONG_PAUSE;
				Play_Music(song);
				Save_Game_Mode = Game_Mode;
				Game_Mode = PAUSE_MODE;
			}
			
			
			if ((injury_Timer == 99)&&(Death_Flag !=0)) { // init game over
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = SONG_GAME_OVER;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
			

			// ++DEBUG; // scanline 180 ish, good.

			NMI_flag = 0;
		}
		
// BOSS		
		while (Game_Mode == BOSS_MODE){ // Title Screen
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			Blank_sprite();
			Set_Sprite_Zero();
			
			if (injury_Timer == 0){
				shooting();
			}
			Check_Injury();
			
			move_logic();
			
			collision_Test4(); // hit boss
			if (Game_Mode != BOSS_MODE) break;
			
			Draw_Bullets(); // and move them
			Draw_Hero();
			
			Move_Lasers(); // and draw them
			
			Move_Boss(); // and draw him
			
			
			
			Set_Sprite_Zero(); // redundant...just double checking
			Music_Update();
			Adjust_Scoreboard();
			
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){ // start Pause Mode
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Pause_Sprites();
				Reset_Music();
				song = SONG_PAUSE;
				Play_Music(song);
				Save_Game_Mode = Game_Mode;
				Game_Mode = PAUSE_MODE;
			}
			
			if ((injury_Timer == 99)&&(Death_Flag !=0)) { // init game over
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = SONG_GAME_OVER;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
			
			NMI_flag = 0;
		}

// PAUSE		
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
				Game_Mode = Save_Game_Mode;
			}
			
			NMI_flag = 0;
		}
		
// GAME OVER		
		while (Game_Mode == GAME_OVER_MODE){ // Game Over
			while (NMI_flag == 0); // wait till v-blank
			
			Get_Input();
			
			
			Music_Update();

			++Wait;
			if (Wait == 0)
				++Wait2;
			if (Wait2 == 2){ // about 8 seconds
				All_Off();
				Reset(); // go back to title, by resetting the game see reset.s _Reset
			}
			
			NMI_flag = 0;
		}
		
// VICTORY		
		while (Game_Mode == VICTORY_MODE){ // Victory
			while (NMI_flag == 0); // wait till NMI
			
			Get_Input();
			
			Blank_sprite();
			Set_Sprite_Zero();
			Turn_On_Victory_Sprites();
			Draw_Hero();
			
			
			Music_Update();
			
			if (Wait2 < 2){ // about 8 seconds
				++Wait;
				if (Wait == 0) ++Wait2;
			}
			if (Wait2 == 2){ // after waiting 8 seconds, we can skip back to start
				if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
					NMI_flag = 0;
					while (NMI_flag == 0); // wait till NMI
					All_Off();
					PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
					PPU_ADDRESS = 0x00;
					UnRLE(Keep);	// uncompresses our data
					PPU_ADDRESS = 0x3f; // address of nametable #0 = 0x2000
					PPU_ADDRESS = 0x03;
					PPU_DATA = 0x30; // change 1 color to white
					Reset_Scroll();
					
					Game_Mode = TITLE_MODE; // not really
					Blank_sprite();
					Selection = 0;
					Wait_Vblank();
					All_On();
					NMI_flag = 0;
					Keep_Loop(); // exit from here
					
				}	
			}
			NMI_flag = 0;
		}
	}
}
	
// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank




void Keep_Loop(void){
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI
		
		if (Selection == 0) {
			SPRITES[0] = 0x57;
		}
		else SPRITES[0] = 0x67;
		
		SPRITES[1] = 0x0d;
		SPRITES[2] = 0x00;
		SPRITES[3] = 0x50;
		Get_Input();
		
		if ((joypad1old & UP) != 0) Selection = 0; // yes
		
		if ((joypad1old & DOWN) != 0) Selection = 1; // no
		
		if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
			NMI_flag = 0;
			while (NMI_flag == 0); // wait till v-blank
			
			if (Selection == 0){
				Reset_Keep_Score();
			}
			else {
				Reset(); // go back to title, by resetting the game see reset.s _Reset
			}
		}
		
		Music_Update();
		
		NMI_flag = 0;
	}
}



	
	
	
void Check_Injury (void) {
	if (injury_Timer == 0){
		if (invincible == 0){
			if (Game_Mode == RUN_GAME_MODE){
				collision_Test2(); // get injured
			}
			else if (Game_Mode == BOSS_MODE){
				collision_Test3(); // get injured
			}
		}
	}
	else {
		joypad1 = 0;
		++injury_Timer;
		if (injury_Timer == 100) {
			injury_Timer = 0;
			invincible = 30;
		}

	}
	if (invincible != 0)
		--invincible;
	if (Lives == 0xff){
		++Death_Flag; // wait a little before entering death mode
		Lives = 0;
	}
}	
	


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
	if (injury_Timer == 0){
		hero_ship.anime = ship_reg;
		if ((joypad1 & (LEFT)) != 0) hero_ship.anime = ship_left;
		if ((joypad1 & (RIGHT)) != 0) hero_ship.anime = ship_right;
	}
	else if (injury_Timer < 10){
		hero_ship.anime = ship_expl_1;
	}
	else if (injury_Timer < 20){
		hero_ship.anime = ship_expl_2;
	}
	else if (injury_Timer < 30){
		hero_ship.anime = ship_expl_3;
	}
	else return;

	hero_ship.dir = NO_FLIP; // never flipped
		//++DEBUG;
	Draw_Sprites(&hero_ship);
		//++DEBUG;
	// 854 cycles, no flip
	// 1136 cycles, h flip
	// 1151 cycles, v flip
	// 1421 cycles, both flipped
}



void move_logic(void) {
	if ((joypad1 & (RIGHT|LEFT)) == 0){ // no L or R
		// apply friction, if no L or R
		// it was 0, but I was getting a delay on Right presses, now 7
		if (X_speed > 7) // if positive, going right
			X_speed-=2;

		if (X_speed < 6) // going left
			X_speed+=2;
	}
	
	if ((joypad1 & RIGHT) != 0){
		if (X_speed >= 7){ // going right
			X_speed += 2;
		}
		else { // going left
			X_speed += 4; // double friction
		}
	}
	if ((joypad1 & LEFT) != 0){
		if (X_speed <= 7){ // going left
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
		if (X_speed < (-0x34))
			X_speed = (-0x34); // 0xe0
	}
		

	// move
	hero_ship.X += ((X_speed+8)>>4);
	
	if (hero_ship.X < 0x08){ // too far left
		hero_ship.X = 0x08;
		X_speed = 7; // stopped. it was 0, but I was getting a delay on Right presses
	}
	
	if (hero_ship.X > 0xe8){ // too far right
		hero_ship.X = 0xe8;
		X_speed = 7; // stopped. it was 0, but I was getting a delay on Right presses
	}
	
	// scroll up
	--Vert_scroll;
	if (Vert_scroll > 0xef){
		Vert_scroll = 0xef; // screen is only 240 pixels high
	}
}


void shooting(void) { // can the hero shoot bullets
	if ((joypad1 & A_B_BUTTON) == 0) bullet_Wait = 0; // allow rapid taps


	if (bullet_Wait != 0) {
		--bullet_Wait;
	}
	else {
		if ((joypad1 & A_B_BUTTON) != 0) // either A or B
			spawn_bullets();
	}
}

void spawn_bullets(void){
	
	bullet_Wait = 0x10; // return;
	if (hero_bullet1.Y == 0){
		hero_bullet1.X = hero_ship.X;
		hero_bullet1.Y = hero_ship.Y;
		hero_bullet1.Y_speed = 0xe0;
		hero_bullet1.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	if (hero_bullet2.Y == 0){
		hero_bullet2.X = hero_ship.X;
		hero_bullet2.Y = hero_ship.Y;
		hero_bullet2.Y_speed = 0xe0;
		hero_bullet2.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	if (hero_bullet3.Y == 0){
		hero_bullet3.X = hero_ship.X;
		hero_bullet3.Y = hero_ship.Y;
		hero_bullet3.Y_speed = 0xe0;
		hero_bullet3.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	if (hero_bullet4.Y == 0){
		hero_bullet4.X = hero_ship.X;
		hero_bullet4.Y = hero_ship.Y;
		hero_bullet4.Y_speed = 0xe0;
		hero_bullet4.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	if (hero_bullet5.Y == 0){
		hero_bullet5.X = hero_ship.X;
		hero_bullet5.Y = hero_ship.Y;
		hero_bullet5.Y_speed = 0xe0;
		hero_bullet5.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	if (hero_bullet6.Y == 0){
		hero_bullet6.X = hero_ship.X;
		hero_bullet6.Y = hero_ship.Y;
		hero_bullet6.Y_speed = 0xe0;
		hero_bullet6.tile = 0xe0;
		Play_Fx(SOUND_SHOOT);
		return;
	}
	
	// if 6 on screen, end up here...
	bullet_Wait = 0x20;
	/* removed last 2 bullets
	if (hero_bullet7.Y == 0){
		hero_bullet7.X = hero_ship.X;
		hero_bullet7.Y = hero_ship.Y;
		hero_bullet7.Y_speed = 0xe0;
		hero_bullet7.tile = 0xe0;
		return;
	}
	if (hero_bullet8.Y == 0){
		hero_bullet8.X = hero_ship.X;
		hero_bullet8.Y = hero_ship.Y;
		hero_bullet8.Y_speed = 0xe0;
		hero_bullet8.tile = 0xe0;
		return;
	}
	*/
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
	dummy.dir = NO_FLIP;
	dummy.X = 0x6c;
	dummy.Y = 0x70;

	Draw_Sprites(&dummy);
	// 785 cycles
}

// note 657 cycles per 4 tile object
// 10,000 / 657 = 15 estimate of how many objects I can have on screen



void Turn_On_Game_Over_Sprites(void){
	dummy.anime = Game_O_Sprites;
	dummy.dir = NO_FLIP;
	dummy.X = 0x5c;
	dummy.Y = 0x70;

	Draw_Sprites(&dummy);
	// 1169 cycles
}




void Turn_On_Victory_Sprites(void){
	dummy.anime = Vic_Sprites;
	dummy.dir = NO_FLIP;
	dummy.X = 0x60;
	dummy.Y = 0x70;
	Draw_Sprites(&dummy);

}


void Set_Sprite_Zero(void){
	SPRITES[0] = 0x1d;
	SPRITES[1] = 0xff; // the last tile in the set
	SPRITES[2] = 0x20;
	SPRITES[3] = 0xb9;
	Sprite_Index = 4; // so the drawing routine will skip the zero sprite
}



void Draw_Bullets(void){ // and move them
	if (hero_bullet1.Y != 0){
		move_bullets(&hero_bullet1);
	}
	if (hero_bullet2.Y != 0){
		move_bullets(&hero_bullet2);
	}
	if (hero_bullet3.Y != 0){
		move_bullets(&hero_bullet3);
	}
	if (hero_bullet4.Y != 0){
		move_bullets(&hero_bullet4);
	}
	if (hero_bullet5.Y != 0){
		move_bullets(&hero_bullet5);
	}
	if (hero_bullet6.Y != 0){
		move_bullets(&hero_bullet6);
	}
	if (hero_bullet7.Y != 0){
		move_bullets(&hero_bullet7);
	}
	if (hero_bullet8.Y != 0){
		move_bullets(&hero_bullet8);
	}
	
	if (enemy_bullet1.Y != 0){
		move_bullets2(&enemy_bullet1);
	}
	if (enemy_bullet2.Y != 0){
		move_bullets2(&enemy_bullet2);
	}
	if (enemy_bullet3.Y != 0){
		move_bullets2(&enemy_bullet3);
	}
	if (enemy_bullet4.Y != 0){
		move_bullets2(&enemy_bullet4);
	}
	if (enemy_bullet5.Y != 0){
		move_bullets2(&enemy_bullet5);
	}
	if (enemy_bullet6.Y != 0){
		move_bullets2(&enemy_bullet6);
	}
	if (enemy_bullet7.Y != 0){
		move_bullets2(&enemy_bullet7);
	}
	if (enemy_bullet8.Y != 0){
		move_bullets2(&enemy_bullet8);
	}
	
	
}



void Draw_Enemies2(void){ // and move them


	++Frame_Count2;
	
	if (ENEMY1.delay != 0) --ENEMY1.delay;
	if (ENEMY2.delay != 0) --ENEMY2.delay;
	if (ENEMY3.delay != 0) --ENEMY3.delay;
	if (ENEMY4.delay != 0) --ENEMY4.delay;
	if (ENEMY5.delay != 0) --ENEMY5.delay;
	if (ENEMY6.delay != 0) --ENEMY6.delay;
	if (ENEMY7.delay != 0) --ENEMY7.delay;
	if (ENEMY8.delay != 0) --ENEMY8.delay;
	
	
// I made some alternates, for shuffling of sprite priorities every frame
	
	if ((Frame_Count2 & 3) == 0){
		Draw_Explosion();
		
		if (ENEMY1.Y != 0 && ENEMY1.delay == 0){
			Enemy_Pointer = &ENEMY1;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY2.Y != 0 && ENEMY2.delay == 0){
			Enemy_Pointer = &ENEMY2;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY3.Y != 0 && ENEMY3.delay == 0){
			Enemy_Pointer = &ENEMY3;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY4.Y != 0 && ENEMY4.delay == 0){
			Enemy_Pointer = &ENEMY4;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY5.Y != 0 && ENEMY5.delay == 0){
			Enemy_Pointer = &ENEMY5;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY6.Y != 0 && ENEMY6.delay == 0){
			Enemy_Pointer = &ENEMY6;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY7.Y != 0 && ENEMY7.delay == 0){
			Enemy_Pointer = &ENEMY7;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY8.Y != 0 && ENEMY8.delay == 0){
			Enemy_Pointer = &ENEMY8;
			Move_Enemies();
			draw_enemies();
		}
	}	
	else if ((Frame_Count2 & 3) == 1){
		if (ENEMY1.Y != 0 && ENEMY1.delay == 0){
			Enemy_Pointer = &ENEMY1;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY3.Y != 0 && ENEMY3.delay == 0){
			Enemy_Pointer = &ENEMY3;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY5.Y != 0 && ENEMY5.delay == 0){
			Enemy_Pointer = &ENEMY5;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY7.Y != 0 && ENEMY7.delay == 0){
			Enemy_Pointer = &ENEMY7;
			Move_Enemies();
			draw_enemies();
		}
		
		Draw_Explosion();
		
		if (ENEMY2.Y != 0 && ENEMY2.delay == 0){
			Enemy_Pointer = &ENEMY2;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY4.Y != 0 && ENEMY4.delay == 0){
			Enemy_Pointer = &ENEMY4;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY6.Y != 0 && ENEMY6.delay == 0){
			Enemy_Pointer = &ENEMY6;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY8.Y != 0 && ENEMY8.delay == 0){
			Enemy_Pointer = &ENEMY8;
			Move_Enemies();
			draw_enemies();
		}
	}
	else if ((Frame_Count2 & 3) == 2){
		if (ENEMY8.Y != 0 && ENEMY8.delay == 0){
			Enemy_Pointer = &ENEMY8;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY7.Y != 0 && ENEMY7.delay == 0){
			Enemy_Pointer = &ENEMY7;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY6.Y != 0 && ENEMY6.delay == 0){
			Enemy_Pointer = &ENEMY6;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY5.Y != 0 && ENEMY5.delay == 0){
			Enemy_Pointer = &ENEMY5;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY4.Y != 0 && ENEMY4.delay == 0){
			Enemy_Pointer = &ENEMY4;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY3.Y != 0 && ENEMY3.delay == 0){
			Enemy_Pointer = &ENEMY3;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY2.Y != 0 && ENEMY2.delay == 0){
			Enemy_Pointer = &ENEMY2;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY1.Y != 0 && ENEMY1.delay == 0){
			Enemy_Pointer = &ENEMY1;
			Move_Enemies();
			draw_enemies();
		}
		
		Draw_Explosion();
	}
	else{
		if (ENEMY8.Y != 0 && ENEMY8.delay == 0){
			Enemy_Pointer = &ENEMY8;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY6.Y != 0 && ENEMY6.delay == 0){
			Enemy_Pointer = &ENEMY6;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY4.Y != 0 && ENEMY4.delay == 0){
			Enemy_Pointer = &ENEMY4;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY2.Y != 0 && ENEMY2.delay == 0){
			Enemy_Pointer = &ENEMY2;
			Move_Enemies();
			draw_enemies();
		}
		
		Draw_Explosion();
		
		if (ENEMY7.Y != 0 && ENEMY7.delay == 0){
			Enemy_Pointer = &ENEMY7;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY5.Y != 0 && ENEMY5.delay == 0){
			Enemy_Pointer = &ENEMY5;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY3.Y != 0 && ENEMY3.delay == 0){
			Enemy_Pointer = &ENEMY3;
			Move_Enemies();
			draw_enemies();
		}
		if (ENEMY1.Y != 0 && ENEMY1.delay == 0){
			Enemy_Pointer = &ENEMY1;
			Move_Enemies();
			draw_enemies();
		}
	};
}






void Spawn_Explosion (void){
	if (Explode1.Y == 0){
		Explode1.Y = objectB_y;
		Explode1.X = objectB_x;
		Explode1.anime = enemy_expl;
		Explode1.count = 10; // count down
	}
	else if (Explode2.Y == 0){
		Explode2.Y = objectB_y;
		Explode2.X = objectB_x;
		Explode2.anime = enemy_expl;
		Explode2.count = 10; // count down
	}
	else if (Explode3.Y == 0){
		Explode3.Y = objectB_y;
		Explode3.X = objectB_x;
		Explode3.anime = enemy_expl;
		Explode3.count = 10; // count down
	}
	else if (Explode4.Y == 0){
		Explode4.Y = objectB_y;
		Explode4.X = objectB_x;
		Explode4.anime = enemy_expl;
		Explode4.count = 10; // count down
	}
	
}


void All_Enemies(void){ // tests each bullet vs each enemy
	collision = 0;
	if (ENEMY1.Y != 0 && ENEMY1.delay == 0){
		objectB_y = ENEMY1.Y;
		objectB_x = ENEMY1.X;
		Hitbox();
		if (collision != 0){
			ENEMY1.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY2.Y != 0 && ENEMY2.delay == 0){
		objectB_y = ENEMY2.Y;
		objectB_x = ENEMY2.X;
		Hitbox();
		if (collision != 0){
			ENEMY2.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY3.Y != 0 && ENEMY3.delay == 0){
		objectB_y = ENEMY3.Y;
		objectB_x = ENEMY3.X;
		Hitbox();
		if (collision != 0){
			ENEMY3.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY4.Y != 0 && ENEMY4.delay == 0){
		objectB_y = ENEMY4.Y;
		objectB_x = ENEMY4.X;
		Hitbox();
		if (collision != 0){
			ENEMY4.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY5.Y != 0 && ENEMY5.delay == 0){
		objectB_y = ENEMY5.Y;
		objectB_x = ENEMY5.X;
		Hitbox();
		if (collision != 0){
			ENEMY5.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY6.Y != 0 && ENEMY6.delay == 0){
		objectB_y = ENEMY6.Y;
		objectB_x = ENEMY6.X;
		Hitbox();
		if (collision != 0){
			ENEMY6.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY7.Y != 0 && ENEMY7.delay == 0){
		objectB_y = ENEMY7.Y;
		objectB_x = ENEMY7.X;
		Hitbox();
		if (collision != 0){
			ENEMY7.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
	if (ENEMY8.Y != 0 && ENEMY8.delay == 0){
		objectB_y = ENEMY8.Y;
		objectB_x = ENEMY8.X;
		Hitbox();
		if (collision != 0){
			ENEMY8.Y = 0; // turn off
			Spawn_Explosion();
			Score4 += 5;
			return;
		}
	}
}

void collision_Test(void){ // hit enemy
	objectA_width = 15; // all bullets and enemies assumed to be the same size
	objectB_width = 15;
	objectB_height = 15; 
	objectA_height = 7;
		
	if (hero_bullet1.Y != 0){
		objectA_y = hero_bullet1.Y;
		objectA_x = hero_bullet1.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet1.Y = 0;
		}
	}
	
	if (hero_bullet2.Y != 0){
		objectA_y = hero_bullet2.Y;
		objectA_x = hero_bullet2.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet2.Y = 0;
		}
	}
	
	if (hero_bullet3.Y != 0){
		objectA_y = hero_bullet3.Y;
		objectA_x = hero_bullet3.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet3.Y = 0;
		}
	}
	
	if (hero_bullet4.Y != 0){
		objectA_y = hero_bullet4.Y;
		objectA_x = hero_bullet4.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet4.Y = 0;
		}
	}
	
	if (hero_bullet5.Y != 0){
		objectA_y = hero_bullet5.Y;
		objectA_x = hero_bullet5.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet5.Y = 0;
		}
	}
	
	if (hero_bullet6.Y != 0){
		objectA_y = hero_bullet6.Y;
		objectA_x = hero_bullet6.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet6.Y = 0;
		}
	}
	
	if (hero_bullet7.Y != 0){
		objectA_y = hero_bullet7.Y;
		objectA_x = hero_bullet7.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet7.Y = 0;
		}
	}
	
	if (hero_bullet8.Y != 0){
		objectA_y = hero_bullet8.Y;
		objectA_x = hero_bullet8.X;
		
		All_Enemies();
		if (collision != 0){
			hero_bullet8.Y = 0;
		}
	}
	
	
	
}


void collision_Test2(void){ // get injured
	objectA_x = hero_ship.X;
	objectA_y = hero_ship.Y;
	objectA_width = 15;
	objectA_height = 15;
	objectB_width = 15; // all the enemy ships assumed to be this size
	objectB_height = 15;
	
	if (ENEMY1.Y != 0){
		objectB_y = ENEMY1.Y;
		objectB_x = ENEMY1.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY2.Y != 0){
		objectB_y = ENEMY2.Y;
		objectB_x = ENEMY2.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY3.Y != 0){
		objectB_y = ENEMY3.Y;
		objectB_x = ENEMY3.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY4.Y != 0){
		objectB_y = ENEMY4.Y;
		objectB_x = ENEMY4.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY5.Y != 0){
		objectB_y = ENEMY5.Y;
		objectB_x = ENEMY5.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY6.Y != 0){
		objectB_y = ENEMY6.Y;
		objectB_x = ENEMY6.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY7.Y != 0){
		objectB_y = ENEMY7.Y;
		objectB_x = ENEMY7.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (ENEMY8.Y != 0){
		objectB_y = ENEMY8.Y;
		objectB_x = ENEMY8.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	
	objectB_width = 7; // all the enemy bullets assumed to be this size
	objectB_height = 7;
	
	if (enemy_bullet1.Y != 0){
		objectB_y = enemy_bullet1.Y;
		objectB_x = enemy_bullet1.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet2.Y != 0){
		objectB_y = enemy_bullet2.Y;
		objectB_x = enemy_bullet2.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet3.Y != 0){
		objectB_y = enemy_bullet3.Y;
		objectB_x = enemy_bullet3.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet4.Y != 0){
		objectB_y = enemy_bullet4.Y;
		objectB_x = enemy_bullet4.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet5.Y != 0){
		objectB_y = enemy_bullet5.Y;
		objectB_x = enemy_bullet5.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet6.Y != 0){
		objectB_y = enemy_bullet6.Y;
		objectB_x = enemy_bullet6.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet7.Y != 0){
		objectB_y = enemy_bullet7.Y;
		objectB_x = enemy_bullet7.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	if (enemy_bullet8.Y != 0){
		objectB_y = enemy_bullet8.Y;
		objectB_x = enemy_bullet8.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	
}



void Adjust_Scoreboard (void) {
	while (Score4 > 9) {
		++Score3;
		Score4 -= 10;
	}
	while (Score3 > 9) {
		++Score2;
		Score3 -= 10;
	}
	
	if (Score2 > 9) {
		++Score1;
		Score2 -= 10;
	}
	if (Score1 > 9) {
		Score4 = 9;
		Score3 = 9;
		Score2 = 9;
		Score1 = 9;
	}
}


void Draw_Explosion (void) {
	if (Explode1.Y != 0){
		Draw_Sprites(&Explode1);
		--Explode1.count;
		if (Explode1.count == 0) Explode1.Y = 0;
	}
	if (Explode2.Y != 0){
		Draw_Sprites(&Explode2);
		--Explode2.count;
		if (Explode2.count == 0) Explode2.Y = 0;
	}
	if (Explode3.Y != 0){
		Draw_Sprites(&Explode3);
		--Explode3.count;
		if (Explode3.count == 0) Explode3.Y = 0;
	}
	if (Explode4.Y != 0){
		Draw_Sprites(&Explode4);
		--Explode4.count;
		if (Explode4.count == 0) Explode4.Y = 0;
	}
}



void Clear_Enemies(void){
	ENEMY1.Y = 0;
	ENEMY2.Y = 0;
	ENEMY3.Y = 0;
	ENEMY4.Y = 0;
	ENEMY5.Y = 0;
	ENEMY6.Y = 0;
	ENEMY7.Y = 0;
	ENEMY8.Y = 0;
	enemy_bullet1.Y = 0;
	enemy_bullet2.Y = 0;
	enemy_bullet3.Y = 0;
	enemy_bullet4.Y = 0;
	enemy_bullet5.Y = 0;
	enemy_bullet6.Y = 0;
	enemy_bullet7.Y = 0;
	enemy_bullet8.Y = 0;
}



void Check_Wave_Over(void){
	
	//the wave is over when all enemies are dead or off screen...Y = 0
	if ((Init_Wave_Flag == 0) && (Master_Delay == 0 )){
	
		Big_Math = (int)ENEMY1.Y; // Big math = 16 bit
		Big_Math += ENEMY2.Y;
		Big_Math += ENEMY3.Y;
		Big_Math += ENEMY4.Y;
		Big_Math += ENEMY5.Y;
		Big_Math += ENEMY6.Y;
		Big_Math += ENEMY7.Y;
		Big_Math += ENEMY8.Y;
		
		if (Big_Math == 0){ // wave is over
			Master_Delay = 100; // will trigger new wave, when reaches 0
			
		}
	}
	
}


/*	STRUCT definition, FOR REFERENCE
 *	unsigned char anime; // for drawing object to screen
 *	unsigned char dir;	// direction, for flipping 0 = L
 *	unsigned char Y;	// top side, if = 0 = off
 *	unsigned char X;	// left side
 *	unsigned char delay; // wait to start moves
 *	unsigned char bullet_type;	// type of bullet
 *	unsigned char move;	// which move to do
 *	unsigned char count; // how far along on the move are we
 */
 
// These are all starting positions of each wave...

// all start at the right at different heights, staggered delay
// use pattern 0
const char Wave0[]={
	SAUCER1,NO_FLIP,0x20,0xff,0x00,1,0,0,
	SAUCER1,NO_FLIP,0x30,0xff,0x10,0,0,0,
	SAUCER1,NO_FLIP,0x40,0xff,0x20,0,0,0,
	SAUCER1,NO_FLIP,0x50,0xff,0x30,0,0,0,
	SAUCER1,NO_FLIP,0x60,0xff,0x40,2,0,0,
	SAUCER1,NO_FLIP,0x70,0xff,0x50,0,0,0,
	SAUCER1,NO_FLIP,0x80,0xff,0x60,3,0,0,
	SAUCER1,NO_FLIP,0x90,0xff,0x70,0,0,0
};

// all start top left, file one at a time downward
// use pattern 1
const char Wave1[]={
	FLY_D,H_FLIP,1,0x10,0,0,0,0,
	FLY_D,H_FLIP,1,0x10,0x10,0,0,0,
	FLY_D,H_FLIP,1,0x10,0x20,0,0,0,
	FLY_D,H_FLIP,1,0x10,0x30,0,0,0,
	FLY_D,H_FLIP,1,0x10,0x40,0,0,0,
	FLY_D,H_FLIP,1,0x10,0x50,4,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

// all start top right, file one at a time downward
// use pattern 2
const char Wave2[]={
	FLY_D,NO_FLIP,1,0xe0,0,0,0,0,
	FLY_D,NO_FLIP,1,0xe0,0x10,0,0,0,
	FLY_D,NO_FLIP,1,0xe0,0x20,0,0,0,
	FLY_D,NO_FLIP,1,0xe0,0x30,0,0,0,
	FLY_D,NO_FLIP,1,0xe0,0x40,0,0,0,
	FLY_D,NO_FLIP,1,0xe0,0x50,4,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

// start at left, split even/odd in timing
// use pattern 0
const char Wave3[]={
	SAUCER1,NO_FLIP,0x20,0,0x00,1,2,0,
	SAUCER1,NO_FLIP,0x30,0,0x40,0,2,0,
	SAUCER1,NO_FLIP,0x40,0,0x10,0,2,0,
	SAUCER1,NO_FLIP,0x50,0,0x50,0,2,0,
	SAUCER1,NO_FLIP,0x60,0,0x20,2,2,0,
	SAUCER1,NO_FLIP,0x70,0,0x60,0,2,0,
	SAUCER1,NO_FLIP,0x80,0,0x30,3,2,0,
	SAUCER1,NO_FLIP,0x90,0,0x70,0,2,0
};

// start low on right, stagger delay
// use pattern 3
const char Wave4[]={
	FLY_L,NO_FLIP,0xa0,0xff,0,1,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x10,0,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x20,0,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x30,0,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x40,2,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x50,0,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x60,0,0,0,
	FLY_L,NO_FLIP,0xa0,0xff,0x70,0,0,0,
};

// start low on left, stagger delay
// use pattern 4
const char Wave5[]={
	FLY_L,H_FLIP,0xa0,0,0,1,0,0,
	FLY_L,H_FLIP,0xa0,0,0x10,0,0,0,
	FLY_L,H_FLIP,0xa0,0,0x20,0,0,0,
	FLY_L,H_FLIP,0xa0,0,0x30,0,0,0,
	FLY_L,H_FLIP,0xa0,0,0x40,3,0,0,
	FLY_L,H_FLIP,0xa0,0,0x50,0,0,0,
	FLY_L,H_FLIP,0xa0,0,0x60,0,0,0,
	FLY_L,H_FLIP,0xa0,0,0x70,0,0,0,
};

// x pattern, starting at top
// use pattern 5
const char Wave6[]={
	FLY_DL,H_FLIP,1,0x10,0,1,0,0,
	FLY_DL,NO_FLIP,1,0xe0,0x10,0,2,0,
	FLY_DL,H_FLIP,1,0x10,0x20,0,0,0,
	FLY_DL,NO_FLIP,1,0xe0,0x30,1,2,0,
	FLY_DL,H_FLIP,1,0x10,0x40,0,0,0,
	FLY_DL,NO_FLIP,1,0xe0,0x50,0,2,0,
	FLY_DL,H_FLIP,1,0x10,0x60,0,0,0,
	FLY_DL,NO_FLIP,1,0xe0,0x70,0,2,0,
};

// \ pattern, starting at top left
// use pattern 5
const char Wave7[]={
	FLY_DL,H_FLIP,1,0x10,0,2,0,0,
	FLY_DL,H_FLIP,1,0x10,0x10,1,0,0,
	FLY_DL,H_FLIP,1,0x10,0x20,0,0,0,
	FLY_DL,H_FLIP,1,0x10,0x30,0,0,0,
	FLY_DL,H_FLIP,1,0x10,0x40,0,0,0,
	FLY_DL,H_FLIP,1,0x10,0x50,0,0,0,
	FLY_DL,H_FLIP,1,0x10,0x60,0,0,0,
	FLY_DL,H_FLIP,1,0x10,0x70,0,0,0,
};

// / pattern, starting at top right
// use pattern 5
const char Wave8[]={
	FLY_DL,NO_FLIP,1,0xe0,0,3,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x10,1,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x20,0,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x30,0,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x40,0,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x50,0,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x60,0,2,0,
	FLY_DL,NO_FLIP,1,0xe0,0x70,0,2,0,
};

// for circle pattern, start upper right
// use pattern 6
const char Wave9[]={
	FLY_L,NO_FLIP,0x28,0xff,0,0,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x10,5,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x20,0,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x30,6,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x40,0,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x50,0,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x60,5,0,0,
	FLY_L,NO_FLIP,0x28,0xff,0x70,0,0,0,
};

// all attack hero, come from every direction
// use pattern 7
const char Wave10[]={
	SAUCER1,NO_FLIP,0x40,0,   0,0,0,0,
	SAUCER1,NO_FLIP,1,   0,   0x10,0,0,0,
	SAUCER1,NO_FLIP,1,   0x30,0x20,0,0,0,
	SAUCER1,NO_FLIP,1,   0x60,0x30,0,0,0,
	SAUCER1,NO_FLIP,1,   0xa0,0x08,0,0,0,
	SAUCER1,NO_FLIP,1,   0xd0,0x18,0,0,0,
	SAUCER1,NO_FLIP,1,   0xff,0x38,0,0,0,
	SAUCER1,NO_FLIP,0x40,0xff,0x48,0,0,0
};


// NOTE, we can reuse wave starting positions
const unsigned char * const All_Wave_Addresses[] = {
	Wave0, Wave3,
	Wave1, Wave2, 
	Wave3,
	Wave1, Wave2,
	Wave7, Wave8,
	Wave1, Wave2,
	Wave7, Wave8,
	Wave6, // 14
	
	Wave0,
	Wave4, Wave5, 
	Wave3,
	Wave4, Wave5,
	Wave0,
	Wave6,
	Wave6, // 23
	
	Wave9,
	Wave1, Wave2,
	Wave9,
	Wave1, Wave2,
	Wave6,
	Wave6, // 31
	
	Wave10,
	Wave9,
	Wave9,
	Wave1, Wave2,
	Wave1, Wave2,
	Wave6,
	Wave6,
	Wave7, Wave8,
	Wave6, // 43
	
	Wave10,
	Wave4, Wave5, 
	Wave10,
	Wave4, Wave5, 
	Wave9,
	Wave9,
	Wave10,
	Wave10,
	Wave10 // 54
	
	};
	
	

// MOST IMPORTANT GAME CONTROL !!!
// value must be 0-7
const char Wave_to_Pattern[] = {
	0,0,1,2,0,1,2,5,5,1,
	2,5,5,5, 0,3,4,0,3,4,
	0,5,5, 6,1,2,6,1,2,5,
	5, 7,6,6,1,2,1,2,5,5,
	5,5,5, 7,3,4,7,3,4,6,
	6,7,7,7
	
	}; // which pattern to use for each wave, see below 'Move_Enemies'






void Init_New_Wave(void){
	if (Wave >= sizeof(Wave_to_Pattern)){
		Init_Boss_Level();
		return;
	}
	
	Pattern = Wave_to_Pattern[Wave];
	
	p_Wave_Data = All_Wave_Addresses[Wave];
	memcpy (&ENEMY1, p_Wave_Data, 64); // enemy1 is the start address for all the enemy objects
	
	++Wave;
}

// for pattern 6, circle, counter-clockwise
// relative positions, 32 total

// high nibble = pixels, low nibble = probability to go plus 1 more
const unsigned char CIRCLE_X []={
	0xe4,0xe8,
	0xec,0xf1,0xf4,0xf7,
	0xfa,0xfd,0,0x03,
	0x06,0x09,0x0c,0x0f,
	0x12,0x15,
	0x18,0x15,
	0x12,0x0f,0x0c,0x09,
	0x06,0x03,0,0xfd,
	0xfa,0xf7,0xf4,0xf1,
	0xec,0xe8,
};


const unsigned char CIRCLE_Y []={
	0,0x03,
	0x06,0x09,0x0c,0x0f,
	0x12,0x15,0x18,0x15,
	0x12,0x0f,0x0c,0x06,
	0x03,0,
	0xfe,0xfd,
	0xfa,0xf7,0xf4,0xf1,
	0xec,0xe8,0xe4,0xe8, // a little more, to adjust
	0xed,0xf1,0xf4,0xf7,
	0xfa,0xfd
};

const unsigned char CIRCLE_D []={ // direction
	NO_FLIP,NO_FLIP,
	NO_FLIP,NO_FLIP,NO_FLIP,NO_FLIP,
	NO_FLIP,NO_FLIP,NO_FLIP,NO_FLIP,
	H_FLIP,H_FLIP,H_FLIP,H_FLIP,
	H_FLIP,H_FLIP,
	H_FLIP,H_FLIP,
	H_FLIP,H_FLIP,H_FLIP,H_FLIP,
	H_FLIP,H_FLIP,H_FLIP,H_FLIP,
	NO_FLIP,NO_FLIP,NO_FLIP,NO_FLIP,
	NO_FLIP,NO_FLIP
};

const unsigned char CIRCLE_A []={ // animation
	FLY_L,FLY_L,
	FLY_DL, FLY_DL, FLY_DL, FLY_DL,
	FLY_D, FLY_D, FLY_D, FLY_D,
	FLY_DL, FLY_DL, FLY_DL, FLY_DL,
	FLY_L, FLY_L, 
	FLY_L, FLY_L,
	FLY_UL, FLY_UL, FLY_UL, FLY_UL,
	FLY_U, FLY_U, FLY_U, FLY_U,
	FLY_UL, FLY_UL, FLY_UL, FLY_UL,
	FLY_L,FLY_L
};




void Move_Enemies(void){
	// each active enemy puts their address in the pointer, and goes here
	// to figure out what move to do
	
	Current_Move = Enemy_Pointer->move;
	
	
	switch (Pattern){
		case 0: // pattern 0

			switch (Current_Move){
				case 0:
					Enemy_Pointer->X -= 2; // go left
					if (Enemy_Pointer->X < 0x10){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->count = 0;
					}
					break;
					
				case 1:
					// wait
					if (Enemy_Pointer->count>0x80){
						Enemy_Pointer->move = 2;
						Enemy_Pointer->count = 0;
					}
					break;
					
				case 2:
					Enemy_Pointer->X += 2; // go right
					if (Enemy_Pointer->X > 0xe0){
						Enemy_Pointer->move = 3;
						Enemy_Pointer->count = 0;
					}
					break;
					
				case 3:
					// wait
					if (Enemy_Pointer->count>0x80){
						Enemy_Pointer->move = 0;
						Enemy_Pointer->count = 0;
					}
					break;
			}
			
			break;
			
		case 1: // pattern 1
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->Y += 2; // go down
					Enemy_Pointer->anime = FLY_D;
					if (Enemy_Pointer->count > 0x20){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->count = 0;
					}
					break;
					
				case 1:
					// go down + right
					Enemy_Pointer->Y += 2; // go down
					Enemy_Pointer->X += 2; // go right
					Enemy_Pointer->anime = FLY_DL;
					if (Enemy_Pointer->count > 0x15){
						Enemy_Pointer->move = 0;
					}
					break;
					
			}
			
			break;
			
		case 2: // pattern 2
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->Y += 2; // go down
					Enemy_Pointer->anime = FLY_D;
					if (Enemy_Pointer->count > 0x20){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->count = 0;
					}
					break;
					
				case 1:
					// go down + left
					Enemy_Pointer->Y += 2; // go down
					Enemy_Pointer->X -= 2; // go left
					Enemy_Pointer->anime = FLY_DL;
					if (Enemy_Pointer->count > 0x15){
						Enemy_Pointer->move = 0;
					}
					break;
					
			}
			
			break;
			
		case 3: // pattern 3
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->X -= 2; // go left
					if (Enemy_Pointer->X < 0xd0){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->anime = FLY_U;
					}
					break;
					
				case 1:
					Enemy_Pointer->Y -= 2; // go up
					if (Enemy_Pointer->Y < 0x30){
						Enemy_Pointer->move = 2;
						Enemy_Pointer->anime = FLY_L;
					}
					break;
					
				case 2:
					Enemy_Pointer->X -= 2; // go left
					if (Enemy_Pointer->X < 0x20){
						Enemy_Pointer->move = 3;
						Enemy_Pointer->anime = FLY_D;
					}
					break;
					
				case 3:
					Enemy_Pointer->Y += 2; // go down
					break;
			}
		
			break;
			
			
		case 4: // pattern 4
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->X += 2; // go right
					if (Enemy_Pointer->X > 0x20){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->anime = FLY_U;
					}
					break;
					
				case 1:
					Enemy_Pointer->Y -= 2; // go up
					if (Enemy_Pointer->Y < 0x30){
						Enemy_Pointer->move = 2;
						Enemy_Pointer->anime = FLY_L;
					}
					break;
					
				case 2:
					Enemy_Pointer->X += 2; // go right
					if (Enemy_Pointer->X > 0xe0){
						Enemy_Pointer->move = 3;
						Enemy_Pointer->anime = FLY_D;
					}
					break;
					
				case 3:
					Enemy_Pointer->Y += 2; // go down
					break;
			}
		
			break;
			
			
		case 5: // pattern 5 - X
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->X += 2; // go right
					Enemy_Pointer->Y += 2; // go down
					if (Enemy_Pointer->Y > 0xd0){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->anime = FLY_UL;
						Enemy_Pointer->dir = NO_FLIP;
					}
					break;
					
				case 1:
					Enemy_Pointer->Y -= 2; // go up
					Enemy_Pointer->X -= 2; // go left
					// continue till off screen
					break;
					
				case 2:
					Enemy_Pointer->X -= 2; // go left
					Enemy_Pointer->Y += 2; // go down
					if (Enemy_Pointer->Y > 0xd0){
						Enemy_Pointer->move = 3;
						Enemy_Pointer->anime = FLY_UL;
						Enemy_Pointer->dir = H_FLIP;
					}
					break;
					
				case 3:
					Enemy_Pointer->Y -= 2; // go up
					Enemy_Pointer->X += 2; // go right
					// continue till off screen
					break;
			}
		
			break;
			
		case 6: // pattern 6 circle
		
			switch (Current_Move){
				case 0:
					Enemy_Pointer->X -= 2; // go left, from right edge
					if (Enemy_Pointer->X < 0x81){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->count = 0xff;
					}
					break;
					
				case 1: // circle counter-clockwise
				
					temp = (Enemy_Pointer->count >> 3); // now 0-31
					temp2 = CIRCLE_Y[temp];	
					Circle_Math();
					Enemy_Pointer->Y += temp3;
					temp2 = CIRCLE_X[temp];
					Circle_Math();
					Enemy_Pointer->X += temp3;
					
					Enemy_Pointer->dir = CIRCLE_D[temp];
					Enemy_Pointer->anime = CIRCLE_A[temp];
					
					if (Enemy_Pointer->count == 0xff) {
						Enemy_Pointer->move = 0;
					}
					break;
			}
			break;
			
			
		case 7: // pattern 7 aim at hero
			switch (Current_Move){
				case 0:
					if (Enemy_Pointer->X > hero_ship.X){
						Enemy_Pointer->X -=1;
					}
					if (Enemy_Pointer->X < hero_ship.X){
						Enemy_Pointer->X +=1;
					}
					Enemy_Pointer->Y +=1;
					
					if (Enemy_Pointer->Y > 0x90){
						Enemy_Pointer->move = 1;
						Enemy_Pointer->count = 0;
					}
					
					break;
				case 1:
					// pause
					if (Enemy_Pointer->count == 0x20) {
						Enemy_Pointer->move = 2;
						Spawn_Bullet_E();
					}
					break;
				case 2:
					Enemy_Pointer->Y -=1; // slow, up
					if (Enemy_Pointer->count == 0x40) Enemy_Pointer->move = 3;
					break;
				case 3:
					Enemy_Pointer->Y -=2; // up and off screen
					break;
			}
	}
	
	++Enemy_Pointer->count;
	
	// spawning shooting
	Current_Type = Enemy_Pointer->bullet_type;
	switch (Current_Type){
		
		// first 3 for enemies moving 2 pixels per frame L/R
		case 1:
			if ((Enemy_Pointer->X == 0x80)||(Enemy_Pointer->X == 0x81)){
				Spawn_Bullet_E();
			}
			break;
		case 2:
			if ((Enemy_Pointer->X == 0x40)||(Enemy_Pointer->X == 0x41)){
				Spawn_Bullet_E();
			}
			break;
		case 3:
			if ((Enemy_Pointer->X == 0xc0)||(Enemy_Pointer->X == 0xc1)){
				Spawn_Bullet_E();
			}
			break;
		case 4:
			if (Enemy_Pointer->Y == 0x41) Spawn_Bullet_E();
			break;
		case 5:
			if (Enemy_Pointer->count == 0x40) Spawn_Bullet_E();
			break;
		case 6:
			if (Enemy_Pointer->count == 0xc0) Spawn_Bullet_E();
			break;
	}	
}






void Spawn_Bullet_E (void) {
	// probably should pass a type of bullet, or some more info
	// and index to an array of bullet data...to vary the bullet behavior
	// not implemented...yet
	
	if (Enemy_Pointer->X > hero_ship.X) {
		X_compare = (Enemy_Pointer->X - hero_ship.X);
		X_compare >>= 3;
		X_compare = (0 - X_compare); 
	}
	else {
		X_compare = (hero_ship.X - Enemy_Pointer->X);
		X_compare >>= 3;
		
	}
	
	if (enemy_bullet1.Y == 0) {
		enemy_bullet1.Y = Enemy_Pointer->Y + 15;
		enemy_bullet1.X = Enemy_Pointer->X + 7;
		enemy_bullet1.tile = 0xe2;
		enemy_bullet1.attrib = 1;
		enemy_bullet1.Y_speed = 0x20;
		enemy_bullet1.X_speed = X_compare;
		return;
	}
	if (enemy_bullet2.Y == 0) {
		enemy_bullet2.Y = Enemy_Pointer->Y + 15;
		enemy_bullet2.X = Enemy_Pointer->X + 7;
		enemy_bullet2.tile = 0xe2;
		enemy_bullet2.attrib = 1;
		enemy_bullet2.Y_speed = 0x20;
		enemy_bullet2.X_speed = X_compare;
		return;
	}
	if (enemy_bullet3.Y == 0) {
		enemy_bullet3.Y = Enemy_Pointer->Y + 15;
		enemy_bullet3.X = Enemy_Pointer->X + 7;
		enemy_bullet3.tile = 0xe2;
		enemy_bullet3.attrib = 1;
		enemy_bullet3.Y_speed = 0x20;
		enemy_bullet3.X_speed = X_compare;
		return;
	}
	if (enemy_bullet4.Y == 0) {
		enemy_bullet4.Y = Enemy_Pointer->Y + 15;
		enemy_bullet4.X = Enemy_Pointer->X + 7;
		enemy_bullet4.tile = 0xe2;
		enemy_bullet4.attrib = 1;
		enemy_bullet4.Y_speed = 0x20;
		enemy_bullet4.X_speed = X_compare;
		return;
	}
	if (enemy_bullet5.Y == 0) {
		enemy_bullet5.Y = Enemy_Pointer->Y + 15;
		enemy_bullet5.X = Enemy_Pointer->X + 7;
		enemy_bullet5.tile = 0xe2;
		enemy_bullet5.attrib = 1;
		enemy_bullet5.Y_speed = 0x20;
		enemy_bullet5.X_speed = X_compare;
		return;
	}
	if (enemy_bullet6.Y == 0) {
		enemy_bullet6.Y = Enemy_Pointer->Y + 15;
		enemy_bullet6.X = Enemy_Pointer->X + 7;
		enemy_bullet6.tile = 0xe2;
		enemy_bullet6.attrib = 1;
		enemy_bullet6.Y_speed = 0x20;
		enemy_bullet6.X_speed = X_compare;
		return;
	}
	if (enemy_bullet7.Y == 0) {
		enemy_bullet7.Y = Enemy_Pointer->Y + 15;
		enemy_bullet7.X = Enemy_Pointer->X + 7;
		enemy_bullet7.tile = 0xe2;
		enemy_bullet7.attrib = 1;
		enemy_bullet7.Y_speed = 0x20;
		enemy_bullet7.X_speed = X_compare;
		return;
	}
	if (enemy_bullet8.Y == 0) {
		enemy_bullet8.Y = Enemy_Pointer->Y + 15;
		enemy_bullet8.X = Enemy_Pointer->X + 7;
		enemy_bullet8.tile = 0xe2;
		enemy_bullet8.attrib = 1;
		enemy_bullet8.Y_speed = 0x20;
		enemy_bullet8.X_speed = X_compare;
		return;
	}
}


void Randomly_Shoot(void){
	// find first active enemy, shoot if framecount2  == 0...every 4 seconds
	// this is the second of 2 ways that bullets spawn
	if (Frame_Count2 == 0){
		if ((ENEMY1.Y != 0)&&(ENEMY1.Y<0x80)){
			Enemy_Pointer = &ENEMY1;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY2.Y != 0)&&(ENEMY2.Y<0x80)){
			Enemy_Pointer = &ENEMY2;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY3.Y != 0)&&(ENEMY3.Y<0x80)){
			Enemy_Pointer = &ENEMY3;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY4.Y != 0)&&(ENEMY4.Y<0x80)){
			Enemy_Pointer = &ENEMY4;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY5.Y != 0)&&(ENEMY5.Y<0x80)){
			Enemy_Pointer = &ENEMY5;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY6.Y != 0)&&(ENEMY6.Y<0x80)){
			Enemy_Pointer = &ENEMY6;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY7.Y != 0)&&(ENEMY7.Y<0x80)){
			Enemy_Pointer = &ENEMY7;
			Spawn_Bullet_E();
			return;
		}
		if ((ENEMY8.Y != 0)&&(ENEMY8.Y<0x80)){
			Enemy_Pointer = &ENEMY8;
			Spawn_Bullet_E();
			return;
		}
	}
}


#include "Boss.c"
