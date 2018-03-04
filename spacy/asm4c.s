;asm funtions that can be called by the c code. Written by Doug Fraker 2015.
;note, the unrle code was provided with NES screen tool. Written by Shiru 2010.

.importzp _joypad1, _joypad1old, _joypad1test, _joypad2, _joypad2old, _joypad2test, _Sprite_Index, _temp, _temp2, _temp3
.import _SPRITES
.export _Get_Input, _Wait_Vblank, _UnRLE, _Draw_Sprites


.segment "ZEROPAGE"
RLE_LOW:	.res 1
RLE_HIGH:	.res 1
RLE_TAG:	.res 1
RLE_BYTE:	.res 1
Pointer_Low:	.res 1
Pointer_High:	.res 1
Pointer2_Low:	.res 1
Pointer2_High:	.res 1
Sprite_Count:	.res 1
index:			.res 1


.segment "CODE"

;note... Blank_sprite is over in reset.s


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
	
	

_Draw_Sprites:

;object = struct = {type, direction, Y, X, delay, move, count} 
;this takes in the address of the object, which we use to index
;to a table of addresses of drawing lists (animation frames)
;first element of table = how many sprites object has in this frame

;max size = 50 sprites

	stx Pointer_High ;x = high byte address
	sta Pointer_Low ;a = low byte address
	ldy #2
	lda (Pointer_Low), y ;Y
	sta _temp
	iny ;3
	lda (Pointer_Low), y ;X
	sta _temp2
	
	ldy #0
	lda (Pointer_Low), y ;type
	asl a ;times 2
	tax
	lda Address_Animation, x
	sta Pointer2_Low
	lda Address_Animation+1, x
	sta Pointer2_High
	;ldy #0 ;already zero
	lda (Pointer2_Low), y
	sta Sprite_Count ;first byte = how many sprites to draw
	
;sanity check
	tax
	lda Times4, x
	clc
	adc _Sprite_Index
	bcc :+
	rts ;if overflow, abort, don't draw this object
:
	
	iny ;y = 1
	
	;_temp = Y, _temp2 = X
;y coordinates
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop:
	lda (Pointer2_Low), y ;y coordinates
	clc
	adc _temp
	sta _SPRITES+4, x
	bcc :+
	lda #$f8 ;if overflow, put off screen
	sta _SPRITES+4, x
:
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne @loop
	
;y = 1 + sprite count
;now tiles
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
	
:	lda (Pointer2_Low), y ;next tiles
	sta _SPRITES+4+1, x
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne :-
	
	sty _temp3
	
	ldy #1
	lda (Pointer_Low), y ;direction 0=L=no flip
	beq _Draw_Sprites_L
	jmp _Draw_Sprites_R
	
_Draw_Sprites_L:
	ldy _temp3
	;y = 1 + sprite count + sprite count
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
	
:	lda (Pointer2_Low), y ;first attributes
	sta _SPRITES+4+2, x
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne :-
	
	
	
;x coordinates
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop2:
	lda (Pointer2_Low), y ;next X coordinates, assumed to be symmetrical
	clc
	adc _temp2
	bcc :+
	lda #$f8 ;if overflow, put off screen
	sta _SPRITES+4, x
:
	sta _SPRITES+4+3, x
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne @loop2	
	
	jmp _Draw_Sprites_Both
	
	

	
_Draw_Sprites_R: ;flipped version
	ldy _temp3
	;y = 1 + sprite count + sprite count
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
	
:	lda (Pointer2_Low), y
	eor #$40 ;flip horizontal
	sta _SPRITES+4+2, x
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne :-
	
	
	tya 
	clc
	adc Sprite_Count
	tay
	

	;x coordinates, flipped
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop2:
	lda (Pointer2_Low), y ;next X coordinates, flipped
	clc
	adc _temp2
	bcc :+
	lda #$f8 ;if overflow, put off screen
	sta _SPRITES+4, x
:
	sta _SPRITES+4+3, x
	iny
	inx
	inx
	inx
	inx
	
	dec index
	bne @loop2	
	

_Draw_Sprites_Both:


	
;add to the index
	ldx Sprite_Count
	lda Times4, x
	clc
	adc _Sprite_Index
	rts
	
Times4:
.byte 0,4,8,12,16,20,24,28,32,36,40

	
Address_Animation:
.word Ship_Reg, Ship_Left, Ship_Right, Pause_Data, Game_Over_Data, Victory_Data, $0000

;number of tiles
;all y coordinates, relative
;all tiles
;all attributes
;all x coordinates, relative
;all x flipped, relative

Ship_Reg:
.byte 6
.byte 0, 0, 8, 8, $10, $10  ;all y coordinates, relative
.byte 0, 0, $10, $10, $20, $20 ;all tiles
.byte 0, $40, 0, $40, 0, $40 ;all attributes
.byte 0, 8, 0,8,0,8 ;all x coordinates, relative
;these don't have flipped states, omitted

Ship_Left:
.byte 6
.byte 0, 0, 8, 8, $10, $10  ;all y coordinates, relative
.byte 1, 2, $11, $12, $21, $22 ;all tiles
.byte 0, 0, 0, 0, 0, 0 ;all attributes
.byte 0, 8, 0,8,0,8 ;all x coordinates, relative
;these don't have flipped states, omitted

Ship_Right:
.byte 6
.byte 0, 0, 8, 8, $10, $10  ;all y coordinates, relative
.byte 2, 1, $12, $11, $22, $21 ;all tiles
.byte $40, $40, $40, $40, $40, $40 ;all attributes
.byte 0, 8, 0,8,0,8 ;all x coordinates, relative
;these don't have flipped states, omitted

Pause_Data:
.byte 5
.byte 0, 0, 0, 0, 0  ;all y coordinates, relative
.byte $f0, $f1, $f2, $f3, $f4 ;all tiles
.byte 0, 0, 0, 0, 0 ;all attributes
.byte 0, 8, $10, $18, $20 ;all x coordinates, relative
;these don't have flipped states, omitted

Game_Over_Data:
.byte 8
.byte 0, 0, 0, 0, 0, 0, 0, 0  ;all y coordinates, relative
.byte $f5, $f1, $f6, $f4, $f7, $f8, $f4, $f9 ;all tiles
.byte 0, 0, 0, 0, 0,0,0,0 ;all attributes
.byte 0, 8, $10, $18, $28, $30, $38, $40 ;all x coordinates, relative
;these don't have flipped states, omitted

Victory_Data:
.byte 8
.byte 0, 0, 0, 0, 0, 0, 0, 0 ;all y coordinates, relative
.byte $fa, $fb, $fc, $fd, $fe, $f9, $ee, $ef ;all tiles
.byte 0, 0, 0, 0, 0,0,0,0 ;all attributes
.byte 0, 8, $10, $18, $20, $28, $30, $38 ;all x coordinates, relative
;these don't have flipped states, omitted



