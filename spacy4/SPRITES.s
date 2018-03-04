; ------------------ ;
; all sprite objects ;
; ------------------ ;



Address_Animation:
.word Ship_Reg, Ship_Left, Ship_Right, Ship_Exp_1, Ship_Exp_2, Ship_Exp_3
.word Enemy1_Data, Enemy2_Data, Enemy3_Data, Enemy4_Data, Enemy5_Data, Enemy6_Data, Enemy7_Data
.word Enemy_Explode, Pause_Data, Game_Over_Data, Victory_Data, Boss, Boss2, Laser




; each animation recipe goes like this...
; number of sprites
; y, tile, attrib, x
; (repeated, 4 bytes per tile)
; then a list of all the flipped x's, if flipped available


Ship_Reg:

.byte $06

.byte $00, $00, $00, $00
.byte $00, $00, $40, $08
.byte $08, $10, $00, $00
.byte $08, $10, $40, $08
.byte $10, $20, $00, $00
.byte $10, $20, $40, $08


.byte 8,0,8,0,8,0			;x flipped values
.byte $10,$10,$08,$08,0,0 	;y flipped values
; left in for testing the code



Ship_Left:
.byte $06
.byte $00, $01, $00, $00
.byte $00, $02, $00, $08
.byte $08, $11, $00, $00
.byte $08, $12, $00, $08
.byte $10, $21, $00, $00
.byte $10, $22, $00, $08
;these don't have flipped states, omitted




Ship_Right:
.byte $06
.byte $00, $02, $40, $00
.byte $00, $01, $40, $08
.byte $08, $12, $40, $00
.byte $08, $11, $40, $08
.byte $10, $22, $40, $00
.byte $10, $21, $40, $08
;these don't have flipped states, omitted




Ship_Exp_1: ;hero explosion
.byte $6
.byte $0, $30, $0, $0
.byte $0, $31, $0, $8
.byte $8, $40, $0, $0
.byte $8, $41, $0, $8
.byte $10, $50, $0, $0
.byte $10, $51, $0, $8
;these don't have flipped states, omitted


Ship_Exp_2:
.byte $9
.byte $0, $32, $0, $fc
.byte $0, $33, $0, $04
.byte $0, $34, $0, $0c
.byte $8, $42, $0, $fc
.byte $8, $43, $0, $04
.byte $8, $44, $0, $0c
.byte $10, $52, $0, $fc
.byte $10, $53, $0, $04
.byte $10, $54, $0, $0c
;these don't have flipped states, omitted

Ship_Exp_3:
.byte $9
.byte $0, $35, $0, $fc
.byte $0, $36, $0, $4
.byte $0, $37, $0, $0c
.byte $8, $45, $0, $fc
.byte $8, $46, $0, $4
.byte $8, $47, $0, $0c
.byte $10, $55, $0, $fc
.byte $10, $56, $0, $4
.byte $10, $57, $0, $0c
;these don't have flipped states, omitted

Enemy1_Data: ;saucer plain
.byte $4
.byte $0, $3, $3, $0
.byte $0, $4, $3, $8
.byte $8, $13, $3, $0
.byte $8, $14, $3, $8
.byte 8,0,8,0 ;flipped x

Enemy2_Data: ;saucer angled
.byte $4
.byte $0, $5, $3, $0
.byte $0, $6, $3, $8
.byte $8, $15, $3, $0
.byte $8, $16, $3, $8
.byte 8,0,8,0 ;flipped x

Enemy3_Data: ;fly down
.byte $4
.byte $0, $7, $2, $0
.byte $0, $8, $2, $8
.byte $8, $17, $2, $0
.byte $8, $18, $2, $8
.byte 8,0,8,0 ;flipped x

Enemy4_Data: ;fly down left
.byte $4
.byte $0, $9, $2, $0
.byte $0, $a, $2, $8
.byte $8, $19, $2, $0
.byte $8, $1a, $2, $8
.byte 8,0,8,0 ;flipped x

Enemy5_Data: ;fly left
.byte $4
.byte $0, $b, $2, $0
.byte $0, $c, $2, $8
.byte $8, $1b, $2, $0
.byte $8, $1c, $2, $8
.byte 8,0,8,0 ;flipped x

Enemy6_Data: ;fly up right

.byte $4
.byte $0, $19, $82, $0
.byte $0, $1a, $82, $8
.byte $8, $9, $82, $0
.byte $8, $a, $82, $8
.byte 8,0,8,0 ;flipped x

Enemy7_Data: ;fly up

.byte $4
.byte $0, $17, $82, $0
.byte $0, $18, $82, $8
.byte $8, $7, $82, $0
.byte $8, $8, $82, $8
.byte 8,0,8,0 ;flipped x


Enemy_Explode:
.byte $4
.byte $0, $e, $1, $0
.byte $0, $f, $1, $8
.byte $8, $1e, $1, $0
.byte $8, $1f, $1, $8
.byte 8,0,8,0 ;flipped x
;shouldn't be flipped, but I added it anyway


Pause_Data:
.byte $5
.byte $0, $f0, $0, $0
.byte $0, $f1, $0, $8
.byte $0, $f2, $0, $10
.byte $0, $f3, $0, $18
.byte $0, $f4, $0, $20
;these don't have flipped states, omitted


Game_Over_Data:
.byte $8
.byte $0, $f5, $0, $0
.byte $0, $f1, $0, $8
.byte $0, $f6, $0, $10
.byte $0, $f4, $0, $18
.byte $0, $f7, $0, $28
.byte $0, $f8, $0, $30
.byte $0, $f4, $0, $38
.byte $0, $f9, $0, $40
;these don't have flipped states, omitted


Victory_Data:
.byte $8
.byte $0, $fa, $0, $0
.byte $0, $fb, $0, $8
.byte $0, $fc, $0, $10
.byte $0, $fd, $0, $18
.byte $0, $fe, $0, $20
.byte $0, $f9, $0, $28
.byte $0, $ee, $0, $30
.byte $0, $ef, $0, $38
;these don't have flipped states, omitted


Boss:
.byte 30 ;number of sprites

.byte $0, $90, $2, $0
.byte $0, $91, $2, $8
.byte $0, $92, $2, $10
.byte $0, $92, $42, $18
.byte $0, $91, $42, $20
.byte $0, $90, $42, $28
.byte $8, $a0, $2, $0
.byte $8, $a1, $2, $8
.byte $8, $a2, $2, $10
.byte $8, $a2, $42, $18
.byte $8, $a1, $42, $20
.byte $8, $a0, $42, $28
.byte $10, $b0, $2, $0
.byte $10, $b1, $2, $8
.byte $10, $b2, $2, $10
.byte $10, $b2, $42, $18
.byte $10, $b1, $42, $20
.byte $10, $b0, $42, $28
.byte $18, $c0, $2, $0
.byte $18, $c1, $2, $8
.byte $18, $c2, $2, $10
.byte $18, $c2, $42, $18
.byte $18, $c1, $42, $20
.byte $18, $c0, $42, $28
.byte $20, $d0, $2, $0
.byte $20, $d1, $2, $8
.byte $20, $d2, $2, $10
.byte $20, $d2, $42, $18
.byte $20, $d1, $42, $20
.byte $20, $d0, $42, $28

Boss2:
.byte 30 ;number of sprites

.byte $0, $93, $2, $0
.byte $0, $94, $2, $8
.byte $0, $95, $2, $10
.byte $0, $95, $42, $18
.byte $0, $94, $42, $20
.byte $0, $93, $42, $28
.byte $8, $a3, $2, $0
.byte $8, $a4, $2, $8
.byte $8, $a5, $2, $10
.byte $8, $a5, $42, $18
.byte $8, $a4, $42, $20
.byte $8, $a3, $42, $28
.byte $10, $b3, $2, $0
.byte $10, $b4, $2, $8
.byte $10, $b5, $2, $10
.byte $10, $b5, $42, $18
.byte $10, $b4, $42, $20
.byte $10, $b3, $42, $28
.byte $18, $c3, $2, $0
.byte $18, $c4, $2, $8
.byte $18, $c5, $2, $10
.byte $18, $c5, $42, $18
.byte $18, $c4, $42, $20
.byte $18, $c3, $42, $28
.byte $20, $d3, $2, $0
.byte $20, $d4, $2, $8
.byte $20, $d5, $2, $10
.byte $20, $d5, $42, $18
.byte $20, $d4, $42, $20
.byte $20, $d3, $42, $28


Laser:
.byte 2  ;number of sprites

.byte $0, $3f, $1, $0
.byte $8, $4f, $1, $0

