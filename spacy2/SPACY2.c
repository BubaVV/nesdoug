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
			 
			if (((joypad1old & SELECT) == 0)&&((joypad1 & SELECT) != 0)){ // select = spawn 8 enemies
				Spawn_Enemies();
			}
			if (((joypad1old & DOWN) == 0)&&((joypad1 & DOWN) != 0)){ // select = spawn 8 enemies bullets
				Spawn_Bullets();
			}
			
			if (injury_Timer == 0){
				if (invincible == 0){
					collision_Test2(); // get injured
				}
			}
			else {
				joypad1 = 0;
				++injury_Timer;
				if (injury_Timer == 120) {
					injury_Timer = 0;
					invincible = 30;
					hero_ship.X += 4;
				}
				if ((injury_Timer == 40)&&(hero_ship.X >= 4)) hero_ship.X -= 4; // shift left for larger explosion
			}
			if (invincible != 0)
				--invincible;
			if (Lives == 0xff){
				++Death_Flag; // wait a little before entering death mode
				Lives = 0;
			}
			
			move_logic();
			shooting();
			
			collision_Test(); // hit enemy
			
			
			Draw_Bullets(); // and move them
			Draw_Hero();
			Draw_Enemies(); // and move them
			Draw_Explosion(); // and animate
			
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
				Game_Mode = PAUSE_MODE;
			}
			
			
			if ((injury_Timer == 119)&&(Death_Flag !=0)) { // init game over
				Blank_sprite();
				Set_Sprite_Zero();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = SONG_GAME_OVER;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
			//++DEBUG; // scanline 170 ish, good.
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

			++Wait;
			if (Wait == 0)
				++Wait2;
			if (Wait2 == 2){ // about 8 seconds
				All_Off();
				Reset(); // go back to title, by resetting the game see reset.s _Reset
			}
			
			NMI_flag = 0;
		}
		
		
		while (Game_Mode == VICTORY_MODE){ // Victory
			while (NMI_flag == 0); // wait till NMI
			
			Get_Input();
			
			Music_Update();
			
			if (Wait2 < 2) // about 8 seconds
				++Wait;
			if (Wait == 0)
				++Wait2;
			if (Wait2 == 2){ // after waiting 8 seconds, we can skip back to start
				if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
					All_Off();
					Reset(); // go back to title, by resetting the game see reset.s _Reset
				}	
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
	if (injury_Timer == 0){
		hero_ship.anime = ship_reg;
		if ((joypad1 & (LEFT)) != 0) hero_ship.anime = ship_left;
		if ((joypad1 & (RIGHT)) != 0) hero_ship.anime = ship_right;
	}
	else if (injury_Timer < 40){
		hero_ship.anime = ship_expl_1;
	}
	else if (injury_Timer < 80){
		hero_ship.anime = ship_expl_2;
	}
	else {
		hero_ship.anime = ship_expl_3;
	}
	

	hero_ship.dir = 0; // never flipped
	Draw_Sprites(&hero_ship);
	// 931 cycles
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


void shooting(void) { // can the hero shoot bullets
	if (bullet_Wait != 0) {
		--bullet_Wait;
	}
	else {
		if ((joypad1 & A_B_BUTTON) != 0) // either A or B
			spawn_bullets();
	}
}

void spawn_bullets(void){
	Play_Fx(SOUND_SHOOT);
	bullet_Wait = 0x14; // return;
	if (hero_bullet1.Y == 0){
		hero_bullet1.X = hero_ship.X;
		hero_bullet1.Y = hero_ship.Y;
		hero_bullet1.Y_speed = 0xe0;
		hero_bullet1.tile = 0xe0;
		return;
	}
	if (hero_bullet2.Y == 0){
		hero_bullet2.X = hero_ship.X;
		hero_bullet2.Y = hero_ship.Y;
		hero_bullet2.Y_speed = 0xe0;
		hero_bullet2.tile = 0xe0;
		return;
	}
	if (hero_bullet3.Y == 0){
		hero_bullet3.X = hero_ship.X;
		hero_bullet3.Y = hero_ship.Y;
		hero_bullet3.Y_speed = 0xe0;
		hero_bullet3.tile = 0xe0;
		return;
	}
	if (hero_bullet4.Y == 0){
		hero_bullet4.X = hero_ship.X;
		hero_bullet4.Y = hero_ship.Y;
		hero_bullet4.Y_speed = 0xe0;
		hero_bullet4.tile = 0xe0;
		return;
	}
	if (hero_bullet5.Y == 0){
		hero_bullet5.X = hero_ship.X;
		hero_bullet5.Y = hero_ship.Y;
		hero_bullet5.Y_speed = 0xe0;
		hero_bullet5.tile = 0xe0;
		return;
	}
	if (hero_bullet6.Y == 0){
		hero_bullet6.X = hero_ship.X;
		hero_bullet6.Y = hero_ship.Y;
		hero_bullet6.Y_speed = 0xe0;
		hero_bullet6.tile = 0xe0;
		return;
	}
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

	Draw_Sprites(&dummy);
	// 785 cycles
}

// note 657 cycles per 4 tile object
//10,000 / 657 = 15 estimate of how many objects I can have on screen



void Turn_On_Game_Over_Sprites(void){
	dummy.anime = Game_O_Sprites;
	dummy.dir = 0;
	dummy.X = 0x5c;
	dummy.Y = 0x70;

	Draw_Sprites(&dummy);
	// 1169 cycles
}




void Turn_On_Victory_Sprites(void){
	dummy.anime = Vic_Sprites;
	dummy.dir = 0;
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

void Spawn_Enemies(void){ // replace this later
	ENEMY1.X = 0x10;
	ENEMY2.X = 0x30;
	ENEMY3.X = 0x50;
	ENEMY4.X = 0x70;
	ENEMY5.X = 0x90;
	ENEMY6.X = 0xb0;
	ENEMY7.X = 0xd0;
	ENEMY8.X = 0x30;
	ENEMY1.Y = 0x30;
	ENEMY2.Y = 0x30;
	ENEMY3.Y = 0x30;
	ENEMY4.Y = 0x30;
	ENEMY5.Y = 0x30;
	ENEMY6.Y = 0x30;
	ENEMY7.Y = 0x30;
	ENEMY8.Y = 0x50;
	ENEMY1.anime = enemy_1;
	ENEMY2.anime = enemy_1;
	ENEMY3.anime = enemy_2;
	ENEMY4.anime = enemy_2;
	ENEMY5.anime = enemy_3;
	ENEMY6.anime = enemy_3;
	ENEMY7.anime = enemy_4;
	ENEMY8.anime = enemy_4;
	ENEMY1.dir = 0;
	ENEMY2.dir = 0;
	ENEMY3.dir = 0;
	ENEMY4.dir = 0;
	ENEMY5.dir = 1; //flipped, testing
	ENEMY6.dir = 0;
	ENEMY7.dir = 0;
	ENEMY8.dir = 0;
}

void Spawn_Bullets(void) { // replace this later
	enemy_bullet1.X = 0x10;
	enemy_bullet2.X = 0x30;
	enemy_bullet3.X = 0x50;
	enemy_bullet4.X = 0x70;
	enemy_bullet5.X = 0x90;
	enemy_bullet6.X = 0xb0;
	enemy_bullet7.X = 0xd0;
	enemy_bullet8.X = 0x30;
	enemy_bullet1.Y = 0x30;
	enemy_bullet2.Y = 0x30;
	enemy_bullet3.Y = 0x30;
	enemy_bullet4.Y = 0x30;
	enemy_bullet5.Y = 0x30;
	enemy_bullet6.Y = 0x30;
	enemy_bullet7.Y = 0x30;
	enemy_bullet8.Y = 0x50;
	enemy_bullet1.X_speed = 0x0;
	enemy_bullet2.X_speed = 0x0;
	enemy_bullet3.X_speed = 0xf0;
	enemy_bullet4.X_speed = 0xfc;
	enemy_bullet5.X_speed = 0x0;
	enemy_bullet6.X_speed = 0x0;
	enemy_bullet7.X_speed = 0x10;
	enemy_bullet8.X_speed = 0x04;
	enemy_bullet1.Y_speed = 0x10;
	enemy_bullet2.Y_speed = 0x04;
	enemy_bullet3.Y_speed = 0;
	enemy_bullet4.Y_speed = 0;
	enemy_bullet5.Y_speed = 0xf0;
	enemy_bullet6.Y_speed = 0xfc;
	enemy_bullet7.Y_speed = 0;
	enemy_bullet8.Y_speed = 0;
	enemy_bullet1.tile = 0xe1;
	enemy_bullet2.tile = 0xe1;
	enemy_bullet3.tile = 0xe2;
	enemy_bullet4.tile = 0xe2;
	enemy_bullet5.tile = 0xe3;
	enemy_bullet6.tile = 0xe3;
	enemy_bullet7.tile = 0xe4;
	enemy_bullet8.tile = 0xe4;
	
}

void Draw_Enemies(void){ // and move them
// I made some alternates, for shuffling of sprite priorities

	++Frame_Count2;
	if ((Frame_Count2 & 3) == 0){

		if (ENEMY1.Y != 0){
			move_enemies(&ENEMY1);
		}
		if (ENEMY2.Y != 0){
			move_enemies(&ENEMY2);
		}
		if (ENEMY3.Y != 0){
			move_enemies(&ENEMY3);
		}
		if (ENEMY4.Y != 0){
			move_enemies(&ENEMY4);
		}
		if (ENEMY5.Y != 0){
			move_enemies(&ENEMY5);
		}
		if (ENEMY6.Y != 0){
			move_enemies(&ENEMY6);
		}
		if (ENEMY7.Y != 0){
			move_enemies(&ENEMY7);
		}
		if (ENEMY8.Y != 0){
			move_enemies(&ENEMY8);
		}
	}	
	else if ((Frame_Count2 & 3) == 1){
		if (ENEMY1.Y != 0){
			move_enemies(&ENEMY1);
		}
		if (ENEMY3.Y != 0){
			move_enemies(&ENEMY3);
		}
		if (ENEMY5.Y != 0){
			move_enemies(&ENEMY5);
		}
		if (ENEMY7.Y != 0){
			move_enemies(&ENEMY7);
		}
		if (ENEMY2.Y != 0){
			move_enemies(&ENEMY2);
		}
		if (ENEMY4.Y != 0){
			move_enemies(&ENEMY4);
		}
		if (ENEMY6.Y != 0){
			move_enemies(&ENEMY6);
		}
		if (ENEMY8.Y != 0){
			move_enemies(&ENEMY8);
		}
	}
	else if ((Frame_Count2 & 3) == 2){
		if (ENEMY8.Y != 0){
			move_enemies(&ENEMY8);
		}
		if (ENEMY7.Y != 0){
			move_enemies(&ENEMY7);
		}
		if (ENEMY6.Y != 0){
			move_enemies(&ENEMY6);
		}
		if (ENEMY5.Y != 0){
			move_enemies(&ENEMY5);
		}
		if (ENEMY4.Y != 0){
			move_enemies(&ENEMY4);
		}
		if (ENEMY3.Y != 0){
			move_enemies(&ENEMY3);
		}
		if (ENEMY2.Y != 0){
			move_enemies(&ENEMY2);
		}
		if (ENEMY1.Y != 0){
			move_enemies(&ENEMY1);
		}
	}
	else{
		if (ENEMY8.Y != 0){
			move_enemies(&ENEMY8);
		}
		if (ENEMY6.Y != 0){
			move_enemies(&ENEMY6);
		}
		if (ENEMY4.Y != 0){
			move_enemies(&ENEMY4);
		}
		if (ENEMY2.Y != 0){
			move_enemies(&ENEMY2);
		}
		if (ENEMY7.Y != 0){
			move_enemies(&ENEMY7);
		}
		if (ENEMY5.Y != 0){
			move_enemies(&ENEMY5);
		}
		if (ENEMY3.Y != 0){
			move_enemies(&ENEMY3);
		}
		if (ENEMY1.Y != 0){
			move_enemies(&ENEMY1);
		}
	};
}



void Spawn_Explosion (void){
	if (Explode1.Y == 0){
		Explode1.Y = objectB_y;
		Explode1.X = objectB_x;
		Explode1.anime = enemy_expl;
		Explode1.count = 20; // count down
	}
	else if (Explode2.Y == 0){
		Explode2.Y = objectB_y;
		Explode2.X = objectB_x;
		Explode2.anime = enemy_expl;
		Explode2.count = 20; // count down
	}
	else if (Explode3.Y == 0){
		Explode3.Y = objectB_y;
		Explode3.X = objectB_x;
		Explode3.anime = enemy_expl;
		Explode3.count = 20; // count down
	}
	else if (Explode4.Y == 0){
		Explode4.Y = objectB_y;
		Explode4.X = objectB_x;
		Explode4.anime = enemy_expl;
		Explode4.count = 20; // count down
	}
	
}


void All_Enemies(void){ // tests each bullet vs each enemy
	collision = 0;
	if (ENEMY1.Y != 0){
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
	if (ENEMY2.Y != 0){
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
	if (ENEMY3.Y != 0){
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
	if (ENEMY4.Y != 0){
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
	if (ENEMY5.Y != 0){
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
	if (ENEMY6.Y != 0){
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
	if (ENEMY7.Y != 0){
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
	if (ENEMY8.Y != 0){
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
