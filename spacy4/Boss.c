// Boss Level Stuff

void Init_Boss_Level(void){
	Game_Mode = BOSS_MODE;
	Clear_Enemies();
	BOSS.anime = BOSS1;
	BOSS.dir = 0;
	BOSS.X = 0x60;
	BOSS.Y = 0x50;
	
	BOSS_health = 100;
}

void collision_Test3(void){ // boss mode, check if injured (by lasers)
	objectA_x = hero_ship.X;
	objectA_y = hero_ship.Y;
	objectA_width = 15;
	objectA_height = 15;
	
	objectB_y = BOSS.Y;
	objectB_x = BOSS.X;
	objectB_width = 47; // Boss size
	objectB_height = 39;
	
	Hitbox();
	if (collision != 0){
		++injury_Timer;
		--Lives;
		Play_Fx(SOUND_BOOM);
		return;
	}
	
	objectB_width = 7; // Laser Size
	objectB_height = 15;
	
	if (LASER1.Y != 0){
		objectB_y = LASER1.Y;
		objectB_x = LASER1.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	
	if (LASER2.Y != 0){
		objectB_y = LASER2.Y;
		objectB_x = LASER2.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	
	if (LASER3.Y != 0){
		objectB_y = LASER3.Y;
		objectB_x = LASER3.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
	
	if (LASER4.Y != 0){
		objectB_y = LASER4.Y;
		objectB_x = LASER4.X;
		
		Hitbox();
		if (collision != 0){
			++injury_Timer;
			--Lives;
			Play_Fx(SOUND_BOOM);
			return;
		}
	}
}


//BOSS_health
void collision_Test4(void){ // boss mode, check if hit the boss
	objectA_width = 15; // all bullets
	objectA_height = 7;
	objectB_width = 47;	// boss
	objectB_height = 39; 
	
	objectB_y = BOSS.Y;
	objectB_x = BOSS.X;
	
	BOSS_health_test = BOSS_health; // test later, see if passed zero
	
	if (hero_bullet1.Y != 0){
		objectA_y = hero_bullet1.Y;
		objectA_x = hero_bullet1.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet1.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet2.Y != 0){
		objectA_y = hero_bullet2.Y;
		objectA_x = hero_bullet2.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet2.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet3.Y != 0){
		objectA_y = hero_bullet3.Y;
		objectA_x = hero_bullet3.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet3.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet4.Y != 0){
		objectA_y = hero_bullet4.Y;
		objectA_x = hero_bullet4.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet4.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet5.Y != 0){
		objectA_y = hero_bullet5.Y;
		objectA_x = hero_bullet5.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet5.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet6.Y != 0){
		objectA_y = hero_bullet6.Y;
		objectA_x = hero_bullet6.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet6.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet7.Y != 0){
		objectA_y = hero_bullet7.Y;
		objectA_x = hero_bullet7.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet7.Y = 0;
			--BOSS_health;
		}
	}
	
	if (hero_bullet8.Y != 0){
		objectA_y = hero_bullet8.Y;
		objectA_x = hero_bullet8.X;
		
		Hitbox();
		if (collision != 0){
			hero_bullet8.Y = 0;
			--BOSS_health;
		}
	}
	
	
	if (BOSS_health > BOSS_health_test) {  // killed the boss
	
		Blank_sprite();
		Set_Sprite_Zero();
		
		SHAKE_AND_BLINK();
		
		Blank_sprite();
		Set_Sprite_Zero();
		
		Reset_Music(); // note, this is famitone init, and I added the music data address. see famitone2.s
		APU_CTRL = 0x1f; // all sound channels back on
		Play_Fx(SILENCE); // I was getting a bug, sound effect was replaying
		song = SONG_VICTORY;
		Play_Music(song);
		Game_Mode = VICTORY_MODE;
		
		Score2 += 2;
		Adjust_Scoreboard();
	}
}

void Move_Boss(void){ // and draw him
	++BOSS.count;

	switch (BOSS.move){
		case 0: // go left

			--BOSS.X;
			if ((BOSS.count == 40)||(BOSS.count == 150)){ // about every 2 seconds, 128 frames, shoot
				Spawn_Lasers();
				BOSS_eyes = 8;
			}
			
			if (BOSS_eyes !=0) {
				--BOSS_eyes;
				BOSS.anime = BOSS2;
			}
			else {
				BOSS.anime = BOSS1;
			}
			
			if (BOSS.X < 8) {
				++BOSS.move;
				BOSS.count = 0;
			}
			break;
		case 1: // shoot, change sprite
			BOSS.anime = BOSS2; // eyes
			if (BOSS.count > 10){
				++BOSS.move;
				BOSS.count = 0;
				Spawn_Lasers();
			}
			break;
		case 2: // go right
			++BOSS.X;
			if ((BOSS.count == 40)||(BOSS.count == 150)){ // about every 2 seconds, 128 frames, shoot
				Spawn_Lasers();
				BOSS_eyes = 8;
			}
			
			if (BOSS_eyes !=0) {
				--BOSS_eyes;
				BOSS.anime = BOSS2;
			}
			else {
				BOSS.anime = BOSS1;
			}
			
			if (BOSS.X > 200) {
				++BOSS.move;
				BOSS.count = 0;
			}
			break;
		case 3: // shoot, change sprite
			BOSS.anime = BOSS2;
			if (BOSS.count > 10){
				++BOSS.move;
				BOSS.count = 0;
				Spawn_Lasers();
			}
			break;
		case 4: // go left until at hero
			BOSS.anime = BOSS1;
			--BOSS.X;
			if (((BOSS.X+16) < hero_ship.X)||(BOSS.X < 10)) {
				++BOSS.move;
				BOSS.count = 0;
			}
			break;
		case 5: // flash eyes
			BOSS.anime = BOSS2;
			if (BOSS.count > 10){
				++BOSS.move;
				BOSS.count = 0;
			}
			break;
		case 6: // drop quickly
			BOSS.anime = BOSS1;
			BOSS.Y += 2;
			if (BOSS.Y > 190) {
				++BOSS.move;
				BOSS.count = 0;
			}
			break;
		case 7: // rise slowly
			--BOSS.Y;
			if (BOSS.count == 60){ // about every 2 seconds, 128 frames, shoot
				Spawn_Lasers();
				BOSS_eyes = 8;
			}
			
			if (BOSS_eyes !=0) {
				--BOSS_eyes;
				BOSS.anime = BOSS2;
			}
			else {
				BOSS.anime = BOSS1;
			}
			
			if (BOSS.Y == 0x50) {
				BOSS.move = 0;
				BOSS.count = 0;
			}
			break;
	}
	

	Draw_Sprites(&BOSS);

}

void Spawn_Lasers(void){
	if (LASER1.Y == 0){
		LASER1.anime = LASER;
		LASER1.dir = 0;
		LASER1.X = BOSS.X + 15;
		LASER1.Y = BOSS.Y + 16;

		LASER2.anime = LASER;
		LASER2.dir = 0;
		LASER2.X = BOSS.X + 38;
		LASER2.Y = BOSS.Y + 16;

		Play_Fx(SOUND_LASER);
		return;
	}
	
	if (LASER3.Y == 0){
		LASER3.anime = LASER;
		LASER3.dir = 0;
		LASER3.X = BOSS.X + 15;
		LASER3.Y = BOSS.Y + 16;

		LASER4.anime = LASER;
		LASER4.dir = 0;
		LASER4.X = BOSS.X + 38;
		LASER4.Y = BOSS.Y + 16;

		Play_Fx(SOUND_LASER);
	}
	
}

void Move_Lasers(void){ // and draw them
	if (LASER1.Y != 0) {
		LASER1.Y += 2;
		if ((Frame_Count&3)==0){
			if (LASER1.X > hero_ship.X) --LASER1.X;
			else if (LASER1.X < hero_ship.X) ++LASER1.X;
		}
		
		Draw_Sprites(&LASER1);
	}
	if (LASER1.Y > 240) {
		LASER1.Y = 0;
	}
	
	if (LASER2.Y != 0) {
		LASER2.Y += 2;
		if ((Frame_Count&3)==0){
			if (LASER2.X > hero_ship.X) --LASER2.X;
			else if (LASER2.X < hero_ship.X) ++LASER2.X;
		}
		
		Draw_Sprites(&LASER2);
	}
	if (LASER2.Y > 240) {
		LASER2.Y = 0;
	}
	
	if (LASER3.Y != 0) {
		LASER3.Y += 2;
		if ((Frame_Count&3)==0){
			if (LASER3.X > hero_ship.X) --LASER3.X;
			else if (LASER3.X < hero_ship.X) ++LASER3.X;
		}
		
		Draw_Sprites(&LASER3);
	}
	if (LASER3.Y > 240) {
		LASER3.Y = 0;
	}
	
	if (LASER4.Y != 0) {
		LASER4.Y += 2;
		if ((Frame_Count&3)==0){
			if (LASER4.X > hero_ship.X) --LASER4.X;
			else if (LASER4.X < hero_ship.X) ++LASER4.X;
		}
		
		Draw_Sprites(&LASER4);
	}
	if (LASER4.Y > 240) {
		LASER4.Y = 0;
	}
}


const unsigned char BOSS_BLINK[]={
	0x02,0x12,0x1f,0x12
};

// special loop after defeating the boss
void SHAKE_AND_BLINK(void){
	Shake_Timer_1 = 0;
	Shake_Timer_2 = 0;
	
	Play_Fx(SOUND_BIG_BOOM);
	APU_CTRL = 8; // only allow noise channel
	NMI_flag = 0;
	
	while(Shake_Timer_2 == 0){
		while (NMI_flag == 0); // wait till v-blank
		
		Flag_Blink_Boss = 1;
		
		Reset_Scroll();
		
		Blank_sprite();
		Set_Sprite_Zero();
		
		Draw_Hero();
		
		Draw_Sprites(&BOSS);

		Music_Update();
		Adjust_Scoreboard();
		
		++Shake_Timer_1;
		if (Shake_Timer_1 == 0) ++Shake_Timer_2;
		
		
		if ((Shake_Timer_1&0x3f) == 0) {
			if (Shake_Timer_1 < 0xb0){
				Play_Fx(SOUND_BIG_BOOM);
			}
		}
		
		if ((Shake_Timer_1&0x05) == 0){
			++BOSS.X;
		}
		else if ((Shake_Timer_1&0x05) == 0x04){
			--BOSS.X;
		}
		
		NMI_flag = 0;
		temp = BOSS_BLINK[((Shake_Timer_1&0x0c)>>2)];
		
	}
	
	Flag_Blink_Boss = 0;
	
}