#ifndef MICROKNIGHT_H
#define MICROKNIGHT_H

// Microknight credits to Niels Krogh Mortensen A.K.A. N�lb / Grafictive (according to http://www.pouet.net/topic.php?which=9097)
const unsigned char microknight[128-32][8] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* $20   */
	{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00}, /* $21 ! */
	{ 0x6C, 0x6C, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00}, /* $22 " */
	{ 0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00}, /* $23 # */
	{ 0x10, 0x7C, 0xD0, 0x7C, 0x16, 0x16, 0x7C, 0x10}, /* $24 $ */
	{ 0x60, 0x96, 0x7C, 0x18, 0x30, 0x6C, 0xD2, 0x0C}, /* $25 % */
	{ 0x70, 0xD8, 0x70, 0xF6, 0xDC, 0xD8, 0x7C, 0x06}, /* $26 & */
	{ 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00}, /* $27 ' */
	{ 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00}, /* $28 ( */
	{ 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00}, /* $29 ) */
	{ 0x00, 0x6C, 0x38, 0xFE, 0x38, 0x6C, 0x00, 0x00}, /* $2A * */
	{ 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00}, /* $2B + */
	{ 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x10}, /* $2C , */
	{ 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00}, /* $2D - */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, /* $2E . */
	{ 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x00}, /* $2F / */
	{ 0x00, 0x78, 0xCC, 0xDE, 0xF6, 0xE6, 0x7C, 0x00}, /* $30 0 */
	{ 0x18, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00}, /* $31 1 */
	{ 0x7C, 0x06, 0x3C, 0x60, 0xC0, 0xC0, 0xFE, 0x00}, /* $32 2 */
	{ 0x3C, 0x06, 0x1C, 0x06, 0x46, 0xC6, 0x7C, 0x00}, /* $33 3 */
	{ 0x18, 0x18, 0x30, 0x6C, 0xCC, 0xFE, 0x0C, 0x00}, /* $34 4 */
	{ 0xF8, 0xC0, 0xFC, 0x06, 0x46, 0xCC, 0x78, 0x00}, /* $35 5 */
	{ 0x70, 0xC0, 0xFC, 0xC6, 0xC6, 0xCC, 0x78, 0x00}, /* $36 6 */
	{ 0xFE, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00}, /* $37 7 */
	{ 0x78, 0xCC, 0x7C, 0xC6, 0xC6, 0xCC, 0x78, 0x00}, /* $38 8 */
	{ 0x78, 0xCC, 0xC6, 0xC6, 0x7E, 0x06, 0x1C, 0x00}, /* $39 9 */
	{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00}, /* $3A : */
	{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x08, 0x10}, /* $3B ; */
	{ 0x00, 0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0x00}, /* $3C < */
	{ 0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0x00, 0x00}, /* $3D = */
	{ 0x00, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x00, 0x00}, /* $3E > */
	{ 0x7C, 0xC6, 0x06, 0x3C, 0x30, 0x00, 0x30, 0x00}, /* $3F ? */
	{ 0x38, 0x6C, 0xDE, 0xF6, 0xDE, 0xC0, 0x66, 0x3C}, /* $40 @ */
	{ 0x78, 0xCC, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00}, /* $41 A */
	{ 0xF8, 0xCC, 0xFC, 0xC6, 0xC6, 0xCC, 0xF8, 0x00}, /* $42 B */
	{ 0x78, 0xCC, 0xC0, 0xC0, 0xC0, 0xC6, 0x7C, 0x00}, /* $43 C */
	{ 0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xC6, 0xFC, 0x00}, /* $44 D */
	{ 0xFE, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0xFE, 0x00}, /* $45 E */
	{ 0xFE, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0xC0, 0x00}, /* $46 F */
	{ 0x38, 0x60, 0xC0, 0xCE, 0xC6, 0xC6, 0x7E, 0x06}, /* $47 G */
	{ 0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00}, /* $48 H */
	{ 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, /* $49 I */
	{ 0x0E, 0x06, 0x06, 0x06, 0xC6, 0xC6, 0x7C, 0x00}, /* $4A J */
	{ 0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00}, /* $4B K */
	{ 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0x00}, /* $4C L */
	{ 0xC6, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0x00}, /* $4D M */
	{ 0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00}, /* $4E N */
	{ 0x78, 0xCC, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00}, /* $4F O */
	{ 0xF8, 0xCC, 0xC6, 0xC6, 0xFC, 0xC0, 0xC0, 0x00}, /* $50 P */
	{ 0x78, 0xCC, 0xC6, 0xC6, 0xC6, 0xD6, 0x7C, 0x0C}, /* $51 Q */
	{ 0xF8, 0xCC, 0xC6, 0xC6, 0xFC, 0xD8, 0xCC, 0x06}, /* $52 R */
	{ 0x78, 0xC0, 0x7C, 0x06, 0x46, 0xC6, 0x7C, 0x00}, /* $53 S */
	{ 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, /* $54 T */
	{ 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00}, /* $55 U */
	{ 0xC6, 0xC6, 0xC6, 0x6C, 0x6C, 0x38, 0x38, 0x00}, /* $56 V */
	{ 0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00}, /* $57 W */
	{ 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0xC6, 0x00}, /* $58 X */
	{ 0xC6, 0xC6, 0xC6, 0x7C, 0x0C, 0x0C, 0x0C, 0x00}, /* $59 Y */
	{ 0xFE, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xFE, 0x00}, /* $5A Z */
	{ 0x38, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x00}, /* $5B [ */
	{ 0x00, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00}, /* $5C \ */
	{ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 0x00}, /* $5D ] */
	{ 0x10, 0x38, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00}, /* $5E ^ */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE}, /* $5F _ */
	{ 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00}, /* $60 ` */
	{ 0x00, 0x3C, 0x06, 0x7E, 0xC6, 0xC6, 0x7E, 0x00}, /* $61 a */
	{ 0xC0, 0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xFC, 0x00}, /* $62 b */
	{ 0x00, 0x78, 0xCC, 0xC0, 0xC0, 0xC6, 0x7C, 0x00}, /* $63 c */
	{ 0x06, 0x3E, 0x66, 0xC6, 0xC6, 0xC6, 0x7E, 0x00}, /* $64 d */
	{ 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0xC6, 0x7C, 0x00}, /* $65 e */
	{ 0x38, 0x6C, 0x60, 0x78, 0x60, 0x60, 0x60, 0x60}, /* $66 f */
	{ 0x00, 0x7E, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x7C}, /* $67 g */
	{ 0xC0, 0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00}, /* $68 h */
	{ 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00}, /* $69 i */
	{ 0x0C, 0x00, 0x1C, 0x0C, 0x0C, 0x0C, 0x4C, 0x38}, /* $6A j */
	{ 0xC0, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00}, /* $6B k */
	{ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, /* $6C l */
	{ 0x00, 0xC4, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00}, /* $6D m */
	{ 0x00, 0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00}, /* $6E n */
	{ 0x00, 0x78, 0xCC, 0xC6, 0xC6, 0xC6, 0x7C, 0x00}, /* $6F o */
	{ 0x00, 0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xFC, 0xC0}, /* $70 p */
	{ 0x00, 0x3E, 0x66, 0xC6, 0xC6, 0xC6, 0x7E, 0x06}, /* $71 q */
	{ 0x00, 0xFC, 0xC6, 0xC0, 0xC0, 0xC0, 0xC0, 0x00}, /* $72 r */
	{ 0x00, 0x78, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00}, /* $73 s */
	{ 0x30, 0x7C, 0x30, 0x30, 0x30, 0x32, 0x1C, 0x00}, /* $74 t */
	{ 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00}, /* $75 u */
	{ 0x00, 0xC6, 0xC6, 0x6C, 0x6C, 0x38, 0x38, 0x00}, /* $76 v */
	{ 0x00, 0xC6, 0xD6, 0xFE, 0x7C, 0x6C, 0x44, 0x00}, /* $77 w */
	{ 0x00, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00}, /* $78 x */
	{ 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x7C}, /* $79 y */
	{ 0x00, 0xFE, 0x0C, 0x18, 0x30, 0x60, 0xFE, 0x00}, /* $7A z */
	{ 0x0C, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0C, 0x00}, /* $7B { */
	{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, /* $7C | */
	{ 0x30, 0x18, 0x18, 0x0C, 0x18, 0x18, 0x30, 0x00}, /* $7D } */
	{ 0x00, 0x00, 0x10, 0x38, 0x7C, 0xFE, 0x00, 0x00}, /* $7E ~ */
	{ 0x00, 0x00, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x00}, /* $7F  */
};
#endif
