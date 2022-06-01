
        DEVICE NOSLOT64K
        OUTPUT "EXAMPLE.BIN"
		
		DEFINE BITBUSTER_OPTIMIZE_SPEED

CHGMOD:	equ $5F
CHGET:	equ $9F
LDIRVM: equ $5C
BAKCLR: equ $F3EA
BDRCLR: equ $F3EB

        org $8100 - 7

header: db $fe
        dw start, end, entry
		
start:
entry:
		xor a
		ld (BAKCLR),a
		ld (BDRCLR),a
		ld a,8
		call CHGMOD	; switch to screen8 with black border/background color
		
		ld hl,gfx	; address of compressed gfx
		ld de,0		; address to load to in VRAM
		xor a		; start decompressing new data
.depack_loop:
		push de		; push VRAM address

		ld de,$9000	; decompress to this address
		call bitbuster.depack
		
		pop de		; pop VRAM address
		
		push af		; push blocks remaining status
		push hl		; push pointer to next compressed block
		push de		; push VRAM address
		push bc		; push length of decompressed data
		
		ld hl,$9000	; address of decompressed data
		call LDIRVM	; transfer to VRAM

		pop bc		; pop length of decompressed data
		pop hl		; pop VRAM address
		add hl,bc	; calculate VRAM address for next block
		ex de,hl	; and put it in DE
		pop hl		; pop pointer to next compressed block
		pop af		; pop blocks remaining status

		jr nz,.depack_loop	; loop while more blocks remaining
		
		call CHGET	; wait for some key press
		ret			; back to BASIC
		
		
		include "../bitbuster.asm"

gfx:
		incbin "MSX.PCK"
		
end: