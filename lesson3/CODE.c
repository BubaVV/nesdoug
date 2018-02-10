void All_Off(void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}
	
void All_On(void) {
	PPU_CTRL = 0x90; //	screen is on, NMI on
	PPU_MASK = 0x1e; 
}
	
void Load_Palette(void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
	PPU_ADDRESS = 0x23;
	PPU_ADDRESS = 0xda;
	for( index = 0; index < sizeof(Attrib_Table); ++index ){
		PPU_DATA = Attrib_Table[index];
	}
}

void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}

