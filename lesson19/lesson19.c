// Example of bank switching, using MMC3

void __fastcall__ UnRLE(const unsigned char *data);

void Get_Input(void);

void Wait_Vblank(void);




#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80



#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test; 
unsigned char joypad2; 
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char CHRbank;
unsigned char PRGbank;


#pragma rodata-name ("CODE0")
#pragma code-name ("CODE0")
const unsigned char TEXT1[]={
"Bank0"};

#pragma rodata-name ("CODE1")
#pragma code-name ("CODE1")
const unsigned char TEXT2[]={
"Bank1"};

#pragma rodata-name ("CODE2")
#pragma code-name ("CODE2")
const unsigned char TEXT3[]={
"Bank2"};

#pragma rodata-name ("CODE3")
#pragma code-name ("CODE3")
const unsigned char TEXT4[]={
"Bank3"};

#pragma rodata-name ("CODE4")
#pragma code-name ("CODE4")
const unsigned char TEXT5[]={
"Bank4"};

#pragma rodata-name ("CODE5")
#pragma code-name ("CODE5")
const unsigned char TEXT6[]={
"Bank5"};

#pragma rodata-name ("CODE6")
#pragma code-name ("CODE6")
const unsigned char TEXT7[]={
"Bank6"};


#pragma rodata-name ("CODE")
#pragma code-name ("CODE")
// the startup code should be in the last bank!
// bank7

#include "NT.h" // called NT


void All_Off(void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}
	
void All_On(void) {
	PPU_CTRL = 0x88; // screen is on, NMI on
	PPU_MASK = 0x1e; 
}


void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}


const unsigned char Palette[]={
	0x21, 0x09, 0x2a, 0x2a, 0x21, 0x0c, 0x1c, 0x3c
};

void Load_Palette(void){
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for(index = 0;index<sizeof(Palette);++index){
		PPU_DATA = Palette[index];
	}
}



void Draw_Bank_Num(void){
	PPU_ADDRESS = 0x20; //write to address $20a6 of PPU
	PPU_ADDRESS = 0xa6;
	for (index = 0;index < 5;++index){
		PPU_DATA = TEXT1[index];
		/*	this loads the first 5 bytes located at $8000 to the PPU
		 *	TEXT1 is the only thing in PRG BANK 0, in the CFG file
		 *	we defined CODE (BANK0) to start at $8000, so this will compile
		 *	into a load from $8000+index, store to PPU
		 *	however, every time we switch a new bank into $8000, the 
		 *	contents of that bank (at location $8000) will change
		 */
	}
	PPU_ADDRESS = 0; // reset it, so it doesn't affect the scroll position
	PPU_ADDRESS = 0;
};


void Initialize_MMC3(void){
	// set the initial state of the mapper
	// e000-ffff will always be fixed to the last PRG bank
	
	// init mmc3 mirroring, 1 = horizontal
	*((unsigned char*)0xa000) = 1;
	
	*((unsigned char*)0x8000) = 6;
	// sets c000-dfff to be fixed to the 2nd to last PRG bank
	// and indicates we want to switch a PRG bank into the 8000-9fff slot
	
	*((unsigned char*)0x8001) = 0;
	// puts PRG bank 0 to be at 8000-9fff
	
	*((unsigned char*)0x8000) = 7;
	// indicates we want to switch a PRG bank into the a000-bfff slot
	*((unsigned char*)0x8001) = 5;
	// puts PRG bank 5 to be at a000-bfff
	
	// MMC3 divides CHR-ROM banks into chunks of $400 bytes
	*((unsigned char*)0x8000) = 0;
	// indicates we want to switch 2 ($400 bytes each) CHR banks into PPU 0-7ff
	*((unsigned char*)0x8001) = 0;
	// put CHR banks 0-1 in at 0-7ff (PPU)
	
	
	// now the banks are ordered as such
	// bank 0 8000-9fff
	// bank 5 a000-bfff
	// bank 6 c000-dfff
	// bank 7 e000-ffff
}	
	
	


void main(void){
	All_Off();
	Initialize_MMC3();
	
	PPU_ADDRESS = 0x20;
	PPU_ADDRESS = 0x00;
	UnRLE(NT);	// uncompresses our BG data
	
	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();	// don't turn on screen until in v-blank
	All_On();
		
	while(1){
		while (NMI_flag == 0); // wait till NMI
		
		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();
		if ((Frame_Count & 0x03) == 3){ //bankswitch the CHR = waterfall, every 4 frames
			++CHRbank;
			++CHRbank;
			if (CHRbank > 7) CHRbank = 0;
			*((unsigned char*)0x8000) = 0;
			*((unsigned char*)0x8001) = CHRbank;
		}
		if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
			++PRGbank;
			if (PRGbank > 7) PRGbank = 0;
			*((unsigned char*)0x8000) = 6; // bankswitch a PRG bank into $8000
			*((unsigned char*)0x8001) = PRGbank;
			
			Draw_Bank_Num(); // re-writes the text on the screen
		}
		
		NMI_flag = 0;
	}
}

// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
