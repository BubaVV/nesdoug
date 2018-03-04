;asm funtions that can be called by the c code. Written by Doug Fraker 2015.
;note, the unrle code was provided with NES screen tool. Written by Shiru 2010.

.importzp _joypad1, _joypad1old, _joypad1test, _joypad2, _joypad2old, _joypad2test, _Sprite_Index, _temp, _temp2, _temp3
.importzp _collision, _objectA_x, _objectA_y, _objectA_width, _objectA_height, _objectB_x, _objectB_y, _objectB_width, _objectB_height

.import _SPRITES
.export _Get_Input, _Wait_Vblank, _UnRLE, _Draw_Sprites, _move_bullets, _move_bullets2, _move_enemies, _Hitbox


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

;max size = 64 sprites (of course)

	stx Pointer_High ;x = high byte address
	sta Pointer_Low ;a = low byte address
	
		; this is a pointer to a struct
		; unsigned char anime; // for drawing object to screen
		; unsigned char dir;	// direction, for flipping 0 = L, no flip
		; unsigned char Y;	// top side, if = 0 = off
		; unsigned char X;	// left side
		; unsigned char delay; // wait to start moves
		; unsigned char type;	// type of object
		; unsigned char move;	// which move to do
		; unsigned char count; // how far along on the move are we
		
	
_Draw_Sprites_B:
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
	; ldy #0 ;already zero
	lda (Pointer2_Low), y
	sta Sprite_Count ;first byte = how many sprites to draw
	
	; this part is so 64 sprites can work
	inc Pointer2_Low
	bne :+
	inc Pointer2_High
:
	
;note, I'm not checking for overflow, your game will have to keep the sprite count to 64

	;ldy #0 already is
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop:
	lda (Pointer2_Low), y ;y coordinates
	sta _SPRITES, x 
	inx
	iny
	
	lda (Pointer2_Low), y ;tile
	sta _SPRITES, x 
	inx
	iny
	
	lda (Pointer2_Low), y ;attrib
	sta _SPRITES, x 
	inx
	iny
	
	lda (Pointer2_Low), y ;x coordinates
	sta _SPRITES, x 
	inx
	iny
	dec index
	bne @loop
	
	
;check flipping...

	ldy #1 ; are we flipping ?
	lda (Pointer_Low), y
	beq Skip_Flip

	lda Pointer2_High
	sta Pointer_High ; reusing the other pointer
	ldx Sprite_Count
	lda Times4, x
	bne :+
	inc Pointer_High ; if sprite count 64, times 4 = 0
:
	clc
	adc Pointer2_Low
	sta Pointer_Low
	bcc :+
	inc Pointer_High
:
;; pointer should now point to just past the regular sprite data, to the flipped x values

	ldy #0
	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop2:
	lda _SPRITES+2, x ; attributes
	eor #$40
	sta _SPRITES+2, x ; attributes
	
	lda (Pointer_Low), y ;now points to the x flipped part
	sta _SPRITES+3, x ; x value
	inx
	inx
	inx
	inx
	iny
	dec index
	bne @loop2
	
Skip_Flip:


	ldx _Sprite_Index
	lda Sprite_Count
	sta index
@loop3:

;now add the y value, put off screen if overflow
	lda _SPRITES, x ;y
	clc
	adc _temp ;y
	bcc :+
	lda #$f8
:
	sta _SPRITES, x ;y
	
;now add the x value, put off screen if overflow
	lda _SPRITES+3, x ;x
	clc
	adc _temp2 ;x
	sta _SPRITES+3, x ;x
	bcc :+
	lda #$f8
	sta _SPRITES, x ;y
:
	inx
	inx
	inx
	inx
	dec index
	bne @loop3

	
;add to the index
	ldx Sprite_Count
	lda Times4, x
	clc
	adc _Sprite_Index
	sta _Sprite_Index
	rts
	
Times4:
.byte 0,4,8,12,16,20,24,28,32,36,40
.byte 44,48,52,56,60,64,68,72,76,80
.byte 84,88,92,96,100,104,108,112,116,120
.byte 124,128,132,136,140,144,148,152,156,160
.byte 164,168,172,176,180,184,188,192,196,200
.byte 204,208,212,216,220,224,228,232,236,240
.byte 244,248,252, 0

	




.include "SPRITES.s"





; i rewrote the drawing code, to be faster
; bullets are assumed to be 1 tile, non-flipped


_move_bullets: ;specific to hero bullets 16x8
;	unsigned char Y; // y = 0 = off
;	unsigned char Y_sub;
;	unsigned char tile;
;	unsigned char attrib;
;	unsigned char X;
;	unsigned char X_sub;
;	unsigned char Y_speed; // high nibble = speed, low = sub
;	unsigned char X_speed;
;sent was the address of hero bullet, first process it's moves, then draw it

	jsr Shared_Bullet_Code
	;a = y
	cmp #$f0
	bcc :+
	lda #0
	tay
	sta (Pointer_Low), y ;Y
	rts ;skip drawing if past top of screen
:

;now draw it
	ldx _Sprite_Index
	ldy #0
	lda (Pointer_Low), y ;Y
	sta _SPRITES, x
	sta _SPRITES+4, x
	
	iny
	iny

	lda (Pointer_Low), y ;tile
	sta _SPRITES+1, x
	sta _SPRITES+5, x
	
	iny
	
	;attrib = zero
	lda #0
	sta _SPRITES+2, x
	sta _SPRITES+6, x
	
	iny
	
	lda (Pointer_Low), y ;X
	sta _SPRITES+3, x
	clc
	adc #13
	sta _SPRITES+7, x

	lda _Sprite_Index
	clc
	adc #8
	sta _Sprite_Index
	rts
	
	
	
	

_move_bullets2: ;enemy bullets, all 8x8
	jsr Shared_Bullet_Code
	;a = y
	cmp #$f0
	bcc :+
	lda #0
	tay
	sta (Pointer_Low), y ;Y
	rts ;skip drawing if past top of screen
:

;now x moves
	ldy #4
	lda (Pointer_Low), y ;X
	sta _temp
	iny
	lda (Pointer_Low), y ;X sub
	sta _temp2
	iny
	iny
	lda (Pointer_Low), y ;X speed (shifted)
	sta _temp3
	
	asl a
	asl a
	asl a
	asl a
	
	clc
	adc _temp2 ;x sub
	bcc :+
	inc _temp ;x
:
	ldy #5
	sta (Pointer_Low), y ;Y sub
	
	lda _temp3
	
	lsr a
	lsr a
	lsr a
	lsr a
	
	cmp #$08 ;carry is now set if >= 8
	bcc :+
	ora #$f0 ;for negative numbers
:
	clc
	adc _temp ;x
	ldy #4
	sta (Pointer_Low), y ;X
	
	;a = x
	cmp #$fc ;assume flying off the screen sideways
	bcc :+
	lda #0
	tay
	sta (Pointer_Low), y ;Y
	rts ;skip drawing if past top of screen
:
	

;now draw it
	ldx _Sprite_Index
	ldy #0
	lda (Pointer_Low), y ;Y
	sta _SPRITES, x
	
	iny
	iny

	lda (Pointer_Low), y ;tile
	sta _SPRITES+1, x
	
	iny
	
	;attrib = zero
	lda (Pointer_Low), y ;attrib
	sta _SPRITES+2, x
	
	iny
	
	lda (Pointer_Low), y ;X
	sta _SPRITES+3, x

	lda _Sprite_Index
	clc
	adc #4
	sta _Sprite_Index
	rts
	
	
	
	
	;for all kinds of projectiles, movements
Shared_Bullet_Code:
	stx Pointer_High ;x = high byte address
	sta Pointer_Low ;a = low byte address
	ldy #0
	lda (Pointer_Low), y ;Y
	sta _temp
	iny ;1
	lda (Pointer_Low), y ;Y sub
	sta _temp2
	ldy #6
	lda (Pointer_Low), y ;Y speed (shifted)
	sta _temp3
	
	asl a
	asl a
	asl a
	asl a
	
	clc
	adc _temp2 ;y sub
	bcc :+
	inc _temp ;y
:
	ldy #1
	sta (Pointer_Low), y ;Y sub
	
	lda _temp3 ;Y speed (shifted)
	
	lsr a
	lsr a
	lsr a
	lsr a
	
	cmp #$08 ;carry is now set if >= 8
	bcc :+
	ora #$f0 ;for negative numbers
:
	clc
	adc _temp ;y
	ldy #0
	sta (Pointer_Low), y ;Y
	

	rts
	
	
	

_move_enemies: ;and draw them
;all assumed to be 2x2 block of 4 tiles

;	unsigned char anime; // for drawing object to screen
;	unsigned char dir;	// direction, for flipping 0 = L
;	unsigned char Y;	// top side
;	unsigned char X;	// left side
;	unsigned char delay; // wait to start moves
;	unsigned char type;	// type of object
;	unsigned char move;	// which move to do
;	unsigned char count; // how far along on the move are we
	
	stx Pointer_High ;x = high byte address
	sta Pointer_Low ;a = low byte address
	ldy #2
	lda (Pointer_Low), y ;Y
	clc
	adc #1
	sta (Pointer_Low), y ;Y
	
	cmp #$f0
	bcc :+
	lda #0
	sta (Pointer_Low), y ;Y  ;if off screen, end, don't draw
	rts
:
	jmp _Draw_Sprites_B
	;rts
	
	
	
	
	

_Hitbox:
;generic hitbox code for all sprite vs sprite collisions
;unsigned char _objectA_x;
;unsigned char _objectA_y;
;unsigned char _objectA_width;
;unsigned char _objectA_height;
;unsigned char _objectB_x;
;unsigned char _objectB_y;
;unsigned char _objectB_width;
;unsigned char _objectB_height;
	
	lda #0
	sta _collision
	
;first check if _objectA_x + _objectA_width < _objectB_x
	lda _objectA_x
	clc
	adc _objectA_width
	bcs :+
	cmp _objectB_x
	bcs :+
	rts
:	

;now check if _objectB_x + _objectB_width < _objectA_x
	lda _objectB_x
	clc
	adc _objectB_width
	bcs :+
	cmp _objectA_x
	bcs :+
	rts
:	

;now check if _objectA_y + _objectA_height < _objectB_y
	lda _objectA_y
	clc
	adc _objectA_height
	bcs :+
	cmp _objectB_y
	bcs :+
	rts
:	

;now check if _objectB_y + objectB_height < _objectA_y
	lda _objectB_y
	clc
	adc _objectB_height
	bcs :+
	cmp _objectA_y
	bcs :+
	rts
:
	inc _collision
	rts
;80 cycles per test