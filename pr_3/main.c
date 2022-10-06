#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define CONVERT_ERROR 123

#define HIGH_MASK 0xFFC00
#define LOW_MASK 0x3FF

uint32_t utf16_to_int32(uint16_t master, uint16_t slave){
	if ((master >= 0xD800) && (master <= 0xDFFF)){
		//pair (master, slave) --> uint32
		uint32_t res = master - 0xD800;
		res <<= 10;
		res += slave - 0xDC00;
		res += 0x10000;
		return res;
	}
	// master --> uint32
	return (uint32_t)master;
}

void int32_to_utf16(uint32_t ch, uint16_t *master, uint16_t *slave){
	if ((ch >= 0xD800) && (ch <= 0xDFFF)) {
		fprintf(stderr, "Invalid value of ch = %d\n", ch);
		exit(CONVERT_ERROR);
	}
	if (ch <= 0xFFFF){
		// uint32 --> master
		*master = (uint16_t)ch;
		*slave = 0x0;
		return;
	}
	// uint32 --> (master, slave)
	ch -= 0x10000;
	*master = 0xD800 + ((ch & HIGH_MASK) >> 10);
	*slave = 0xDC00 + (ch & LOW_MASK);
	return;
}

uint32_t utf8_to_int32(uint8_t* uft8_seq, int len){
	
}

void int32_to_utf8(uint32_t ch, uint8_t* utf8_seq, int* len){
	if (ch <= 0x7F) {
		*len = 1;
		utf8_seq[0] = ch;
		return;
	}
	if (ch <= 0x7FF){
		*len = 2;
		utf8_seq[1] = (ch & 0x3F) | 0x80;
		utf8_seq[0] = (ch >> 6) | 0xC0;
		return;
	}
	if (ch <= 0xFFFF){
		*len = 3;
		utf8_seq[2] = (ch & 0x3F) | 0x80;
		utf8_seq[1] = ((ch >> 6) & 0x3F) | 0x80;
		utf8_seq[0] = (ch >> 12) | 0xE0;
		return;
	}
	if (ch <= 0x1FFFFF){
		*len = 4;
		utf8_seq[3] = (ch & 0x3F) | 0x80;
		utf8_seq[2] = ((ch >> 6) & 0x3F) | 0x80;
		utf8_seq[1] = ((ch >> 12) & 0x3F) | 0x80;
		utf8_seq[0] = (ch >> 18) | 0xF0;
		return;
	}
	fprintf(stderr, "Can't convert %d to UTF-8 (%d > 0x1FFFFF)", ch, ch);
	exit(CONVERT_ERROR);
}

void utf8_to_utf16(FILE* fin, FILE* fout){


}

#define DEFAULT_BOM 0
void tof16_to_utf8(FILE* fin, FILE* fout){
	uint16_t ch = 0;
	fread(&ch, sizeof(ch), 1, fin);
	int bom = DEFAULT_BOM;
	if (ch == 0xFEFF) bom = 1;
	else if (ch == 0xFFFE) bom = 0;

	int utf8_seq_len;
	uint8_t utf8_seq[4];
	uint32_t i32;
	uint16_t master, slave;

	while (!feof(fin)){
		fread(&ch, sizeof(ch), 1, fin);
		if (bom && ((ch >= 0xDF80) && (ch <= 0xDFFF))){
			master = ch;
			fread(&slave, sizeof(slave), 1, fin);
		}
		else if (!bom && ((ch >= 0xDC00) && (0xDFFF))){
			slave = ch;
			fread(&master, sizeof(master), 1, fin);
		}
		else master = ch;
		i32 = utf16_to_int32(master, slave);
		int32_to_utf8(i32, utf8_seq, &utf8_seq_len);
		fwrite(utf8_seq, sizeof(*utf8_seq), utf8_seq_len, fout);
	}
}

int main(){
	FILE *fin = stdin, *fout = stdout;
	uint16_t master, slave;
	
	int32_to_utf16(0x10000, &master, &slave);
	printf("%x\t%x\n", master, slave);
	int32_to_utf16(0x103FE, &master, &slave);
	printf("%x\t%x\n", master, slave);
	int32_to_utf16(0x103FF, &master, &slave);
	printf("%x\t%x\n", master, slave);
	int32_to_utf16(0x10400, &master, &slave);
	printf("%x\t%x\n", master, slave);
	int32_to_utf16(0x107FE, &master, &slave);
	printf("%x\t%x\n", master, slave);
	int32_to_utf16(0x107FF, &master, &slave);
	printf("%x\t%x\n", master, slave);

}
