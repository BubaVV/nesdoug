;asm funtions that can be called by the c code. Written by Doug Fraker 2015.
;note, the unrle code was provided with NES screen tool. Written by Shiru 2010.

.importzp _joypad1, _joypad1old, _joypad1test, _joypad2, _joypad2old, _joypad2test
.importzp _PPU_ADDRESS_High, _PPU_ADDRESS_Low
.import _BUFFER1, _BUFFER2, _BUFFER3, _BUFFER4, _BUFFER7
.export _Get_Input, _Wait_Vblank, _UnRLE, _Sprite_Zero, _Super_Fast_Write_PPU, _Super_Fast_Write_PPU2

.segment "ZEROPAGE"
RLE_LOW:	.res 1
RLE_HIGH:	.res 1
RLE_TAG:	.res 1
RLE_BYTE:	.res 1

.segment "CODE"


;due to dmc audio playback occasionally messing with input, it is best to read twice and compare them
_Get_Input:
	lda _joypad1
	sta _joypad1old
	lda _joypad2
	sta _joypad2old
	
	ldx #$01	;strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input2:		;get first read, store them as a test
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1test
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2test
	dey
	bne Get_Input2
	
GetInputFixBug:
	ldx #$01	;restrobe strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input3:		;read again, store them as joypads
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2
	dey
	bne Get_Input3
	
CompareInput:
	lda _joypad1
	cmp _joypad1test
	bne :+
	lda _joypad2
	cmp _joypad2test
	bne :+
	rts				;if same, done
	
:	lda _joypad1
	sta _joypad1test
	lda _joypad2
	sta _joypad2test

	jmp GetInputFixBug ;if different, reread
	

	
_Wait_Vblank:
	lda $2002
	bpl _Wait_Vblank
	rts
	
	
_UnRLE:
	tay
	stx <RLE_HIGH
	lda #0
	sta <RLE_LOW

	lda (RLE_LOW),y
	sta <RLE_TAG
	iny
	bne @1
	inc <RLE_HIGH
@1:
	lda (RLE_LOW),y
	iny
	bne @11
	inc <RLE_HIGH
@11:
	cmp <RLE_TAG
	beq @2
	sta $2007
	sta <RLE_BYTE
	bne @1
@2:
	lda (RLE_LOW),y
	beq @4
	iny
	bne @21
	inc <RLE_HIGH
@21:
	tax
	lda <RLE_BYTE
@3:
	sta $2007
	dex
	bne @3
	beq @1
@4:
	rts

	
	
_Sprite_Zero:
Sprite0:
	lda $2002
	and #$40
	bne Sprite0
Sprite0b:
	lda $2002
	and #$40
	beq Sprite0b
	rts
	


_Super_Fast_Write_PPU: ;big unrolled loop to draw to the PPU as fast as possible, 26
	lda _BUFFER1
	sta $2007
	lda _BUFFER1+1
	sta $2007
	lda _BUFFER1+2
	sta $2007
	lda _BUFFER1+3
	sta $2007
	lda _BUFFER1+4
	sta $2007
	lda _BUFFER1+5
	sta $2007
	lda _BUFFER1+6
	sta $2007
	lda _BUFFER1+7
	sta $2007
	lda _BUFFER1+8
	sta $2007
	lda _BUFFER1+9
	sta $2007
	lda _BUFFER1+10
	sta $2007
	lda _BUFFER1+11
	sta $2007
	lda _BUFFER1+12
	sta $2007
	lda _BUFFER1+13
	sta $2007
	lda _BUFFER1+14
	sta $2007
	lda _BUFFER1+15
	sta $2007
	lda _BUFFER1+16
	sta $2007
	lda _BUFFER1+17
	sta $2007
	lda _BUFFER1+18
	sta $2007
	lda _BUFFER1+19
	sta $2007
	lda _BUFFER1+20
	sta $2007
	lda _BUFFER1+21
	sta $2007
	lda _BUFFER1+22
	sta $2007
	lda _BUFFER1+23
	sta $2007
	lda _BUFFER1+24
	sta $2007
	lda _BUFFER1+25
	sta $2007
	lda _PPU_ADDRESS_High
	sta $2006
	inc _PPU_ADDRESS_Low
	lda _PPU_ADDRESS_Low
	sta $2006
	lda _BUFFER2
	sta $2007
	lda _BUFFER2+1
	sta $2007
	lda _BUFFER2+2
	sta $2007
	lda _BUFFER2+3
	sta $2007
	lda _BUFFER2+4
	sta $2007
	lda _BUFFER2+5
	sta $2007
	lda _BUFFER2+6
	sta $2007
	lda _BUFFER2+7
	sta $2007
	lda _BUFFER2+8
	sta $2007
	lda _BUFFER2+9
	sta $2007
	lda _BUFFER2+10
	sta $2007
	lda _BUFFER2+11
	sta $2007
	lda _BUFFER2+12
	sta $2007
	lda _BUFFER2+13
	sta $2007
	lda _BUFFER2+14
	sta $2007
	lda _BUFFER2+15
	sta $2007
	lda _BUFFER2+16
	sta $2007
	lda _BUFFER2+17
	sta $2007
	lda _BUFFER2+18
	sta $2007
	lda _BUFFER2+19
	sta $2007
	lda _BUFFER2+20
	sta $2007
	lda _BUFFER2+21
	sta $2007
	lda _BUFFER2+22
	sta $2007
	lda _BUFFER2+23
	sta $2007
	lda _BUFFER2+24
	sta $2007
	lda _BUFFER2+25
	sta $2007
;moved attrib table
	rts

	
_Super_Fast_Write_PPU2: ;big unrolled loop to draw to the PPU as fast as possible, 26
	lda _BUFFER3
	sta $2007
	lda _BUFFER3+1
	sta $2007
	lda _BUFFER3+2
	sta $2007
	lda _BUFFER3+3
	sta $2007
	lda _BUFFER3+4
	sta $2007
	lda _BUFFER3+5
	sta $2007
	lda _BUFFER3+6
	sta $2007
	lda _BUFFER3+7
	sta $2007
	lda _BUFFER3+8
	sta $2007
	lda _BUFFER3+9
	sta $2007
	lda _BUFFER3+10
	sta $2007
	lda _BUFFER3+11
	sta $2007
	lda _BUFFER3+12
	sta $2007
	lda _BUFFER3+13
	sta $2007
	lda _BUFFER3+14
	sta $2007
	lda _BUFFER3+15
	sta $2007
	lda _BUFFER3+16
	sta $2007
	lda _BUFFER3+17
	sta $2007
	lda _BUFFER3+18
	sta $2007
	lda _BUFFER3+19
	sta $2007
	lda _BUFFER3+20
	sta $2007
	lda _BUFFER3+21
	sta $2007
	lda _BUFFER3+22
	sta $2007
	lda _BUFFER3+23
	sta $2007
	lda _BUFFER3+24
	sta $2007
	lda _BUFFER3+25
	sta $2007
	lda _PPU_ADDRESS_High
	sta $2006
	inc _PPU_ADDRESS_Low
	lda _PPU_ADDRESS_Low
	sta $2006
	lda _BUFFER4
	sta $2007
	lda _BUFFER4+1
	sta $2007
	lda _BUFFER4+2
	sta $2007
	lda _BUFFER4+3
	sta $2007
	lda _BUFFER4+4
	sta $2007
	lda _BUFFER4+5
	sta $2007
	lda _BUFFER4+6
	sta $2007
	lda _BUFFER4+7
	sta $2007
	lda _BUFFER4+8
	sta $2007
	lda _BUFFER4+9
	sta $2007
	lda _BUFFER4+10
	sta $2007
	lda _BUFFER4+11
	sta $2007
	lda _BUFFER4+12
	sta $2007
	lda _BUFFER4+13
	sta $2007
	lda _BUFFER4+14
	sta $2007
	lda _BUFFER4+15
	sta $2007
	lda _BUFFER4+16
	sta $2007
	lda _BUFFER4+17
	sta $2007
	lda _BUFFER4+18
	sta $2007
	lda _BUFFER4+19
	sta $2007
	lda _BUFFER4+20
	sta $2007
	lda _BUFFER4+21
	sta $2007
	lda _BUFFER4+22
	sta $2007
	lda _BUFFER4+23
	sta $2007
	lda _BUFFER4+24
	sta $2007
	lda _BUFFER4+25
	sta $2007

	;attribute table, right
AttribTable:
	lda _PPU_ADDRESS_High
	clc
	adc #3
	sta _PPU_ADDRESS_High
	lda _PPU_ADDRESS_Low ;90+0-1f
	and #$1f
	lsr a
	lsr a
	clc
	adc #$c8 ; skip the top one, = HUD
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7
	stx $2007
	;clc	;the clc are not needed, we know it's clear
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+1
	stx $2007
	;clc
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+2
	stx $2007
	;clc
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+3
	stx $2007
	;clc
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+4
	stx $2007
	;clc
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+5
	stx $2007
	;clc
	adc #8
	ldx _PPU_ADDRESS_High
	stx $2006
	sta $2006
	ldx _BUFFER7+6
	stx $2007
	
	rts
