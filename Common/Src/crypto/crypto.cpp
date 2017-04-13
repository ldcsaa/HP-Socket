#include "stdafx.h"
#include "crypto.h"

/****************************** MACROS ******************************/

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

// -------------------------------------------------- BASE64 -------------------------------------------------- //

/****************************** MACROS ******************************/
#define NEWLINE_INVL 76

/**************************** VARIABLES *****************************/
// Note: To change the charset to a URL encoding, replace the '+' and '/' with '*' and '-'
static const BYTE charset[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

/*********************** FUNCTION DEFINITIONS ***********************/
BYTE revchar(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		ch -= 'A';
	else if (ch >= 'a' && ch <='z')
		ch = ch - 'a' + 26;
	else if (ch >= '0' && ch <='9')
		ch = ch - '0' + 52;
	else if (ch == '+')
		ch = 62;
	else if (ch == '/')
		ch = 63;

	return(ch);
}

size_t base64_encode(const BYTE in[], BYTE out[], size_t len, int newline_flag)
{
	size_t idx, idx2, blks, blk_ceiling, left_over, newline_count = 0;

	blks = (len / 3);
	left_over = len % 3;

	if (out == nullptr) {
		idx2 = blks * 4 ;
		if (left_over)
			idx2 += 4;
		if (newline_flag)
			idx2 += len / 57;   // (NEWLINE_INVL / 4) * 3 = 57. One newline per 57 input bytes.
	}
	else {
		// Since 3 input bytes = 4 output bytes, determine out how many even sets of
		// 3 bytes the input has.
		blk_ceiling = blks * 3;
		for (idx = 0, idx2 = 0; idx < blk_ceiling; idx += 3, idx2 += 4) {
			out[idx2]     = charset[in[idx] >> 2];
			out[idx2 + 1] = charset[((in[idx] & 0x03) << 4) | (in[idx + 1] >> 4)];
			out[idx2 + 2] = charset[((in[idx + 1] & 0x0f) << 2) | (in[idx + 2] >> 6)];
			out[idx2 + 3] = charset[in[idx + 2] & 0x3F];
			// The offical standard requires a newline every 76 characters.
			// (Eg, first newline is character 77 of the output.)
			if (((idx2 - newline_count + 4) % NEWLINE_INVL == 0) && newline_flag) {
				out[idx2 + 4] = '\n';
				idx2++;
				newline_count++;
			}
		}

		if (left_over == 1) {
			out[idx2]     = charset[in[idx] >> 2];
			out[idx2 + 1] = charset[(in[idx] & 0x03) << 4];
			out[idx2 + 2] = '=';
			out[idx2 + 3] = '=';
			idx2 += 4;
		}
		else if (left_over == 2) {
			out[idx2]     = charset[in[idx] >> 2];
			out[idx2 + 1] = charset[((in[idx] & 0x03) << 4) | (in[idx + 1] >> 4)];
			out[idx2 + 2] = charset[(in[idx + 1] & 0x0F) << 2];
			out[idx2 + 3] = '=';
			idx2 += 4;
		}
	}

	return(idx2);
}

size_t base64_decode(const BYTE in[], BYTE out[], size_t len)
{
	size_t idx, idx2, blks, blk_ceiling, left_over;

	if (in[len - 1] == '=')
		len--;
	if (in[len - 1] == '=')
		len--;

	blks = len / 4;
	left_over = len % 4;

	if (out == nullptr) {
		if (len >= 77 && in[NEWLINE_INVL] == '\n')   // Verify that newlines where used.
			len -= len / (NEWLINE_INVL + 1);
		blks = len / 4;
		left_over = len % 4;

		idx = blks * 3;
		if (left_over == 2)
			idx ++;
		else if (left_over == 3)
			idx += 2;
	}
	else {
		blk_ceiling = blks * 4;
		for (idx = 0, idx2 = 0; idx2 < blk_ceiling; idx += 3, idx2 += 4) {
			if (in[idx2] == '\n')
				idx2++;
			out[idx]     = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
			out[idx + 1] = (revchar(in[idx2 + 1]) << 4) | (revchar(in[idx2 + 2]) >> 2);
			out[idx + 2] = (revchar(in[idx2 + 2]) << 6) | revchar(in[idx2 + 3]);
		}

		if (left_over == 2) {
			out[idx]     = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
			idx++;
		}
		else if (left_over == 3) {
			out[idx]     = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
			out[idx + 1] = (revchar(in[idx2 + 1]) << 4) | (revchar(in[idx2 + 2]) >> 2);
			idx += 2;
		}
	}

	return(idx);
}

// -------------------------------------------------- URL -------------------------------------------------- //

#define HEX_CHAR_TO_VALUE(c)			(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
#define HEX_DOUBLE_CHAR_TO_VALUE(pc)	(((HEX_CHAR_TO_VALUE(*(pc))) << 4) | (HEX_CHAR_TO_VALUE(*(pc + 1))))
#define HEX_VALUE_TO_CHAR(n)			(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
#define HEX_VALUE_TO_DOUBLE_CHAR(pc, n)	{*(pc) = HEX_VALUE_TO_CHAR((n >> 4)); *((pc) + 1) = HEX_VALUE_TO_CHAR((n & 0X0F));}

int url_encode(const char* src, const int src_size, char* dest, const int dest_size)
{
	if(src == nullptr || dest == nullptr || src_size <= 0 || dest_size <= 0)
		return 0;

	char ch;
	int j = 0;

	for(int i = 0; (i < src_size) && (j < dest_size); ++i)
	{
		ch = src[i];

		if (((ch>='A') && (ch<'Z')) ||
			((ch>='a') && (ch<'z')) ||
			((ch>='0') && (ch<'9')) ||
			(ch == '.' || ch == '-' || ch == '_' || ch == '*'))
			dest[j++] = ch;
		else if(ch == ' ')
			dest[j++] = '+';
		else {
			if(j + 3 < dest_size)
			{
				dest[j++] = '%';
				HEX_VALUE_TO_DOUBLE_CHAR(dest + j, ch);
				j += 2;
			}
			else
				return 0;
		}
	}

	dest[j] = '\0';
	return j;
}

int url_decode(const char* src, const int src_size, char* dest, const int dest_size)
{
	if(src == nullptr || dest == nullptr || src_size <= 0 || dest_size <= 0)
		return 0;

	char ch;
	int j = 0;

	for(int i = 0; i < src_size && j < dest_size; ++i)
	{
		ch = src[i];

		if(ch == '+')
			dest[j++] = ' ';
		else if(ch == '%')
		{
			if(i + 2 < src_size)
			{
				dest[j++] = HEX_DOUBLE_CHAR_TO_VALUE(src + i + 1);
				i += 2;
			}
		}
		else
			dest[j++] = ch;
	}

	dest[j] = 0;
	return j;
}

// -------------------------------------------------- AES -------------------------------------------------- //

/****************************** MACROS ******************************/
// The least significant byte of the word is rotated to the end.
#define KE_ROTWORD(x) (((x) << 8) | ((x) >> 24))

/**************************** DATA TYPES ****************************/
#define AES_128_ROUNDS 10
#define AES_192_ROUNDS 12
#define AES_256_ROUNDS 14

/*********************** FUNCTION DECLARATIONS **********************/
void ccm_prepare_first_ctr_blk(BYTE counter[], const BYTE nonce[], int nonce_len, int payload_len_store_size);
void ccm_prepare_first_format_blk(BYTE buf[], int assoc_len, int payload_len, int payload_len_store_size, int mac_len, const BYTE nonce[], int nonce_len);
void ccm_format_assoc_data(BYTE buf[], int *end_of_buf, const BYTE assoc[], int assoc_len);
void ccm_format_payload_data(BYTE buf[], int *end_of_buf, const BYTE payload[], int payload_len);

/**************************** VARIABLES *****************************/
// This is the specified AES SBox. To look up a substitution value, put the first
// nibble in the first index (row) and the second nibble in the second index (column).
static const BYTE aes_sbox[16][16] = {
	{0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76},
	{0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0},
	{0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15},
	{0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75},
	{0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84},
	{0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF},
	{0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8},
	{0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2},
	{0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73},
	{0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB},
	{0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79},
	{0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08},
	{0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A},
	{0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E},
	{0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF},
	{0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16}
};

static const BYTE aes_invsbox[16][16] = {
	{0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB},
	{0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB},
	{0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E},
	{0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25},
	{0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92},
	{0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84},
	{0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06},
	{0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B},
	{0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73},
	{0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E},
	{0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B},
	{0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4},
	{0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F},
	{0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF},
	{0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61},
	{0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D}
};

// This table stores pre-calculated values for all possible GF(2^8) calculations.This
// table is only used by the (Inv)MixColumns steps.
// USAGE: The second index (column) is the coefficient of multiplication. Only 7 different
// coefficients are used: 0x01, 0x02, 0x03, 0x09, 0x0b, 0x0d, 0x0e, but multiplication by
// 1 is negligible leaving only 6 coefficients. Each column of the table is devoted to one
// of these coefficients, in the ascending order of value, from values 0x00 to 0xFF.
static const BYTE gf_mul[256][6] = {
	{0x00,0x00,0x00,0x00,0x00,0x00},{0x02,0x03,0x09,0x0b,0x0d,0x0e},
	{0x04,0x06,0x12,0x16,0x1a,0x1c},{0x06,0x05,0x1b,0x1d,0x17,0x12},
	{0x08,0x0c,0x24,0x2c,0x34,0x38},{0x0a,0x0f,0x2d,0x27,0x39,0x36},
	{0x0c,0x0a,0x36,0x3a,0x2e,0x24},{0x0e,0x09,0x3f,0x31,0x23,0x2a},
	{0x10,0x18,0x48,0x58,0x68,0x70},{0x12,0x1b,0x41,0x53,0x65,0x7e},
	{0x14,0x1e,0x5a,0x4e,0x72,0x6c},{0x16,0x1d,0x53,0x45,0x7f,0x62},
	{0x18,0x14,0x6c,0x74,0x5c,0x48},{0x1a,0x17,0x65,0x7f,0x51,0x46},
	{0x1c,0x12,0x7e,0x62,0x46,0x54},{0x1e,0x11,0x77,0x69,0x4b,0x5a},
	{0x20,0x30,0x90,0xb0,0xd0,0xe0},{0x22,0x33,0x99,0xbb,0xdd,0xee},
	{0x24,0x36,0x82,0xa6,0xca,0xfc},{0x26,0x35,0x8b,0xad,0xc7,0xf2},
	{0x28,0x3c,0xb4,0x9c,0xe4,0xd8},{0x2a,0x3f,0xbd,0x97,0xe9,0xd6},
	{0x2c,0x3a,0xa6,0x8a,0xfe,0xc4},{0x2e,0x39,0xaf,0x81,0xf3,0xca},
	{0x30,0x28,0xd8,0xe8,0xb8,0x90},{0x32,0x2b,0xd1,0xe3,0xb5,0x9e},
	{0x34,0x2e,0xca,0xfe,0xa2,0x8c},{0x36,0x2d,0xc3,0xf5,0xaf,0x82},
	{0x38,0x24,0xfc,0xc4,0x8c,0xa8},{0x3a,0x27,0xf5,0xcf,0x81,0xa6},
	{0x3c,0x22,0xee,0xd2,0x96,0xb4},{0x3e,0x21,0xe7,0xd9,0x9b,0xba},
	{0x40,0x60,0x3b,0x7b,0xbb,0xdb},{0x42,0x63,0x32,0x70,0xb6,0xd5},
	{0x44,0x66,0x29,0x6d,0xa1,0xc7},{0x46,0x65,0x20,0x66,0xac,0xc9},
	{0x48,0x6c,0x1f,0x57,0x8f,0xe3},{0x4a,0x6f,0x16,0x5c,0x82,0xed},
	{0x4c,0x6a,0x0d,0x41,0x95,0xff},{0x4e,0x69,0x04,0x4a,0x98,0xf1},
	{0x50,0x78,0x73,0x23,0xd3,0xab},{0x52,0x7b,0x7a,0x28,0xde,0xa5},
	{0x54,0x7e,0x61,0x35,0xc9,0xb7},{0x56,0x7d,0x68,0x3e,0xc4,0xb9},
	{0x58,0x74,0x57,0x0f,0xe7,0x93},{0x5a,0x77,0x5e,0x04,0xea,0x9d},
	{0x5c,0x72,0x45,0x19,0xfd,0x8f},{0x5e,0x71,0x4c,0x12,0xf0,0x81},
	{0x60,0x50,0xab,0xcb,0x6b,0x3b},{0x62,0x53,0xa2,0xc0,0x66,0x35},
	{0x64,0x56,0xb9,0xdd,0x71,0x27},{0x66,0x55,0xb0,0xd6,0x7c,0x29},
	{0x68,0x5c,0x8f,0xe7,0x5f,0x03},{0x6a,0x5f,0x86,0xec,0x52,0x0d},
	{0x6c,0x5a,0x9d,0xf1,0x45,0x1f},{0x6e,0x59,0x94,0xfa,0x48,0x11},
	{0x70,0x48,0xe3,0x93,0x03,0x4b},{0x72,0x4b,0xea,0x98,0x0e,0x45},
	{0x74,0x4e,0xf1,0x85,0x19,0x57},{0x76,0x4d,0xf8,0x8e,0x14,0x59},
	{0x78,0x44,0xc7,0xbf,0x37,0x73},{0x7a,0x47,0xce,0xb4,0x3a,0x7d},
	{0x7c,0x42,0xd5,0xa9,0x2d,0x6f},{0x7e,0x41,0xdc,0xa2,0x20,0x61},
	{0x80,0xc0,0x76,0xf6,0x6d,0xad},{0x82,0xc3,0x7f,0xfd,0x60,0xa3},
	{0x84,0xc6,0x64,0xe0,0x77,0xb1},{0x86,0xc5,0x6d,0xeb,0x7a,0xbf},
	{0x88,0xcc,0x52,0xda,0x59,0x95},{0x8a,0xcf,0x5b,0xd1,0x54,0x9b},
	{0x8c,0xca,0x40,0xcc,0x43,0x89},{0x8e,0xc9,0x49,0xc7,0x4e,0x87},
	{0x90,0xd8,0x3e,0xae,0x05,0xdd},{0x92,0xdb,0x37,0xa5,0x08,0xd3},
	{0x94,0xde,0x2c,0xb8,0x1f,0xc1},{0x96,0xdd,0x25,0xb3,0x12,0xcf},
	{0x98,0xd4,0x1a,0x82,0x31,0xe5},{0x9a,0xd7,0x13,0x89,0x3c,0xeb},
	{0x9c,0xd2,0x08,0x94,0x2b,0xf9},{0x9e,0xd1,0x01,0x9f,0x26,0xf7},
	{0xa0,0xf0,0xe6,0x46,0xbd,0x4d},{0xa2,0xf3,0xef,0x4d,0xb0,0x43},
	{0xa4,0xf6,0xf4,0x50,0xa7,0x51},{0xa6,0xf5,0xfd,0x5b,0xaa,0x5f},
	{0xa8,0xfc,0xc2,0x6a,0x89,0x75},{0xaa,0xff,0xcb,0x61,0x84,0x7b},
	{0xac,0xfa,0xd0,0x7c,0x93,0x69},{0xae,0xf9,0xd9,0x77,0x9e,0x67},
	{0xb0,0xe8,0xae,0x1e,0xd5,0x3d},{0xb2,0xeb,0xa7,0x15,0xd8,0x33},
	{0xb4,0xee,0xbc,0x08,0xcf,0x21},{0xb6,0xed,0xb5,0x03,0xc2,0x2f},
	{0xb8,0xe4,0x8a,0x32,0xe1,0x05},{0xba,0xe7,0x83,0x39,0xec,0x0b},
	{0xbc,0xe2,0x98,0x24,0xfb,0x19},{0xbe,0xe1,0x91,0x2f,0xf6,0x17},
	{0xc0,0xa0,0x4d,0x8d,0xd6,0x76},{0xc2,0xa3,0x44,0x86,0xdb,0x78},
	{0xc4,0xa6,0x5f,0x9b,0xcc,0x6a},{0xc6,0xa5,0x56,0x90,0xc1,0x64},
	{0xc8,0xac,0x69,0xa1,0xe2,0x4e},{0xca,0xaf,0x60,0xaa,0xef,0x40},
	{0xcc,0xaa,0x7b,0xb7,0xf8,0x52},{0xce,0xa9,0x72,0xbc,0xf5,0x5c},
	{0xd0,0xb8,0x05,0xd5,0xbe,0x06},{0xd2,0xbb,0x0c,0xde,0xb3,0x08},
	{0xd4,0xbe,0x17,0xc3,0xa4,0x1a},{0xd6,0xbd,0x1e,0xc8,0xa9,0x14},
	{0xd8,0xb4,0x21,0xf9,0x8a,0x3e},{0xda,0xb7,0x28,0xf2,0x87,0x30},
	{0xdc,0xb2,0x33,0xef,0x90,0x22},{0xde,0xb1,0x3a,0xe4,0x9d,0x2c},
	{0xe0,0x90,0xdd,0x3d,0x06,0x96},{0xe2,0x93,0xd4,0x36,0x0b,0x98},
	{0xe4,0x96,0xcf,0x2b,0x1c,0x8a},{0xe6,0x95,0xc6,0x20,0x11,0x84},
	{0xe8,0x9c,0xf9,0x11,0x32,0xae},{0xea,0x9f,0xf0,0x1a,0x3f,0xa0},
	{0xec,0x9a,0xeb,0x07,0x28,0xb2},{0xee,0x99,0xe2,0x0c,0x25,0xbc},
	{0xf0,0x88,0x95,0x65,0x6e,0xe6},{0xf2,0x8b,0x9c,0x6e,0x63,0xe8},
	{0xf4,0x8e,0x87,0x73,0x74,0xfa},{0xf6,0x8d,0x8e,0x78,0x79,0xf4},
	{0xf8,0x84,0xb1,0x49,0x5a,0xde},{0xfa,0x87,0xb8,0x42,0x57,0xd0},
	{0xfc,0x82,0xa3,0x5f,0x40,0xc2},{0xfe,0x81,0xaa,0x54,0x4d,0xcc},
	{0x1b,0x9b,0xec,0xf7,0xda,0x41},{0x19,0x98,0xe5,0xfc,0xd7,0x4f},
	{0x1f,0x9d,0xfe,0xe1,0xc0,0x5d},{0x1d,0x9e,0xf7,0xea,0xcd,0x53},
	{0x13,0x97,0xc8,0xdb,0xee,0x79},{0x11,0x94,0xc1,0xd0,0xe3,0x77},
	{0x17,0x91,0xda,0xcd,0xf4,0x65},{0x15,0x92,0xd3,0xc6,0xf9,0x6b},
	{0x0b,0x83,0xa4,0xaf,0xb2,0x31},{0x09,0x80,0xad,0xa4,0xbf,0x3f},
	{0x0f,0x85,0xb6,0xb9,0xa8,0x2d},{0x0d,0x86,0xbf,0xb2,0xa5,0x23},
	{0x03,0x8f,0x80,0x83,0x86,0x09},{0x01,0x8c,0x89,0x88,0x8b,0x07},
	{0x07,0x89,0x92,0x95,0x9c,0x15},{0x05,0x8a,0x9b,0x9e,0x91,0x1b},
	{0x3b,0xab,0x7c,0x47,0x0a,0xa1},{0x39,0xa8,0x75,0x4c,0x07,0xaf},
	{0x3f,0xad,0x6e,0x51,0x10,0xbd},{0x3d,0xae,0x67,0x5a,0x1d,0xb3},
	{0x33,0xa7,0x58,0x6b,0x3e,0x99},{0x31,0xa4,0x51,0x60,0x33,0x97},
	{0x37,0xa1,0x4a,0x7d,0x24,0x85},{0x35,0xa2,0x43,0x76,0x29,0x8b},
	{0x2b,0xb3,0x34,0x1f,0x62,0xd1},{0x29,0xb0,0x3d,0x14,0x6f,0xdf},
	{0x2f,0xb5,0x26,0x09,0x78,0xcd},{0x2d,0xb6,0x2f,0x02,0x75,0xc3},
	{0x23,0xbf,0x10,0x33,0x56,0xe9},{0x21,0xbc,0x19,0x38,0x5b,0xe7},
	{0x27,0xb9,0x02,0x25,0x4c,0xf5},{0x25,0xba,0x0b,0x2e,0x41,0xfb},
	{0x5b,0xfb,0xd7,0x8c,0x61,0x9a},{0x59,0xf8,0xde,0x87,0x6c,0x94},
	{0x5f,0xfd,0xc5,0x9a,0x7b,0x86},{0x5d,0xfe,0xcc,0x91,0x76,0x88},
	{0x53,0xf7,0xf3,0xa0,0x55,0xa2},{0x51,0xf4,0xfa,0xab,0x58,0xac},
	{0x57,0xf1,0xe1,0xb6,0x4f,0xbe},{0x55,0xf2,0xe8,0xbd,0x42,0xb0},
	{0x4b,0xe3,0x9f,0xd4,0x09,0xea},{0x49,0xe0,0x96,0xdf,0x04,0xe4},
	{0x4f,0xe5,0x8d,0xc2,0x13,0xf6},{0x4d,0xe6,0x84,0xc9,0x1e,0xf8},
	{0x43,0xef,0xbb,0xf8,0x3d,0xd2},{0x41,0xec,0xb2,0xf3,0x30,0xdc},
	{0x47,0xe9,0xa9,0xee,0x27,0xce},{0x45,0xea,0xa0,0xe5,0x2a,0xc0},
	{0x7b,0xcb,0x47,0x3c,0xb1,0x7a},{0x79,0xc8,0x4e,0x37,0xbc,0x74},
	{0x7f,0xcd,0x55,0x2a,0xab,0x66},{0x7d,0xce,0x5c,0x21,0xa6,0x68},
	{0x73,0xc7,0x63,0x10,0x85,0x42},{0x71,0xc4,0x6a,0x1b,0x88,0x4c},
	{0x77,0xc1,0x71,0x06,0x9f,0x5e},{0x75,0xc2,0x78,0x0d,0x92,0x50},
	{0x6b,0xd3,0x0f,0x64,0xd9,0x0a},{0x69,0xd0,0x06,0x6f,0xd4,0x04},
	{0x6f,0xd5,0x1d,0x72,0xc3,0x16},{0x6d,0xd6,0x14,0x79,0xce,0x18},
	{0x63,0xdf,0x2b,0x48,0xed,0x32},{0x61,0xdc,0x22,0x43,0xe0,0x3c},
	{0x67,0xd9,0x39,0x5e,0xf7,0x2e},{0x65,0xda,0x30,0x55,0xfa,0x20},
	{0x9b,0x5b,0x9a,0x01,0xb7,0xec},{0x99,0x58,0x93,0x0a,0xba,0xe2},
	{0x9f,0x5d,0x88,0x17,0xad,0xf0},{0x9d,0x5e,0x81,0x1c,0xa0,0xfe},
	{0x93,0x57,0xbe,0x2d,0x83,0xd4},{0x91,0x54,0xb7,0x26,0x8e,0xda},
	{0x97,0x51,0xac,0x3b,0x99,0xc8},{0x95,0x52,0xa5,0x30,0x94,0xc6},
	{0x8b,0x43,0xd2,0x59,0xdf,0x9c},{0x89,0x40,0xdb,0x52,0xd2,0x92},
	{0x8f,0x45,0xc0,0x4f,0xc5,0x80},{0x8d,0x46,0xc9,0x44,0xc8,0x8e},
	{0x83,0x4f,0xf6,0x75,0xeb,0xa4},{0x81,0x4c,0xff,0x7e,0xe6,0xaa},
	{0x87,0x49,0xe4,0x63,0xf1,0xb8},{0x85,0x4a,0xed,0x68,0xfc,0xb6},
	{0xbb,0x6b,0x0a,0xb1,0x67,0x0c},{0xb9,0x68,0x03,0xba,0x6a,0x02},
	{0xbf,0x6d,0x18,0xa7,0x7d,0x10},{0xbd,0x6e,0x11,0xac,0x70,0x1e},
	{0xb3,0x67,0x2e,0x9d,0x53,0x34},{0xb1,0x64,0x27,0x96,0x5e,0x3a},
	{0xb7,0x61,0x3c,0x8b,0x49,0x28},{0xb5,0x62,0x35,0x80,0x44,0x26},
	{0xab,0x73,0x42,0xe9,0x0f,0x7c},{0xa9,0x70,0x4b,0xe2,0x02,0x72},
	{0xaf,0x75,0x50,0xff,0x15,0x60},{0xad,0x76,0x59,0xf4,0x18,0x6e},
	{0xa3,0x7f,0x66,0xc5,0x3b,0x44},{0xa1,0x7c,0x6f,0xce,0x36,0x4a},
	{0xa7,0x79,0x74,0xd3,0x21,0x58},{0xa5,0x7a,0x7d,0xd8,0x2c,0x56},
	{0xdb,0x3b,0xa1,0x7a,0x0c,0x37},{0xd9,0x38,0xa8,0x71,0x01,0x39},
	{0xdf,0x3d,0xb3,0x6c,0x16,0x2b},{0xdd,0x3e,0xba,0x67,0x1b,0x25},
	{0xd3,0x37,0x85,0x56,0x38,0x0f},{0xd1,0x34,0x8c,0x5d,0x35,0x01},
	{0xd7,0x31,0x97,0x40,0x22,0x13},{0xd5,0x32,0x9e,0x4b,0x2f,0x1d},
	{0xcb,0x23,0xe9,0x22,0x64,0x47},{0xc9,0x20,0xe0,0x29,0x69,0x49},
	{0xcf,0x25,0xfb,0x34,0x7e,0x5b},{0xcd,0x26,0xf2,0x3f,0x73,0x55},
	{0xc3,0x2f,0xcd,0x0e,0x50,0x7f},{0xc1,0x2c,0xc4,0x05,0x5d,0x71},
	{0xc7,0x29,0xdf,0x18,0x4a,0x63},{0xc5,0x2a,0xd6,0x13,0x47,0x6d},
	{0xfb,0x0b,0x31,0xca,0xdc,0xd7},{0xf9,0x08,0x38,0xc1,0xd1,0xd9},
	{0xff,0x0d,0x23,0xdc,0xc6,0xcb},{0xfd,0x0e,0x2a,0xd7,0xcb,0xc5},
	{0xf3,0x07,0x15,0xe6,0xe8,0xef},{0xf1,0x04,0x1c,0xed,0xe5,0xe1},
	{0xf7,0x01,0x07,0xf0,0xf2,0xf3},{0xf5,0x02,0x0e,0xfb,0xff,0xfd},
	{0xeb,0x13,0x79,0x92,0xb4,0xa7},{0xe9,0x10,0x70,0x99,0xb9,0xa9},
	{0xef,0x15,0x6b,0x84,0xae,0xbb},{0xed,0x16,0x62,0x8f,0xa3,0xb5},
	{0xe3,0x1f,0x5d,0xbe,0x80,0x9f},{0xe1,0x1c,0x54,0xb5,0x8d,0x91},
	{0xe7,0x19,0x4f,0xa8,0x9a,0x83},{0xe5,0x1a,0x46,0xa3,0x97,0x8d}
};

/*********************** FUNCTION DEFINITIONS ***********************/
// XORs the in and out buffers, storing the result in out. Length is in bytes.
void xor_buf(const BYTE in[], BYTE out[], size_t len)
{
	size_t idx;

	for (idx = 0; idx < len; idx++)
		out[idx] ^= in[idx];
}

/*******************
* AES - CBC
*******************/
int aes_encrypt_cbc(const BYTE in[], size_t in_len, BYTE out[], const UINT key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

	if (in_len % AES_BLOCK_SIZE != 0)
		return(FALSE);

	blocks = (int)(in_len / AES_BLOCK_SIZE);

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; idx++) {
		memcpy(buf_in, &in[idx * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		xor_buf(iv_buf, buf_in, AES_BLOCK_SIZE);
		aes_encrypt(buf_in, buf_out, key, keysize);
		memcpy(&out[idx * AES_BLOCK_SIZE], buf_out, AES_BLOCK_SIZE);
		memcpy(iv_buf, buf_out, AES_BLOCK_SIZE);
	}

	return(TRUE);
}

int aes_encrypt_cbc_mac(const BYTE in[], size_t in_len, BYTE out[], const UINT key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

	if (in_len % AES_BLOCK_SIZE != 0)
		return(FALSE);

	blocks = (int)(in_len / AES_BLOCK_SIZE);

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; idx++) {
		memcpy(buf_in, &in[idx * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		xor_buf(iv_buf, buf_in, AES_BLOCK_SIZE);
		aes_encrypt(buf_in, buf_out, key, keysize);
		memcpy(iv_buf, buf_out, AES_BLOCK_SIZE);
		// Do not output all encrypted blocks.
	}

	memcpy(out, buf_out, AES_BLOCK_SIZE);   // Only output the last block.

	return(TRUE);
}

int aes_decrypt_cbc(const BYTE in[], size_t in_len, BYTE out[], const UINT key[], int keysize, const BYTE iv[])
{
	BYTE buf_in[AES_BLOCK_SIZE], buf_out[AES_BLOCK_SIZE], iv_buf[AES_BLOCK_SIZE];
	int blocks, idx;

	if (in_len % AES_BLOCK_SIZE != 0)
		return(FALSE);

	blocks = (int)(in_len / AES_BLOCK_SIZE);

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; idx++) {
		memcpy(buf_in, &in[idx * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		aes_decrypt(buf_in, buf_out, key, keysize);
		xor_buf(iv_buf, buf_out, AES_BLOCK_SIZE);
		memcpy(&out[idx * AES_BLOCK_SIZE], buf_out, AES_BLOCK_SIZE);
		memcpy(iv_buf, buf_in, AES_BLOCK_SIZE);
	}

	return(TRUE);
}

/*******************
* AES - CTR
*******************/
void increment_iv(BYTE iv[], int counter_size)
{
	int idx;

	// Use counter_size bytes at the end of the IV as the big-endian integer to increment.
	for (idx = AES_BLOCK_SIZE - 1; idx >= AES_BLOCK_SIZE - counter_size; idx--) {
		iv[idx]++;
		if (iv[idx] != 0 || idx == AES_BLOCK_SIZE - counter_size)
			break;
	}
}

// Performs the encryption in-place, the input and output buffers may be the same.
// Input may be an arbitrary length (in bytes).
void aes_encrypt_ctr(const BYTE in[], size_t in_len, BYTE out[], const UINT key[], int keysize, const BYTE iv[])
{
	size_t idx = 0, last_block_length;
	BYTE iv_buf[AES_BLOCK_SIZE], out_buf[AES_BLOCK_SIZE];

	if (in != out)
		memcpy(out, in, in_len);

	memcpy(iv_buf, iv, AES_BLOCK_SIZE);
	last_block_length = in_len - AES_BLOCK_SIZE;

	if (in_len > AES_BLOCK_SIZE) {
		for (idx = 0; idx < last_block_length; idx += AES_BLOCK_SIZE) {
			aes_encrypt(iv_buf, out_buf, key, keysize);
			xor_buf(out_buf, &out[idx], AES_BLOCK_SIZE);
			increment_iv(iv_buf, AES_BLOCK_SIZE);
		}
	}

	aes_encrypt(iv_buf, out_buf, key, keysize);
	xor_buf(out_buf, &out[idx], in_len - idx);   // Use the Most Significant bytes.
}

void aes_decrypt_ctr(const BYTE in[], size_t in_len, BYTE out[], const UINT key[], int keysize, const BYTE iv[])
{
	// CTR encryption is its own inverse function.
	aes_encrypt_ctr(in, in_len, out, key, keysize, iv);
}

/*******************
* AES - CCM
*******************/
// out_len = payload_len + assoc_len
int aes_encrypt_ccm(const BYTE payload[], UINT payload_len, const BYTE assoc[], unsigned short assoc_len,
	const BYTE nonce[], unsigned short nonce_len, BYTE out[], UINT *out_len,
	UINT mac_len, const BYTE key_str[], int keysize)
{
	BYTE temp_iv[AES_BLOCK_SIZE], counter[AES_BLOCK_SIZE], mac[16], *buf;
	int end_of_buf, payload_len_store_size;
	UINT key[60];

	if (mac_len != 4 && mac_len != 6 && mac_len != 8 && mac_len != 10 &&
		mac_len != 12 && mac_len != 14 && mac_len != 16)
		return(FALSE);

	if (nonce_len < 7 || nonce_len > 13)
		return(FALSE);

	if (assoc_len > 32768 /* = 2^15 */)
		return(FALSE);

	buf = (BYTE*)malloc(payload_len + assoc_len + 48 /*Round both payload and associated data up a block size and add an extra block.*/);
	if (! buf)
		return(FALSE);

	// Prepare the key for usage.
	aes_key_setup(key_str, key, keysize);

	// Format the first block of the formatted data.
	payload_len_store_size = AES_BLOCK_SIZE - 1 - nonce_len;
	ccm_prepare_first_format_blk(buf, assoc_len, payload_len, payload_len_store_size, mac_len, nonce, nonce_len);
	end_of_buf = AES_BLOCK_SIZE;

	// Format the Associated Data, aka, assoc[].
	ccm_format_assoc_data(buf, &end_of_buf, assoc, assoc_len);

	// Format the Payload, aka payload[].
	ccm_format_payload_data(buf, &end_of_buf, payload, payload_len);

	// Create the first counter block.
	ccm_prepare_first_ctr_blk(counter, nonce, nonce_len, payload_len_store_size);

	// Perform the CBC operation with an IV of zeros on the formatted buffer to calculate the MAC.
	memset(temp_iv, 0, AES_BLOCK_SIZE);
	aes_encrypt_cbc_mac(buf, end_of_buf, mac, key, keysize, temp_iv);

	// Copy the Payload and MAC to the output buffer.
	memcpy(out, payload, payload_len);
	memcpy(&out[payload_len], mac, mac_len);

	// Encrypt the Payload with CTR mode with a counter starting at 1.
	memcpy(temp_iv, counter, AES_BLOCK_SIZE);
	increment_iv(temp_iv, AES_BLOCK_SIZE - 1 - mac_len);   // Last argument is the byte size of the counting portion of the counter block. /*BUG?*/
	aes_encrypt_ctr(out, payload_len, out, key, keysize, temp_iv);

	// Encrypt the MAC with CTR mode with a counter starting at 0.
	aes_encrypt_ctr(&out[payload_len], mac_len, &out[payload_len], key, keysize, counter);

	free(buf);
	*out_len = payload_len + mac_len;

	return(TRUE);
}

// plaintext_len = ciphertext_len - mac_len
// Needs a flag for whether the MAC matches.
int aes_decrypt_ccm(const BYTE ciphertext[], UINT ciphertext_len, const BYTE assoc[], unsigned short assoc_len,
	const BYTE nonce[], unsigned short nonce_len, BYTE plaintext[], UINT *plaintext_len,
	UINT mac_len, int *mac_auth, const BYTE key_str[], int keysize)
{
	BYTE temp_iv[AES_BLOCK_SIZE], counter[AES_BLOCK_SIZE], mac[16], mac_buf[16], *buf;
	int end_of_buf, plaintext_len_store_size;
	UINT key[60];

	if (ciphertext_len <= mac_len)
		return(FALSE);

	buf = (BYTE*)malloc(assoc_len + ciphertext_len /*ciphertext_len = plaintext_len + mac_len*/ + 48);
	if (! buf)
		return(FALSE);

	// Prepare the key for usage.
	aes_key_setup(key_str, key, keysize);

	// Copy the plaintext and MAC to the output buffers.
	*plaintext_len = ciphertext_len - mac_len;
	plaintext_len_store_size = AES_BLOCK_SIZE - 1 - nonce_len;
	memcpy(plaintext, ciphertext, *plaintext_len);
	memcpy(mac, &ciphertext[*plaintext_len], mac_len);

	// Prepare the first counter block for use in decryption.
	ccm_prepare_first_ctr_blk(counter, nonce, nonce_len, plaintext_len_store_size);

	// Decrypt the Payload with CTR mode with a counter starting at 1.
	memcpy(temp_iv, counter, AES_BLOCK_SIZE);
	increment_iv(temp_iv, AES_BLOCK_SIZE - 1 - mac_len);   // (AES_BLOCK_SIZE - 1 - mac_len) is the byte size of the counting portion of the counter block.
	aes_decrypt_ctr(plaintext, *plaintext_len, plaintext, key, keysize, temp_iv);

	// Setting mac_auth to nullptr disables the authentication check.
	if (mac_auth != nullptr) {
		// Decrypt the MAC with CTR mode with a counter starting at 0.
		aes_decrypt_ctr(mac, mac_len, mac, key, keysize, counter);

		// Format the first block of the formatted data.
		plaintext_len_store_size = AES_BLOCK_SIZE - 1 - nonce_len;
		ccm_prepare_first_format_blk(buf, assoc_len, *plaintext_len, plaintext_len_store_size, mac_len, nonce, nonce_len);
		end_of_buf = AES_BLOCK_SIZE;

		// Format the Associated Data into the authentication buffer.
		ccm_format_assoc_data(buf, &end_of_buf, assoc, assoc_len);

		// Format the Payload into the authentication buffer.
		ccm_format_payload_data(buf, &end_of_buf, plaintext, *plaintext_len);

		// Perform the CBC operation with an IV of zeros on the formatted buffer to calculate the MAC.
		memset(temp_iv, 0, AES_BLOCK_SIZE);
		aes_encrypt_cbc_mac(buf, end_of_buf, mac_buf, key, keysize, temp_iv);

		// Compare the calculated MAC against the MAC embedded in the ciphertext to see if they are the same.
		if (! memcmp(mac, mac_buf, mac_len)) {
			*mac_auth = TRUE;
		}
		else {
			*mac_auth = FALSE;
			memset(plaintext, 0, *plaintext_len);
		}
	}

	free(buf);

	return(TRUE);
}

// Creates the first counter block. First byte is flags, then the nonce, then the incremented part.
void ccm_prepare_first_ctr_blk(BYTE counter[], const BYTE nonce[], int nonce_len, int payload_len_store_size)
{
	memset(counter, 0, AES_BLOCK_SIZE);
	counter[0] = (payload_len_store_size - 1) & 0x07;
	memcpy(&counter[1], nonce, nonce_len);
}

void ccm_prepare_first_format_blk(BYTE buf[], int assoc_len, int payload_len, int payload_len_store_size, int mac_len, const BYTE nonce[], int nonce_len)
{
	// Set the flags for the first byte of the first block.
	buf[0] = ((((mac_len - 2) / 2) & 0x07) << 3) | ((payload_len_store_size - 1) & 0x07);
	if (assoc_len > 0)
		buf[0] += 0x40;
	// Format the rest of the first block, storing the nonce and the size of the payload.
	memcpy(&buf[1], nonce, nonce_len);
	memset(&buf[1 + nonce_len], 0, AES_BLOCK_SIZE - 1 - nonce_len);
	buf[15] = payload_len & 0x000000FF;
	buf[14] = (payload_len >> 8) & 0x000000FF;
}

void ccm_format_assoc_data(BYTE buf[], int *end_of_buf, const BYTE assoc[], int assoc_len)
{
	int pad;

	buf[*end_of_buf + 1] = assoc_len & 0x00FF;
	buf[*end_of_buf] = (assoc_len >> 8) & 0x00FF;
	*end_of_buf += 2;
	memcpy(&buf[*end_of_buf], assoc, assoc_len);
	*end_of_buf += assoc_len;
	pad = AES_BLOCK_SIZE - (*end_of_buf % AES_BLOCK_SIZE); /*BUG?*/
	memset(&buf[*end_of_buf], 0, pad);
	*end_of_buf += pad;
}

void ccm_format_payload_data(BYTE buf[], int *end_of_buf, const BYTE payload[], int payload_len)
{
	int pad;

	memcpy(&buf[*end_of_buf], payload, payload_len);
	*end_of_buf += payload_len;
	pad = *end_of_buf % AES_BLOCK_SIZE;
	if (pad != 0)
		pad = AES_BLOCK_SIZE - pad;
	memset(&buf[*end_of_buf], 0, pad);
	*end_of_buf += pad;
}

/*******************
* AES
*******************/
/////////////////
// KEY EXPANSION
/////////////////

// Substitutes a word using the AES S-Box.
UINT SubWord(UINT word)
{
	unsigned int result;

	result = (int)aes_sbox[(word >> 4) & 0x0000000F][word & 0x0000000F];
	result += (int)aes_sbox[(word >> 12) & 0x0000000F][(word >> 8) & 0x0000000F] << 8;
	result += (int)aes_sbox[(word >> 20) & 0x0000000F][(word >> 16) & 0x0000000F] << 16;
	result += (int)aes_sbox[(word >> 28) & 0x0000000F][(word >> 24) & 0x0000000F] << 24;
	return(result);
}

// Performs the action of generating the keys that will be used in every round of
// encryption. "key" is the user-supplied input key, "w" is the output key schedule,
// "keysize" is the length in bits of "key", must be 128, 192, or 256.
void aes_key_setup(const BYTE key[], UINT w[], int keysize)
{
	int Nb=4,Nr,Nk,idx;
	UINT temp,Rcon[]={0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,
		0x40000000,0x80000000,0x1b000000,0x36000000,0x6c000000,0xd8000000,
		0xab000000,0x4d000000,0x9a000000};

	switch (keysize) {
	case 128: Nr = 10; Nk = 4; break;
	case 192: Nr = 12; Nk = 6; break;
	case 256: Nr = 14; Nk = 8; break;
	default: return;
	}

	for (idx=0; idx < Nk; ++idx) {
		w[idx] = ((key[4 * idx]) << 24) | ((key[4 * idx + 1]) << 16) |
			((key[4 * idx + 2]) << 8) | ((key[4 * idx + 3]));
	}

	for (idx = Nk; idx < Nb * (Nr+1); ++idx) {
		temp = w[idx - 1];
		if ((idx % Nk) == 0)
			temp = SubWord(KE_ROTWORD(temp)) ^ Rcon[(idx-1)/Nk];
		else if (Nk > 6 && (idx % Nk) == 4)
			temp = SubWord(temp);
		w[idx] = w[idx-Nk] ^ temp;
	}
}

/////////////////
// ADD ROUND KEY
/////////////////

// Performs the AddRoundKey step. Each round has its own pre-generated 16-byte key in the
// form of 4 integers (the "w" array). Each integer is XOR'd by one column of the state.
// Also performs the job of InvAddRoundKey(); since the function is a simple XOR process,
// it is its own inverse.
void AddRoundKey(BYTE state[][4], const UINT w[])
{
	BYTE subkey[4];

	// memcpy(subkey,&w[idx],4); // Not accurate for big endian machines
	// Subkey 1
	subkey[0] = w[0] >> 24;
	subkey[1] = w[0] >> 16;
	subkey[2] = w[0] >> 8;
	subkey[3] = w[0];
	state[0][0] ^= subkey[0];
	state[1][0] ^= subkey[1];
	state[2][0] ^= subkey[2];
	state[3][0] ^= subkey[3];
	// Subkey 2
	subkey[0] = w[1] >> 24;
	subkey[1] = w[1] >> 16;
	subkey[2] = w[1] >> 8;
	subkey[3] = w[1];
	state[0][1] ^= subkey[0];
	state[1][1] ^= subkey[1];
	state[2][1] ^= subkey[2];
	state[3][1] ^= subkey[3];
	// Subkey 3
	subkey[0] = w[2] >> 24;
	subkey[1] = w[2] >> 16;
	subkey[2] = w[2] >> 8;
	subkey[3] = w[2];
	state[0][2] ^= subkey[0];
	state[1][2] ^= subkey[1];
	state[2][2] ^= subkey[2];
	state[3][2] ^= subkey[3];
	// Subkey 4
	subkey[0] = w[3] >> 24;
	subkey[1] = w[3] >> 16;
	subkey[2] = w[3] >> 8;
	subkey[3] = w[3];
	state[0][3] ^= subkey[0];
	state[1][3] ^= subkey[1];
	state[2][3] ^= subkey[2];
	state[3][3] ^= subkey[3];
}

/////////////////
// (Inv)SubBytes
/////////////////

// Performs the SubBytes step. All bytes in the state are substituted with a
// pre-calculated value from a lookup table.
void SubBytes(BYTE state[][4])
{
	state[0][0] = aes_sbox[state[0][0] >> 4][state[0][0] & 0x0F];
	state[0][1] = aes_sbox[state[0][1] >> 4][state[0][1] & 0x0F];
	state[0][2] = aes_sbox[state[0][2] >> 4][state[0][2] & 0x0F];
	state[0][3] = aes_sbox[state[0][3] >> 4][state[0][3] & 0x0F];
	state[1][0] = aes_sbox[state[1][0] >> 4][state[1][0] & 0x0F];
	state[1][1] = aes_sbox[state[1][1] >> 4][state[1][1] & 0x0F];
	state[1][2] = aes_sbox[state[1][2] >> 4][state[1][2] & 0x0F];
	state[1][3] = aes_sbox[state[1][3] >> 4][state[1][3] & 0x0F];
	state[2][0] = aes_sbox[state[2][0] >> 4][state[2][0] & 0x0F];
	state[2][1] = aes_sbox[state[2][1] >> 4][state[2][1] & 0x0F];
	state[2][2] = aes_sbox[state[2][2] >> 4][state[2][2] & 0x0F];
	state[2][3] = aes_sbox[state[2][3] >> 4][state[2][3] & 0x0F];
	state[3][0] = aes_sbox[state[3][0] >> 4][state[3][0] & 0x0F];
	state[3][1] = aes_sbox[state[3][1] >> 4][state[3][1] & 0x0F];
	state[3][2] = aes_sbox[state[3][2] >> 4][state[3][2] & 0x0F];
	state[3][3] = aes_sbox[state[3][3] >> 4][state[3][3] & 0x0F];
}

void InvSubBytes(BYTE state[][4])
{
	state[0][0] = aes_invsbox[state[0][0] >> 4][state[0][0] & 0x0F];
	state[0][1] = aes_invsbox[state[0][1] >> 4][state[0][1] & 0x0F];
	state[0][2] = aes_invsbox[state[0][2] >> 4][state[0][2] & 0x0F];
	state[0][3] = aes_invsbox[state[0][3] >> 4][state[0][3] & 0x0F];
	state[1][0] = aes_invsbox[state[1][0] >> 4][state[1][0] & 0x0F];
	state[1][1] = aes_invsbox[state[1][1] >> 4][state[1][1] & 0x0F];
	state[1][2] = aes_invsbox[state[1][2] >> 4][state[1][2] & 0x0F];
	state[1][3] = aes_invsbox[state[1][3] >> 4][state[1][3] & 0x0F];
	state[2][0] = aes_invsbox[state[2][0] >> 4][state[2][0] & 0x0F];
	state[2][1] = aes_invsbox[state[2][1] >> 4][state[2][1] & 0x0F];
	state[2][2] = aes_invsbox[state[2][2] >> 4][state[2][2] & 0x0F];
	state[2][3] = aes_invsbox[state[2][3] >> 4][state[2][3] & 0x0F];
	state[3][0] = aes_invsbox[state[3][0] >> 4][state[3][0] & 0x0F];
	state[3][1] = aes_invsbox[state[3][1] >> 4][state[3][1] & 0x0F];
	state[3][2] = aes_invsbox[state[3][2] >> 4][state[3][2] & 0x0F];
	state[3][3] = aes_invsbox[state[3][3] >> 4][state[3][3] & 0x0F];
}

/////////////////
// (Inv)ShiftRows
/////////////////

// Performs the ShiftRows step. All rows are shifted cylindrically to the left.
void ShiftRows(BYTE state[][4])
{
	int t;

	// Shift left by 1
	t = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = t;
	// Shift left by 2
	t = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = t;
	t = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = t;
	// Shift left by 3
	t = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = t;
}

// All rows are shifted cylindrically to the right.
void InvShiftRows(BYTE state[][4])
{
	int t;

	// Shift right by 1
	t = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = t;
	// Shift right by 2
	t = state[2][3];
	state[2][3] = state[2][1];
	state[2][1] = t;
	t = state[2][2];
	state[2][2] = state[2][0];
	state[2][0] = t;
	// Shift right by 3
	t = state[3][3];
	state[3][3] = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = t;
}

/////////////////
// (Inv)MixColumns
/////////////////

// Performs the MixColums step. The state is multiplied by itself using matrix
// multiplication in a Galios Field 2^8. All multiplication is pre-computed in a table.
// Addition is equivilent to XOR. (Must always make a copy of the column as the original
// values will be destoyed.)
void MixColumns(BYTE state[][4])
{
	BYTE col[4];

	// Column 1
	col[0] = state[0][0];
	col[1] = state[1][0];
	col[2] = state[2][0];
	col[3] = state[3][0];
	state[0][0] = gf_mul[col[0]][0];
	state[0][0] ^= gf_mul[col[1]][1];
	state[0][0] ^= col[2];
	state[0][0] ^= col[3];
	state[1][0] = col[0];
	state[1][0] ^= gf_mul[col[1]][0];
	state[1][0] ^= gf_mul[col[2]][1];
	state[1][0] ^= col[3];
	state[2][0] = col[0];
	state[2][0] ^= col[1];
	state[2][0] ^= gf_mul[col[2]][0];
	state[2][0] ^= gf_mul[col[3]][1];
	state[3][0] = gf_mul[col[0]][1];
	state[3][0] ^= col[1];
	state[3][0] ^= col[2];
	state[3][0] ^= gf_mul[col[3]][0];
	// Column 2
	col[0] = state[0][1];
	col[1] = state[1][1];
	col[2] = state[2][1];
	col[3] = state[3][1];
	state[0][1] = gf_mul[col[0]][0];
	state[0][1] ^= gf_mul[col[1]][1];
	state[0][1] ^= col[2];
	state[0][1] ^= col[3];
	state[1][1] = col[0];
	state[1][1] ^= gf_mul[col[1]][0];
	state[1][1] ^= gf_mul[col[2]][1];
	state[1][1] ^= col[3];
	state[2][1] = col[0];
	state[2][1] ^= col[1];
	state[2][1] ^= gf_mul[col[2]][0];
	state[2][1] ^= gf_mul[col[3]][1];
	state[3][1] = gf_mul[col[0]][1];
	state[3][1] ^= col[1];
	state[3][1] ^= col[2];
	state[3][1] ^= gf_mul[col[3]][0];
	// Column 3
	col[0] = state[0][2];
	col[1] = state[1][2];
	col[2] = state[2][2];
	col[3] = state[3][2];
	state[0][2] = gf_mul[col[0]][0];
	state[0][2] ^= gf_mul[col[1]][1];
	state[0][2] ^= col[2];
	state[0][2] ^= col[3];
	state[1][2] = col[0];
	state[1][2] ^= gf_mul[col[1]][0];
	state[1][2] ^= gf_mul[col[2]][1];
	state[1][2] ^= col[3];
	state[2][2] = col[0];
	state[2][2] ^= col[1];
	state[2][2] ^= gf_mul[col[2]][0];
	state[2][2] ^= gf_mul[col[3]][1];
	state[3][2] = gf_mul[col[0]][1];
	state[3][2] ^= col[1];
	state[3][2] ^= col[2];
	state[3][2] ^= gf_mul[col[3]][0];
	// Column 4
	col[0] = state[0][3];
	col[1] = state[1][3];
	col[2] = state[2][3];
	col[3] = state[3][3];
	state[0][3] = gf_mul[col[0]][0];
	state[0][3] ^= gf_mul[col[1]][1];
	state[0][3] ^= col[2];
	state[0][3] ^= col[3];
	state[1][3] = col[0];
	state[1][3] ^= gf_mul[col[1]][0];
	state[1][3] ^= gf_mul[col[2]][1];
	state[1][3] ^= col[3];
	state[2][3] = col[0];
	state[2][3] ^= col[1];
	state[2][3] ^= gf_mul[col[2]][0];
	state[2][3] ^= gf_mul[col[3]][1];
	state[3][3] = gf_mul[col[0]][1];
	state[3][3] ^= col[1];
	state[3][3] ^= col[2];
	state[3][3] ^= gf_mul[col[3]][0];
}

void InvMixColumns(BYTE state[][4])
{
	BYTE col[4];

	// Column 1
	col[0] = state[0][0];
	col[1] = state[1][0];
	col[2] = state[2][0];
	col[3] = state[3][0];
	state[0][0] = gf_mul[col[0]][5];
	state[0][0] ^= gf_mul[col[1]][3];
	state[0][0] ^= gf_mul[col[2]][4];
	state[0][0] ^= gf_mul[col[3]][2];
	state[1][0] = gf_mul[col[0]][2];
	state[1][0] ^= gf_mul[col[1]][5];
	state[1][0] ^= gf_mul[col[2]][3];
	state[1][0] ^= gf_mul[col[3]][4];
	state[2][0] = gf_mul[col[0]][4];
	state[2][0] ^= gf_mul[col[1]][2];
	state[2][0] ^= gf_mul[col[2]][5];
	state[2][0] ^= gf_mul[col[3]][3];
	state[3][0] = gf_mul[col[0]][3];
	state[3][0] ^= gf_mul[col[1]][4];
	state[3][0] ^= gf_mul[col[2]][2];
	state[3][0] ^= gf_mul[col[3]][5];
	// Column 2
	col[0] = state[0][1];
	col[1] = state[1][1];
	col[2] = state[2][1];
	col[3] = state[3][1];
	state[0][1] = gf_mul[col[0]][5];
	state[0][1] ^= gf_mul[col[1]][3];
	state[0][1] ^= gf_mul[col[2]][4];
	state[0][1] ^= gf_mul[col[3]][2];
	state[1][1] = gf_mul[col[0]][2];
	state[1][1] ^= gf_mul[col[1]][5];
	state[1][1] ^= gf_mul[col[2]][3];
	state[1][1] ^= gf_mul[col[3]][4];
	state[2][1] = gf_mul[col[0]][4];
	state[2][1] ^= gf_mul[col[1]][2];
	state[2][1] ^= gf_mul[col[2]][5];
	state[2][1] ^= gf_mul[col[3]][3];
	state[3][1] = gf_mul[col[0]][3];
	state[3][1] ^= gf_mul[col[1]][4];
	state[3][1] ^= gf_mul[col[2]][2];
	state[3][1] ^= gf_mul[col[3]][5];
	// Column 3
	col[0] = state[0][2];
	col[1] = state[1][2];
	col[2] = state[2][2];
	col[3] = state[3][2];
	state[0][2] = gf_mul[col[0]][5];
	state[0][2] ^= gf_mul[col[1]][3];
	state[0][2] ^= gf_mul[col[2]][4];
	state[0][2] ^= gf_mul[col[3]][2];
	state[1][2] = gf_mul[col[0]][2];
	state[1][2] ^= gf_mul[col[1]][5];
	state[1][2] ^= gf_mul[col[2]][3];
	state[1][2] ^= gf_mul[col[3]][4];
	state[2][2] = gf_mul[col[0]][4];
	state[2][2] ^= gf_mul[col[1]][2];
	state[2][2] ^= gf_mul[col[2]][5];
	state[2][2] ^= gf_mul[col[3]][3];
	state[3][2] = gf_mul[col[0]][3];
	state[3][2] ^= gf_mul[col[1]][4];
	state[3][2] ^= gf_mul[col[2]][2];
	state[3][2] ^= gf_mul[col[3]][5];
	// Column 4
	col[0] = state[0][3];
	col[1] = state[1][3];
	col[2] = state[2][3];
	col[3] = state[3][3];
	state[0][3] = gf_mul[col[0]][5];
	state[0][3] ^= gf_mul[col[1]][3];
	state[0][3] ^= gf_mul[col[2]][4];
	state[0][3] ^= gf_mul[col[3]][2];
	state[1][3] = gf_mul[col[0]][2];
	state[1][3] ^= gf_mul[col[1]][5];
	state[1][3] ^= gf_mul[col[2]][3];
	state[1][3] ^= gf_mul[col[3]][4];
	state[2][3] = gf_mul[col[0]][4];
	state[2][3] ^= gf_mul[col[1]][2];
	state[2][3] ^= gf_mul[col[2]][5];
	state[2][3] ^= gf_mul[col[3]][3];
	state[3][3] = gf_mul[col[0]][3];
	state[3][3] ^= gf_mul[col[1]][4];
	state[3][3] ^= gf_mul[col[2]][2];
	state[3][3] ^= gf_mul[col[3]][5];
}

/////////////////
// (En/De)Crypt
/////////////////

void aes_encrypt(const BYTE in[], BYTE out[], const UINT key[], int keysize)
{
	BYTE state[4][4];

	// Copy input array (should be 16 bytes long) to a matrix (sequential bytes are ordered
	// by row, not col) called "state" for processing.
	// *** Implementation note: The official AES documentation references the state by
	// column, then row. Accessing an element in C requires row then column. Thus, all state
	// references in AES must have the column and row indexes reversed for C implementation.
	state[0][0] = in[0];
	state[1][0] = in[1];
	state[2][0] = in[2];
	state[3][0] = in[3];
	state[0][1] = in[4];
	state[1][1] = in[5];
	state[2][1] = in[6];
	state[3][1] = in[7];
	state[0][2] = in[8];
	state[1][2] = in[9];
	state[2][2] = in[10];
	state[3][2] = in[11];
	state[0][3] = in[12];
	state[1][3] = in[13];
	state[2][3] = in[14];
	state[3][3] = in[15];

	// Perform the necessary number of rounds. The round key is added first.
	// The last round does not perform the MixColumns step.
	AddRoundKey(state,&key[0]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[4]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[8]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[12]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[16]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[20]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[24]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[28]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[32]);
	SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[36]);
	if (keysize != 128) {
		SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[40]);
		SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[44]);
		if (keysize != 192) {
			SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[48]);
			SubBytes(state); ShiftRows(state); MixColumns(state); AddRoundKey(state,&key[52]);
			SubBytes(state); ShiftRows(state); AddRoundKey(state,&key[56]);
		}
		else {
			SubBytes(state); ShiftRows(state); AddRoundKey(state,&key[48]);
		}
	}
	else {
		SubBytes(state); ShiftRows(state); AddRoundKey(state,&key[40]);
	}

	// Copy the state to the output array.
	out[0] = state[0][0];
	out[1] = state[1][0];
	out[2] = state[2][0];
	out[3] = state[3][0];
	out[4] = state[0][1];
	out[5] = state[1][1];
	out[6] = state[2][1];
	out[7] = state[3][1];
	out[8] = state[0][2];
	out[9] = state[1][2];
	out[10] = state[2][2];
	out[11] = state[3][2];
	out[12] = state[0][3];
	out[13] = state[1][3];
	out[14] = state[2][3];
	out[15] = state[3][3];
}

void aes_decrypt(const BYTE in[], BYTE out[], const UINT key[], int keysize)
{
	BYTE state[4][4];

	// Copy the input to the state.
	state[0][0] = in[0];
	state[1][0] = in[1];
	state[2][0] = in[2];
	state[3][0] = in[3];
	state[0][1] = in[4];
	state[1][1] = in[5];
	state[2][1] = in[6];
	state[3][1] = in[7];
	state[0][2] = in[8];
	state[1][2] = in[9];
	state[2][2] = in[10];
	state[3][2] = in[11];
	state[0][3] = in[12];
	state[1][3] = in[13];
	state[2][3] = in[14];
	state[3][3] = in[15];

	// Perform the necessary number of rounds. The round key is added first.
	// The last round does not perform the MixColumns step.
	if (keysize > 128) {
		if (keysize > 192) {
			AddRoundKey(state,&key[56]);
			InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[52]);InvMixColumns(state);
			InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[48]);InvMixColumns(state);
		}
		else {
			AddRoundKey(state,&key[48]);
		}
		InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[44]);InvMixColumns(state);
		InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[40]);InvMixColumns(state);
	}
	else {
		AddRoundKey(state,&key[40]);
	}
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[36]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[32]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[28]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[24]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[20]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[16]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[12]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[8]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[4]);InvMixColumns(state);
	InvShiftRows(state);InvSubBytes(state);AddRoundKey(state,&key[0]);

	// Copy the state to the output array.
	out[0] = state[0][0];
	out[1] = state[1][0];
	out[2] = state[2][0];
	out[3] = state[3][0];
	out[4] = state[0][1];
	out[5] = state[1][1];
	out[6] = state[2][1];
	out[7] = state[3][1];
	out[8] = state[0][2];
	out[9] = state[1][2];
	out[10] = state[2][2];
	out[11] = state[3][2];
	out[12] = state[0][3];
	out[13] = state[1][3];
	out[14] = state[2][3];
	out[15] = state[3][3];
}

// -------------------------------------------------- DES -------------------------------------------------- //

/****************************** MACROS ******************************/
// Obtain bit "b" from the left and shift it "c" places from the right
#define BITNUM(a,b,c) (((a[(b)/8] >> (7 - (b%8))) & 0x01) << (c))
#define BITNUMINTR(a,b,c) ((((a) >> (31 - (b))) & 0x00000001) << (c))
#define BITNUMINTL(a,b,c) ((((a) << (b)) & 0x80000000) >> (c))

// This macro converts a 6 bit block with the S-Box row defined as the first and last
// bits to a 6 bit block with the row defined by the first two bits.
#define SBOXBIT(a) (((a) & 0x20) | (((a) & 0x1f) >> 1) | (((a) & 0x01) << 4))

/**************************** VARIABLES *****************************/
static const BYTE sbox1[64] = {
	14,  4,  13,  1,   2, 15,  11,  8,   3, 10,   6, 12,   5,  9,   0,  7,
	0, 15,   7,  4,  14,  2,  13,  1,  10,  6,  12, 11,   9,  5,   3,  8,
	4,  1,  14,  8,  13,  6,   2, 11,  15, 12,   9,  7,   3, 10,   5,  0,
	15, 12,   8,  2,   4,  9,   1,  7,   5, 11,   3, 14,  10,  0,   6, 13
};
static const BYTE sbox2[64] = {
	15,  1,   8, 14,   6, 11,   3,  4,   9,  7,   2, 13,  12,  0,   5, 10,
	3, 13,   4,  7,  15,  2,   8, 14,  12,  0,   1, 10,   6,  9,  11,  5,
	0, 14,   7, 11,  10,  4,  13,  1,   5,  8,  12,  6,   9,  3,   2, 15,
	13,  8,  10,  1,   3, 15,   4,  2,  11,  6,   7, 12,   0,  5,  14,  9
};
static const BYTE sbox3[64] = {
	10,  0,   9, 14,   6,  3,  15,  5,   1, 13,  12,  7,  11,  4,   2,  8,
	13,  7,   0,  9,   3,  4,   6, 10,   2,  8,   5, 14,  12, 11,  15,  1,
	13,  6,   4,  9,   8, 15,   3,  0,  11,  1,   2, 12,   5, 10,  14,  7,
	1, 10,  13,  0,   6,  9,   8,  7,   4, 15,  14,  3,  11,  5,   2, 12
};
static const BYTE sbox4[64] = {
	7, 13,  14,  3,   0,  6,   9, 10,   1,  2,   8,  5,  11, 12,   4, 15,
	13,  8,  11,  5,   6, 15,   0,  3,   4,  7,   2, 12,   1, 10,  14,  9,
	10,  6,   9,  0,  12, 11,   7, 13,  15,  1,   3, 14,   5,  2,   8,  4,
	3, 15,   0,  6,  10,  1,  13,  8,   9,  4,   5, 11,  12,  7,   2, 14
};
static const BYTE sbox5[64] = {
	2, 12,   4,  1,   7, 10,  11,  6,   8,  5,   3, 15,  13,  0,  14,  9,
	14, 11,   2, 12,   4,  7,  13,  1,   5,  0,  15, 10,   3,  9,   8,  6,
	4,  2,   1, 11,  10, 13,   7,  8,  15,  9,  12,  5,   6,  3,   0, 14,
	11,  8,  12,  7,   1, 14,   2, 13,   6, 15,   0,  9,  10,  4,   5,  3
};
static const BYTE sbox6[64] = {
	12,  1,  10, 15,   9,  2,   6,  8,   0, 13,   3,  4,  14,  7,   5, 11,
	10, 15,   4,  2,   7, 12,   9,  5,   6,  1,  13, 14,   0, 11,   3,  8,
	9, 14,  15,  5,   2,  8,  12,  3,   7,  0,   4, 10,   1, 13,  11,  6,
	4,  3,   2, 12,   9,  5,  15, 10,  11, 14,   1,  7,   6,  0,   8, 13
};
static const BYTE sbox7[64] = {
	4, 11,   2, 14,  15,  0,   8, 13,   3, 12,   9,  7,   5, 10,   6,  1,
	13,  0,  11,  7,   4,  9,   1, 10,  14,  3,   5, 12,   2, 15,   8,  6,
	1,  4,  11, 13,  12,  3,   7, 14,  10, 15,   6,  8,   0,  5,   9,  2,
	6, 11,  13,  8,   1,  4,  10,  7,   9,  5,   0, 15,  14,  2,   3, 12
};
static const BYTE sbox8[64] = {
	13,  2,   8,  4,   6, 15,  11,  1,  10,  9,   3, 14,   5,  0,  12,  7,
	1, 15,  13,  8,  10,  3,   7,  4,  12,  5,   6, 11,   0, 14,   9,  2,
	7, 11,   4,  1,   9, 12,  14,  2,   0,  6,  10, 13,  15,  3,   5,  8,
	2,  1,  14,  7,   4, 10,   8, 13,  15, 12,   9,  0,   3,  5,   6, 11
};

/*********************** FUNCTION DEFINITIONS ***********************/
// Initial (Inv)Permutation step
void IP(UINT state[], const BYTE in[])
{
	state[0] = BITNUM(in,57,31) | BITNUM(in,49,30) | BITNUM(in,41,29) | BITNUM(in,33,28) |
		BITNUM(in,25,27) | BITNUM(in,17,26) | BITNUM(in,9,25) | BITNUM(in,1,24) |
		BITNUM(in,59,23) | BITNUM(in,51,22) | BITNUM(in,43,21) | BITNUM(in,35,20) |
		BITNUM(in,27,19) | BITNUM(in,19,18) | BITNUM(in,11,17) | BITNUM(in,3,16) |
		BITNUM(in,61,15) | BITNUM(in,53,14) | BITNUM(in,45,13) | BITNUM(in,37,12) |
		BITNUM(in,29,11) | BITNUM(in,21,10) | BITNUM(in,13,9) | BITNUM(in,5,8) |
		BITNUM(in,63,7) | BITNUM(in,55,6) | BITNUM(in,47,5) | BITNUM(in,39,4) |
		BITNUM(in,31,3) | BITNUM(in,23,2) | BITNUM(in,15,1) | BITNUM(in,7,0);

	state[1] = BITNUM(in,56,31) | BITNUM(in,48,30) | BITNUM(in,40,29) | BITNUM(in,32,28) |
		BITNUM(in,24,27) | BITNUM(in,16,26) | BITNUM(in,8,25) | BITNUM(in,0,24) |
		BITNUM(in,58,23) | BITNUM(in,50,22) | BITNUM(in,42,21) | BITNUM(in,34,20) |
		BITNUM(in,26,19) | BITNUM(in,18,18) | BITNUM(in,10,17) | BITNUM(in,2,16) |
		BITNUM(in,60,15) | BITNUM(in,52,14) | BITNUM(in,44,13) | BITNUM(in,36,12) |
		BITNUM(in,28,11) | BITNUM(in,20,10) | BITNUM(in,12,9) | BITNUM(in,4,8) |
		BITNUM(in,62,7) | BITNUM(in,54,6) | BITNUM(in,46,5) | BITNUM(in,38,4) |
		BITNUM(in,30,3) | BITNUM(in,22,2) | BITNUM(in,14,1) | BITNUM(in,6,0);
}

void InvIP(UINT state[], BYTE in[])
{
	in[0] = BITNUMINTR(state[1],7,7) | BITNUMINTR(state[0],7,6) | BITNUMINTR(state[1],15,5) |
		BITNUMINTR(state[0],15,4) | BITNUMINTR(state[1],23,3) | BITNUMINTR(state[0],23,2) |
		BITNUMINTR(state[1],31,1) | BITNUMINTR(state[0],31,0);

	in[1] = BITNUMINTR(state[1],6,7) | BITNUMINTR(state[0],6,6) | BITNUMINTR(state[1],14,5) |
		BITNUMINTR(state[0],14,4) | BITNUMINTR(state[1],22,3) | BITNUMINTR(state[0],22,2) |
		BITNUMINTR(state[1],30,1) | BITNUMINTR(state[0],30,0);

	in[2] = BITNUMINTR(state[1],5,7) | BITNUMINTR(state[0],5,6) | BITNUMINTR(state[1],13,5) |
		BITNUMINTR(state[0],13,4) | BITNUMINTR(state[1],21,3) | BITNUMINTR(state[0],21,2) |
		BITNUMINTR(state[1],29,1) | BITNUMINTR(state[0],29,0);

	in[3] = BITNUMINTR(state[1],4,7) | BITNUMINTR(state[0],4,6) | BITNUMINTR(state[1],12,5) |
		BITNUMINTR(state[0],12,4) | BITNUMINTR(state[1],20,3) | BITNUMINTR(state[0],20,2) |
		BITNUMINTR(state[1],28,1) | BITNUMINTR(state[0],28,0);

	in[4] = BITNUMINTR(state[1],3,7) | BITNUMINTR(state[0],3,6) | BITNUMINTR(state[1],11,5) |
		BITNUMINTR(state[0],11,4) | BITNUMINTR(state[1],19,3) | BITNUMINTR(state[0],19,2) |
		BITNUMINTR(state[1],27,1) | BITNUMINTR(state[0],27,0);

	in[5] = BITNUMINTR(state[1],2,7) | BITNUMINTR(state[0],2,6) | BITNUMINTR(state[1],10,5) |
		BITNUMINTR(state[0],10,4) | BITNUMINTR(state[1],18,3) | BITNUMINTR(state[0],18,2) |
		BITNUMINTR(state[1],26,1) | BITNUMINTR(state[0],26,0);

	in[6] = BITNUMINTR(state[1],1,7) | BITNUMINTR(state[0],1,6) | BITNUMINTR(state[1],9,5) |
		BITNUMINTR(state[0],9,4) | BITNUMINTR(state[1],17,3) | BITNUMINTR(state[0],17,2) |
		BITNUMINTR(state[1],25,1) | BITNUMINTR(state[0],25,0);

	in[7] = BITNUMINTR(state[1],0,7) | BITNUMINTR(state[0],0,6) | BITNUMINTR(state[1],8,5) |
		BITNUMINTR(state[0],8,4) | BITNUMINTR(state[1],16,3) | BITNUMINTR(state[0],16,2) |
		BITNUMINTR(state[1],24,1) | BITNUMINTR(state[0],24,0);
}

UINT f(UINT state, const BYTE key[])
{
	BYTE lrgstate[6]; //,i;
	UINT t1,t2;

	// Expantion Permutation
	t1 = BITNUMINTL(state,31,0) | ((state & 0xf0000000) >> 1) | BITNUMINTL(state,4,5) |
		BITNUMINTL(state,3,6) | ((state & 0x0f000000) >> 3) | BITNUMINTL(state,8,11) |
		BITNUMINTL(state,7,12) | ((state & 0x00f00000) >> 5) | BITNUMINTL(state,12,17) |
		BITNUMINTL(state,11,18) | ((state & 0x000f0000) >> 7) | BITNUMINTL(state,16,23);

	t2 = BITNUMINTL(state,15,0) | ((state & 0x0000f000) << 15) | BITNUMINTL(state,20,5) |
		BITNUMINTL(state,19,6) | ((state & 0x00000f00) << 13) | BITNUMINTL(state,24,11) |
		BITNUMINTL(state,23,12) | ((state & 0x000000f0) << 11) | BITNUMINTL(state,28,17) |
		BITNUMINTL(state,27,18) | ((state & 0x0000000f) << 9) | BITNUMINTL(state,0,23);

	lrgstate[0] = (t1 >> 24) & 0x000000ff;
	lrgstate[1] = (t1 >> 16) & 0x000000ff;
	lrgstate[2] = (t1 >> 8) & 0x000000ff;
	lrgstate[3] = (t2 >> 24) & 0x000000ff;
	lrgstate[4] = (t2 >> 16) & 0x000000ff;
	lrgstate[5] = (t2 >> 8) & 0x000000ff;

	// Key XOR
	lrgstate[0] ^= key[0];
	lrgstate[1] ^= key[1];
	lrgstate[2] ^= key[2];
	lrgstate[3] ^= key[3];
	lrgstate[4] ^= key[4];
	lrgstate[5] ^= key[5];

	// S-Box Permutation
	state = (sbox1[SBOXBIT(lrgstate[0] >> 2)] << 28) |
		(sbox2[SBOXBIT(((lrgstate[0] & 0x03) << 4) | (lrgstate[1] >> 4))] << 24) |
		(sbox3[SBOXBIT(((lrgstate[1] & 0x0f) << 2) | (lrgstate[2] >> 6))] << 20) |
		(sbox4[SBOXBIT(lrgstate[2] & 0x3f)] << 16) |
		(sbox5[SBOXBIT(lrgstate[3] >> 2)] << 12) |
		(sbox6[SBOXBIT(((lrgstate[3] & 0x03) << 4) | (lrgstate[4] >> 4))] << 8) |
		(sbox7[SBOXBIT(((lrgstate[4] & 0x0f) << 2) | (lrgstate[5] >> 6))] << 4) |
		sbox8[SBOXBIT(lrgstate[5] & 0x3f)];

	// P-Box Permutation
	state = BITNUMINTL(state,15,0) | BITNUMINTL(state,6,1) | BITNUMINTL(state,19,2) |
		BITNUMINTL(state,20,3) | BITNUMINTL(state,28,4) | BITNUMINTL(state,11,5) |
		BITNUMINTL(state,27,6) | BITNUMINTL(state,16,7) | BITNUMINTL(state,0,8) |
		BITNUMINTL(state,14,9) | BITNUMINTL(state,22,10) | BITNUMINTL(state,25,11) |
		BITNUMINTL(state,4,12) | BITNUMINTL(state,17,13) | BITNUMINTL(state,30,14) |
		BITNUMINTL(state,9,15) | BITNUMINTL(state,1,16) | BITNUMINTL(state,7,17) |
		BITNUMINTL(state,23,18) | BITNUMINTL(state,13,19) | BITNUMINTL(state,31,20) |
		BITNUMINTL(state,26,21) | BITNUMINTL(state,2,22) | BITNUMINTL(state,8,23) |
		BITNUMINTL(state,18,24) | BITNUMINTL(state,12,25) | BITNUMINTL(state,29,26) |
		BITNUMINTL(state,5,27) | BITNUMINTL(state,21,28) | BITNUMINTL(state,10,29) |
		BITNUMINTL(state,3,30) | BITNUMINTL(state,24,31);

	// Return the final state value
	return(state);
}

void des_key_setup(const BYTE key[], BYTE schedule[][6], DES_MODE mode)
{
	UINT i, j, to_gen, C, D;
	const UINT key_rnd_shift[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
	const UINT key_perm_c[28] = {56,48,40,32,24,16,8,0,57,49,41,33,25,17,
		9,1,58,50,42,34,26,18,10,2,59,51,43,35};
	const UINT key_perm_d[28] = {62,54,46,38,30,22,14,6,61,53,45,37,29,21,
		13,5,60,52,44,36,28,20,12,4,27,19,11,3};
	const UINT key_compression[48] = {13,16,10,23,0,4,2,27,14,5,20,9,
		22,18,11,3,25,7,15,6,26,19,12,1,
		40,51,30,36,46,54,29,39,50,44,32,47,
		43,48,38,55,33,52,45,41,49,35,28,31};

	// Permutated Choice #1 (copy the key in, ignoring parity bits).
	for (i = 0, j = 31, C = 0; i < 28; ++i, --j)
		C |= BITNUM(key,key_perm_c[i],j);
	for (i = 0, j = 31, D = 0; i < 28; ++i, --j)
		D |= BITNUM(key,key_perm_d[i],j);

	// Generate the 16 subkeys.
	for (i = 0; i < 16; ++i) {
		C = ((C << key_rnd_shift[i]) | (C >> (28-key_rnd_shift[i]))) & 0xfffffff0;
		D = ((D << key_rnd_shift[i]) | (D >> (28-key_rnd_shift[i]))) & 0xfffffff0;

		// Decryption subkeys are reverse order of encryption subkeys so
		// generate them in reverse if the key schedule is for decryption useage.
		if (mode == DES_DECRYPT)
			to_gen = 15 - i;
		else /*(if mode == DES_ENCRYPT)*/
			to_gen = i;
		// Initialize the array
		for (j = 0; j < 6; ++j)
			schedule[to_gen][j] = 0;
		for (j = 0; j < 24; ++j)
			schedule[to_gen][j/8] |= BITNUMINTR(C,key_compression[j],7 - (j%8));
		for ( ; j < 48; ++j)
			schedule[to_gen][j/8] |= BITNUMINTR(D,key_compression[j] - 28,7 - (j%8));
	}
}

void des_crypt(const BYTE in[], BYTE out[], const BYTE key[][6])
{
	UINT state[2],idx,t;

	IP(state,in);

	for (idx=0; idx < 15; ++idx) {
		t = state[1];
		state[1] = f(state[1],key[idx]) ^ state[0];
		state[0] = t;
	}
	// Perform the final loop manually as it doesn't switch sides
	state[0] = f(state[1],key[15]) ^ state[0];

	InvIP(state,out);
}

void three_des_key_setup(const BYTE key[], BYTE schedule[][16][6], DES_MODE mode)
{
	if (mode == DES_ENCRYPT) {
		des_key_setup(&key[0],schedule[0],mode);
		des_key_setup(&key[8],schedule[1],(DES_MODE)(!mode));
		des_key_setup(&key[16],schedule[2],mode);
	}
	else /*if (mode == DES_DECRYPT*/ {
		des_key_setup(&key[16],schedule[0],mode);
		des_key_setup(&key[8],schedule[1],(DES_MODE)(!mode));
		des_key_setup(&key[0],schedule[2],mode);
	}
}

void three_des_crypt(const BYTE in[], BYTE out[], const BYTE key[][16][6])
{
	des_crypt(in,out,key[0]);
	des_crypt(out,out,key[1]);
	des_crypt(out,out,key[2]);
}

// -------------------------------------------------- MD2 -------------------------------------------------- //

/**************************** VARIABLES *****************************/
static const BYTE s[256] = {
	41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
	19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
	76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
	138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
	245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
	148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
	39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
	181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
	150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
	112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
	96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
	85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
	234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
	129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
	8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
	203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
	166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
	31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

/*********************** FUNCTION DEFINITIONS ***********************/
void md2_transform(_MD2_CTX *ctx, BYTE data[])
{
	int j,k,t;

	//memcpy(&ctx->state[16], data);
	for (j=0; j < 16; ++j) {
		ctx->state[j + 16] = data[j];
		ctx->state[j + 32] = (ctx->state[j+16] ^ ctx->state[j]);
	}

	t = 0;
	for (j = 0; j < 18; ++j) {
		for (k = 0; k < 48; ++k) {
			ctx->state[k] ^= s[t];
			t = ctx->state[k];
		}
		t = (t+j) & 0xFF;
	}

	t = ctx->checksum[15];
	for (j=0; j < 16; ++j) {
		ctx->checksum[j] ^= s[data[j] ^ t];
		t = ctx->checksum[j];
	}
}

void md2_init(_MD2_CTX *ctx)
{
	int i;

	for (i=0; i < 48; ++i)
		ctx->state[i] = 0;
	for (i=0; i < 16; ++i)
		ctx->checksum[i] = 0;
	ctx->len = 0;
}

void md2_update(_MD2_CTX *ctx, const BYTE data[], size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->len] = data[i];
		ctx->len++;
		if (ctx->len == MD2_BLOCK_SIZE) {
			md2_transform(ctx, ctx->data);
			ctx->len = 0;
		}
	}
}

void md2_final(_MD2_CTX *ctx, BYTE hash[])
{
	int to_pad;

	to_pad = MD2_BLOCK_SIZE - ctx->len;

	while (ctx->len < MD2_BLOCK_SIZE)
		ctx->data[ctx->len++] = to_pad;

	md2_transform(ctx, ctx->data);
	md2_transform(ctx, ctx->checksum);

	memcpy(hash, ctx->state, MD2_BLOCK_SIZE);
}

// -------------------------------------------------- MD5 -------------------------------------------------- //

/****************************** MACROS ******************************/

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^ (x | ~z))

#define FF(a,b,c,d,m,s,t) { a += F(b,c,d) + m + t; \
	a = b + ROTLEFT(a,s); }
#define GG(a,b,c,d,m,s,t) { a += G(b,c,d) + m + t; \
	a = b + ROTLEFT(a,s); }
#define HH(a,b,c,d,m,s,t) { a += H(b,c,d) + m + t; \
	a = b + ROTLEFT(a,s); }
#define II(a,b,c,d,m,s,t) { a += I(b,c,d) + m + t; \
	a = b + ROTLEFT(a,s); }

/*********************** FUNCTION DEFINITIONS ***********************/
void md5_transform(_MD5_CTX *ctx, const BYTE data[])
{
	UINT a, b, c, d, m[16], i, j;

	// MD5 specifies big endian byte order, but this implementation assumes a little
	// endian byte order CPU. Reverse all the bytes upon input, and re-reverse them
	// on output (in md5_final()).
	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j]) + (data[j + 1] << 8) + (data[j + 2] << 16) + (data[j + 3] << 24);

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	FF(a,b,c,d,m[0],  7,0xd76aa478);
	FF(d,a,b,c,m[1], 12,0xe8c7b756);
	FF(c,d,a,b,m[2], 17,0x242070db);
	FF(b,c,d,a,m[3], 22,0xc1bdceee);
	FF(a,b,c,d,m[4],  7,0xf57c0faf);
	FF(d,a,b,c,m[5], 12,0x4787c62a);
	FF(c,d,a,b,m[6], 17,0xa8304613);
	FF(b,c,d,a,m[7], 22,0xfd469501);
	FF(a,b,c,d,m[8],  7,0x698098d8);
	FF(d,a,b,c,m[9], 12,0x8b44f7af);
	FF(c,d,a,b,m[10],17,0xffff5bb1);
	FF(b,c,d,a,m[11],22,0x895cd7be);
	FF(a,b,c,d,m[12], 7,0x6b901122);
	FF(d,a,b,c,m[13],12,0xfd987193);
	FF(c,d,a,b,m[14],17,0xa679438e);
	FF(b,c,d,a,m[15],22,0x49b40821);

	GG(a,b,c,d,m[1],  5,0xf61e2562);
	GG(d,a,b,c,m[6],  9,0xc040b340);
	GG(c,d,a,b,m[11],14,0x265e5a51);
	GG(b,c,d,a,m[0], 20,0xe9b6c7aa);
	GG(a,b,c,d,m[5],  5,0xd62f105d);
	GG(d,a,b,c,m[10], 9,0x02441453);
	GG(c,d,a,b,m[15],14,0xd8a1e681);
	GG(b,c,d,a,m[4], 20,0xe7d3fbc8);
	GG(a,b,c,d,m[9],  5,0x21e1cde6);
	GG(d,a,b,c,m[14], 9,0xc33707d6);
	GG(c,d,a,b,m[3], 14,0xf4d50d87);
	GG(b,c,d,a,m[8], 20,0x455a14ed);
	GG(a,b,c,d,m[13], 5,0xa9e3e905);
	GG(d,a,b,c,m[2],  9,0xfcefa3f8);
	GG(c,d,a,b,m[7], 14,0x676f02d9);
	GG(b,c,d,a,m[12],20,0x8d2a4c8a);

	HH(a,b,c,d,m[5],  4,0xfffa3942);
	HH(d,a,b,c,m[8], 11,0x8771f681);
	HH(c,d,a,b,m[11],16,0x6d9d6122);
	HH(b,c,d,a,m[14],23,0xfde5380c);
	HH(a,b,c,d,m[1],  4,0xa4beea44);
	HH(d,a,b,c,m[4], 11,0x4bdecfa9);
	HH(c,d,a,b,m[7], 16,0xf6bb4b60);
	HH(b,c,d,a,m[10],23,0xbebfbc70);
	HH(a,b,c,d,m[13], 4,0x289b7ec6);
	HH(d,a,b,c,m[0], 11,0xeaa127fa);
	HH(c,d,a,b,m[3], 16,0xd4ef3085);
	HH(b,c,d,a,m[6], 23,0x04881d05);
	HH(a,b,c,d,m[9],  4,0xd9d4d039);
	HH(d,a,b,c,m[12],11,0xe6db99e5);
	HH(c,d,a,b,m[15],16,0x1fa27cf8);
	HH(b,c,d,a,m[2], 23,0xc4ac5665);

	II(a,b,c,d,m[0],  6,0xf4292244);
	II(d,a,b,c,m[7], 10,0x432aff97);
	II(c,d,a,b,m[14],15,0xab9423a7);
	II(b,c,d,a,m[5], 21,0xfc93a039);
	II(a,b,c,d,m[12], 6,0x655b59c3);
	II(d,a,b,c,m[3], 10,0x8f0ccc92);
	II(c,d,a,b,m[10],15,0xffeff47d);
	II(b,c,d,a,m[1], 21,0x85845dd1);
	II(a,b,c,d,m[8],  6,0x6fa87e4f);
	II(d,a,b,c,m[15],10,0xfe2ce6e0);
	II(c,d,a,b,m[6], 15,0xa3014314);
	II(b,c,d,a,m[13],21,0x4e0811a1);
	II(a,b,c,d,m[4],  6,0xf7537e82);
	II(d,a,b,c,m[11],10,0xbd3af235);
	II(c,d,a,b,m[2], 15,0x2ad7d2bb);
	II(b,c,d,a,m[9], 21,0xeb86d391);

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
}

void md5_init(_MD5_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
}

void md5_update(_MD5_CTX *ctx, const BYTE data[], size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			md5_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void md5_final(_MD5_CTX *ctx, BYTE hash[])
{
	size_t i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else if (ctx->datalen >= 56) {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		md5_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[56] = (BYTE)(ctx->bitlen);
	ctx->data[57] = (BYTE)(ctx->bitlen >> 8);
	ctx->data[58] = (BYTE)(ctx->bitlen >> 16);
	ctx->data[59] = (BYTE)(ctx->bitlen >> 24);
	ctx->data[60] = (BYTE)(ctx->bitlen >> 32);
	ctx->data[61] = (BYTE)(ctx->bitlen >> 40);
	ctx->data[62] = (BYTE)(ctx->bitlen >> 48);
	ctx->data[63] = (BYTE)(ctx->bitlen >> 56);
	md5_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and MD uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i]      = (ctx->state[0] >> (i * 8)) & 0x000000ff;
		hash[i + 4]  = (ctx->state[1] >> (i * 8)) & 0x000000ff;
		hash[i + 8]  = (ctx->state[2] >> (i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (i * 8)) & 0x000000ff;
	}
}


// -------------------------------------------------- SHA1 -------------------------------------------------- //

/****************************** MACROS ******************************/


/*********************** FUNCTION DEFINITIONS ***********************/
void sha1_transform(_SHA1_CTX *ctx, const BYTE data[])
{
	UINT a, b, c, d, e, i, j, t, m[80];

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) + (data[j + 1] << 16) + (data[j + 2] << 8) + (data[j + 3]);
	for ( ; i < 80; ++i) {
		m[i] = (m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16]);
		m[i] = (m[i] << 1) | (m[i] >> 31);
	}

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];

	for (i = 0; i < 20; ++i) {
		t = ROTLEFT(a, 5) + ((b & c) ^ (~b & d)) + e + ctx->k[0] + m[i];
		e = d;
		d = c;
		c = ROTLEFT(b, 30);
		b = a;
		a = t;
	}
	for ( ; i < 40; ++i) {
		t = ROTLEFT(a, 5) + (b ^ c ^ d) + e + ctx->k[1] + m[i];
		e = d;
		d = c;
		c = ROTLEFT(b, 30);
		b = a;
		a = t;
	}
	for ( ; i < 60; ++i) {
		t = ROTLEFT(a, 5) + ((b & c) ^ (b & d) ^ (c & d))  + e + ctx->k[2] + m[i];
		e = d;
		d = c;
		c = ROTLEFT(b, 30);
		b = a;
		a = t;
	}
	for ( ; i < 80; ++i) {
		t = ROTLEFT(a, 5) + (b ^ c ^ d) + e + ctx->k[3] + m[i];
		e = d;
		d = c;
		c = ROTLEFT(b, 30);
		b = a;
		a = t;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
}

void sha1_init(_SHA1_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
	ctx->k[0] = 0x5a827999;
	ctx->k[1] = 0x6ed9eba1;
	ctx->k[2] = 0x8f1bbcdc;
	ctx->k[3] = 0xca62c1d6;
}

void sha1_update(_SHA1_CTX *ctx, const BYTE data[], size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			sha1_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void sha1_final(_SHA1_CTX *ctx, BYTE hash[])
{
	UINT i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha1_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = (BYTE)(ctx->bitlen);
	ctx->data[62] = (BYTE)(ctx->bitlen >> 8);
	ctx->data[61] = (BYTE)(ctx->bitlen >> 16);
	ctx->data[60] = (BYTE)(ctx->bitlen >> 24);
	ctx->data[59] = (BYTE)(ctx->bitlen >> 32);
	ctx->data[58] = (BYTE)(ctx->bitlen >> 40);
	ctx->data[57] = (BYTE)(ctx->bitlen >> 48);
	ctx->data[56] = (BYTE)(ctx->bitlen >> 56);
	sha1_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and MD uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
	}
}

// -------------------------------------------------- SHA256 -------------------------------------------------- //

/****************************** MACROS ******************************/

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/**************************** VARIABLES *****************************/
static const UINT k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/*********************** FUNCTION DEFINITIONS ***********************/
void sha256_transform(_SHA256_CTX *ctx, const BYTE data[])
{
	UINT a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	for ( ; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
		t2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void sha256_init(_SHA256_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_update(_SHA256_CTX *ctx, const BYTE data[], size_t len)
{
	UINT i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void sha256_final(_SHA256_CTX *ctx, BYTE hash[])
{
	UINT i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = (BYTE)(ctx->bitlen);
	ctx->data[62] = (BYTE)(ctx->bitlen >> 8);
	ctx->data[61] = (BYTE)(ctx->bitlen >> 16);
	ctx->data[60] = (BYTE)(ctx->bitlen >> 24);
	ctx->data[59] = (BYTE)(ctx->bitlen >> 32);
	ctx->data[58] = (BYTE)(ctx->bitlen >> 40);
	ctx->data[57] = (BYTE)(ctx->bitlen >> 48);
	ctx->data[56] = (BYTE)(ctx->bitlen >> 56);
	sha256_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
}

// -------------------------------------------------- ARCFOUR -------------------------------------------------- //

/*********************** FUNCTION DEFINITIONS ***********************/
void arcfour_key_setup(BYTE state[], const BYTE key[], int len)
{
	int i, j;
	BYTE t;

	for (i = 0; i < 256; ++i)
		state[i] = i;
	for (i = 0, j = 0; i < 256; ++i) {
		j = (j + state[i] + key[i % len]) % 256;
		t = state[i];
		state[i] = state[j];
		state[j] = t;
	}
}

// This does not hold state between calls. It always generates the
// stream starting from the first  output byte.
void arcfour_generate_stream(BYTE state[], BYTE out[], size_t len)
{
	int i, j;
	size_t idx;
	BYTE t;

	for (idx = 0, i = 0, j = 0; idx < len; ++idx)  {
		i = (i + 1) % 256;
		j = (j + state[i]) % 256;
		t = state[i];
		state[i] = state[j];
		state[j] = t;
		out[idx] = state[(state[i] + state[j]) % 256];
	}
}

// -------------------------------------------------- BLOWFISH -------------------------------------------------- //

/****************************** MACROS ******************************/
#define BF(x,t)	t = keystruct->s[0][(x) >> 24]; \
				t += keystruct->s[1][((x) >> 16) & 0xff]; \
				t ^= keystruct->s[2][((x) >> 8) & 0xff]; \
				t += keystruct->s[3][(x) & 0xff];
#define swap(r,l,t) t = l; l = r; r = t;
#define ITERATION(l,r,t,pval) l ^= keystruct->p[pval]; BF(l,t); r^= t; swap(r,l,t);

/**************************** VARIABLES *****************************/
static const UINT p_perm[18] = {
	0x243F6A88,0x85A308D3,0x13198A2E,0x03707344,0xA4093822,0x299F31D0,0x082EFA98,
	0xEC4E6C89,0x452821E6,0x38D01377,0xBE5466CF,0x34E90C6C,0xC0AC29B7,0xC97C50DD,
	0x3F84D5B5,0xB5470917,0x9216D5D9,0x8979FB1B
};

static const UINT s_perm[4][256] = { {
	0xD1310BA6,0x98DFB5AC,0x2FFD72DB,0xD01ADFB7,0xB8E1AFED,0x6A267E96,0xBA7C9045,0xF12C7F99,
		0x24A19947,0xB3916CF7,0x0801F2E2,0x858EFC16,0x636920D8,0x71574E69,0xA458FEA3,0xF4933D7E,
		0x0D95748F,0x728EB658,0x718BCD58,0x82154AEE,0x7B54A41D,0xC25A59B5,0x9C30D539,0x2AF26013,
		0xC5D1B023,0x286085F0,0xCA417918,0xB8DB38EF,0x8E79DCB0,0x603A180E,0x6C9E0E8B,0xB01E8A3E,
		0xD71577C1,0xBD314B27,0x78AF2FDA,0x55605C60,0xE65525F3,0xAA55AB94,0x57489862,0x63E81440,
		0x55CA396A,0x2AAB10B6,0xB4CC5C34,0x1141E8CE,0xA15486AF,0x7C72E993,0xB3EE1411,0x636FBC2A,
		0x2BA9C55D,0x741831F6,0xCE5C3E16,0x9B87931E,0xAFD6BA33,0x6C24CF5C,0x7A325381,0x28958677,
		0x3B8F4898,0x6B4BB9AF,0xC4BFE81B,0x66282193,0x61D809CC,0xFB21A991,0x487CAC60,0x5DEC8032,
		0xEF845D5D,0xE98575B1,0xDC262302,0xEB651B88,0x23893E81,0xD396ACC5,0x0F6D6FF3,0x83F44239,
		0x2E0B4482,0xA4842004,0x69C8F04A,0x9E1F9B5E,0x21C66842,0xF6E96C9A,0x670C9C61,0xABD388F0,
		0x6A51A0D2,0xD8542F68,0x960FA728,0xAB5133A3,0x6EEF0B6C,0x137A3BE4,0xBA3BF050,0x7EFB2A98,
		0xA1F1651D,0x39AF0176,0x66CA593E,0x82430E88,0x8CEE8619,0x456F9FB4,0x7D84A5C3,0x3B8B5EBE,
		0xE06F75D8,0x85C12073,0x401A449F,0x56C16AA6,0x4ED3AA62,0x363F7706,0x1BFEDF72,0x429B023D,
		0x37D0D724,0xD00A1248,0xDB0FEAD3,0x49F1C09B,0x075372C9,0x80991B7B,0x25D479D8,0xF6E8DEF7,
		0xE3FE501A,0xB6794C3B,0x976CE0BD,0x04C006BA,0xC1A94FB6,0x409F60C4,0x5E5C9EC2,0x196A2463,
		0x68FB6FAF,0x3E6C53B5,0x1339B2EB,0x3B52EC6F,0x6DFC511F,0x9B30952C,0xCC814544,0xAF5EBD09,
		0xBEE3D004,0xDE334AFD,0x660F2807,0x192E4BB3,0xC0CBA857,0x45C8740F,0xD20B5F39,0xB9D3FBDB,
		0x5579C0BD,0x1A60320A,0xD6A100C6,0x402C7279,0x679F25FE,0xFB1FA3CC,0x8EA5E9F8,0xDB3222F8,
		0x3C7516DF,0xFD616B15,0x2F501EC8,0xAD0552AB,0x323DB5FA,0xFD238760,0x53317B48,0x3E00DF82,
		0x9E5C57BB,0xCA6F8CA0,0x1A87562E,0xDF1769DB,0xD542A8F6,0x287EFFC3,0xAC6732C6,0x8C4F5573,
		0x695B27B0,0xBBCA58C8,0xE1FFA35D,0xB8F011A0,0x10FA3D98,0xFD2183B8,0x4AFCB56C,0x2DD1D35B,
		0x9A53E479,0xB6F84565,0xD28E49BC,0x4BFB9790,0xE1DDF2DA,0xA4CB7E33,0x62FB1341,0xCEE4C6E8,
		0xEF20CADA,0x36774C01,0xD07E9EFE,0x2BF11FB4,0x95DBDA4D,0xAE909198,0xEAAD8E71,0x6B93D5A0,
		0xD08ED1D0,0xAFC725E0,0x8E3C5B2F,0x8E7594B7,0x8FF6E2FB,0xF2122B64,0x8888B812,0x900DF01C,
		0x4FAD5EA0,0x688FC31C,0xD1CFF191,0xB3A8C1AD,0x2F2F2218,0xBE0E1777,0xEA752DFE,0x8B021FA1,
		0xE5A0CC0F,0xB56F74E8,0x18ACF3D6,0xCE89E299,0xB4A84FE0,0xFD13E0B7,0x7CC43B81,0xD2ADA8D9,
		0x165FA266,0x80957705,0x93CC7314,0x211A1477,0xE6AD2065,0x77B5FA86,0xC75442F5,0xFB9D35CF,
		0xEBCDAF0C,0x7B3E89A0,0xD6411BD3,0xAE1E7E49,0x00250E2D,0x2071B35E,0x226800BB,0x57B8E0AF,
		0x2464369B,0xF009B91E,0x5563911D,0x59DFA6AA,0x78C14389,0xD95A537F,0x207D5BA2,0x02E5B9C5,
		0x83260376,0x6295CFA9,0x11C81968,0x4E734A41,0xB3472DCA,0x7B14A94A,0x1B510052,0x9A532915,
		0xD60F573F,0xBC9BC6E4,0x2B60A476,0x81E67400,0x08BA6FB5,0x571BE91F,0xF296EC6B,0x2A0DD915,
		0xB6636521,0xE7B9F9B6,0xFF34052E,0xC5855664,0x53B02D5D,0xA99F8FA1,0x08BA4799,0x6E85076A
},{
	0x4B7A70E9,0xB5B32944,0xDB75092E,0xC4192623,0xAD6EA6B0,0x49A7DF7D,0x9CEE60B8,0x8FEDB266,
		0xECAA8C71,0x699A17FF,0x5664526C,0xC2B19EE1,0x193602A5,0x75094C29,0xA0591340,0xE4183A3E,
		0x3F54989A,0x5B429D65,0x6B8FE4D6,0x99F73FD6,0xA1D29C07,0xEFE830F5,0x4D2D38E6,0xF0255DC1,
		0x4CDD2086,0x8470EB26,0x6382E9C6,0x021ECC5E,0x09686B3F,0x3EBAEFC9,0x3C971814,0x6B6A70A1,
		0x687F3584,0x52A0E286,0xB79C5305,0xAA500737,0x3E07841C,0x7FDEAE5C,0x8E7D44EC,0x5716F2B8,
		0xB03ADA37,0xF0500C0D,0xF01C1F04,0x0200B3FF,0xAE0CF51A,0x3CB574B2,0x25837A58,0xDC0921BD,
		0xD19113F9,0x7CA92FF6,0x94324773,0x22F54701,0x3AE5E581,0x37C2DADC,0xC8B57634,0x9AF3DDA7,
		0xA9446146,0x0FD0030E,0xECC8C73E,0xA4751E41,0xE238CD99,0x3BEA0E2F,0x3280BBA1,0x183EB331,
		0x4E548B38,0x4F6DB908,0x6F420D03,0xF60A04BF,0x2CB81290,0x24977C79,0x5679B072,0xBCAF89AF,
		0xDE9A771F,0xD9930810,0xB38BAE12,0xDCCF3F2E,0x5512721F,0x2E6B7124,0x501ADDE6,0x9F84CD87,
		0x7A584718,0x7408DA17,0xBC9F9ABC,0xE94B7D8C,0xEC7AEC3A,0xDB851DFA,0x63094366,0xC464C3D2,
		0xEF1C1847,0x3215D908,0xDD433B37,0x24C2BA16,0x12A14D43,0x2A65C451,0x50940002,0x133AE4DD,
		0x71DFF89E,0x10314E55,0x81AC77D6,0x5F11199B,0x043556F1,0xD7A3C76B,0x3C11183B,0x5924A509,
		0xF28FE6ED,0x97F1FBFA,0x9EBABF2C,0x1E153C6E,0x86E34570,0xEAE96FB1,0x860E5E0A,0x5A3E2AB3,
		0x771FE71C,0x4E3D06FA,0x2965DCB9,0x99E71D0F,0x803E89D6,0x5266C825,0x2E4CC978,0x9C10B36A,
		0xC6150EBA,0x94E2EA78,0xA5FC3C53,0x1E0A2DF4,0xF2F74EA7,0x361D2B3D,0x1939260F,0x19C27960,
		0x5223A708,0xF71312B6,0xEBADFE6E,0xEAC31F66,0xE3BC4595,0xA67BC883,0xB17F37D1,0x018CFF28,
		0xC332DDEF,0xBE6C5AA5,0x65582185,0x68AB9802,0xEECEA50F,0xDB2F953B,0x2AEF7DAD,0x5B6E2F84,
		0x1521B628,0x29076170,0xECDD4775,0x619F1510,0x13CCA830,0xEB61BD96,0x0334FE1E,0xAA0363CF,
		0xB5735C90,0x4C70A239,0xD59E9E0B,0xCBAADE14,0xEECC86BC,0x60622CA7,0x9CAB5CAB,0xB2F3846E,
		0x648B1EAF,0x19BDF0CA,0xA02369B9,0x655ABB50,0x40685A32,0x3C2AB4B3,0x319EE9D5,0xC021B8F7,
		0x9B540B19,0x875FA099,0x95F7997E,0x623D7DA8,0xF837889A,0x97E32D77,0x11ED935F,0x16681281,
		0x0E358829,0xC7E61FD6,0x96DEDFA1,0x7858BA99,0x57F584A5,0x1B227263,0x9B83C3FF,0x1AC24696,
		0xCDB30AEB,0x532E3054,0x8FD948E4,0x6DBC3128,0x58EBF2EF,0x34C6FFEA,0xFE28ED61,0xEE7C3C73,
		0x5D4A14D9,0xE864B7E3,0x42105D14,0x203E13E0,0x45EEE2B6,0xA3AAABEA,0xDB6C4F15,0xFACB4FD0,
		0xC742F442,0xEF6ABBB5,0x654F3B1D,0x41CD2105,0xD81E799E,0x86854DC7,0xE44B476A,0x3D816250,
		0xCF62A1F2,0x5B8D2646,0xFC8883A0,0xC1C7B6A3,0x7F1524C3,0x69CB7492,0x47848A0B,0x5692B285,
		0x095BBF00,0xAD19489D,0x1462B174,0x23820E00,0x58428D2A,0x0C55F5EA,0x1DADF43E,0x233F7061,
		0x3372F092,0x8D937E41,0xD65FECF1,0x6C223BDB,0x7CDE3759,0xCBEE7460,0x4085F2A7,0xCE77326E,
		0xA6078084,0x19F8509E,0xE8EFD855,0x61D99735,0xA969A7AA,0xC50C06C2,0x5A04ABFC,0x800BCADC,
		0x9E447A2E,0xC3453484,0xFDD56705,0x0E1E9EC9,0xDB73DBD3,0x105588CD,0x675FDA79,0xE3674340,
		0xC5C43465,0x713E38D8,0x3D28F89E,0xF16DFF20,0x153E21E7,0x8FB03D4A,0xE6E39F2B,0xDB83ADF7
},{
	0xE93D5A68,0x948140F7,0xF64C261C,0x94692934,0x411520F7,0x7602D4F7,0xBCF46B2E,0xD4A20068,
		0xD4082471,0x3320F46A,0x43B7D4B7,0x500061AF,0x1E39F62E,0x97244546,0x14214F74,0xBF8B8840,
		0x4D95FC1D,0x96B591AF,0x70F4DDD3,0x66A02F45,0xBFBC09EC,0x03BD9785,0x7FAC6DD0,0x31CB8504,
		0x96EB27B3,0x55FD3941,0xDA2547E6,0xABCA0A9A,0x28507825,0x530429F4,0x0A2C86DA,0xE9B66DFB,
		0x68DC1462,0xD7486900,0x680EC0A4,0x27A18DEE,0x4F3FFEA2,0xE887AD8C,0xB58CE006,0x7AF4D6B6,
		0xAACE1E7C,0xD3375FEC,0xCE78A399,0x406B2A42,0x20FE9E35,0xD9F385B9,0xEE39D7AB,0x3B124E8B,
		0x1DC9FAF7,0x4B6D1856,0x26A36631,0xEAE397B2,0x3A6EFA74,0xDD5B4332,0x6841E7F7,0xCA7820FB,
		0xFB0AF54E,0xD8FEB397,0x454056AC,0xBA489527,0x55533A3A,0x20838D87,0xFE6BA9B7,0xD096954B,
		0x55A867BC,0xA1159A58,0xCCA92963,0x99E1DB33,0xA62A4A56,0x3F3125F9,0x5EF47E1C,0x9029317C,
		0xFDF8E802,0x04272F70,0x80BB155C,0x05282CE3,0x95C11548,0xE4C66D22,0x48C1133F,0xC70F86DC,
		0x07F9C9EE,0x41041F0F,0x404779A4,0x5D886E17,0x325F51EB,0xD59BC0D1,0xF2BCC18F,0x41113564,
		0x257B7834,0x602A9C60,0xDFF8E8A3,0x1F636C1B,0x0E12B4C2,0x02E1329E,0xAF664FD1,0xCAD18115,
		0x6B2395E0,0x333E92E1,0x3B240B62,0xEEBEB922,0x85B2A20E,0xE6BA0D99,0xDE720C8C,0x2DA2F728,
		0xD0127845,0x95B794FD,0x647D0862,0xE7CCF5F0,0x5449A36F,0x877D48FA,0xC39DFD27,0xF33E8D1E,
		0x0A476341,0x992EFF74,0x3A6F6EAB,0xF4F8FD37,0xA812DC60,0xA1EBDDF8,0x991BE14C,0xDB6E6B0D,
		0xC67B5510,0x6D672C37,0x2765D43B,0xDCD0E804,0xF1290DC7,0xCC00FFA3,0xB5390F92,0x690FED0B,
		0x667B9FFB,0xCEDB7D9C,0xA091CF0B,0xD9155EA3,0xBB132F88,0x515BAD24,0x7B9479BF,0x763BD6EB,
		0x37392EB3,0xCC115979,0x8026E297,0xF42E312D,0x6842ADA7,0xC66A2B3B,0x12754CCC,0x782EF11C,
		0x6A124237,0xB79251E7,0x06A1BBE6,0x4BFB6350,0x1A6B1018,0x11CAEDFA,0x3D25BDD8,0xE2E1C3C9,
		0x44421659,0x0A121386,0xD90CEC6E,0xD5ABEA2A,0x64AF674E,0xDA86A85F,0xBEBFE988,0x64E4C3FE,
		0x9DBC8057,0xF0F7C086,0x60787BF8,0x6003604D,0xD1FD8346,0xF6381FB0,0x7745AE04,0xD736FCCC,
		0x83426B33,0xF01EAB71,0xB0804187,0x3C005E5F,0x77A057BE,0xBDE8AE24,0x55464299,0xBF582E61,
		0x4E58F48F,0xF2DDFDA2,0xF474EF38,0x8789BDC2,0x5366F9C3,0xC8B38E74,0xB475F255,0x46FCD9B9,
		0x7AEB2661,0x8B1DDF84,0x846A0E79,0x915F95E2,0x466E598E,0x20B45770,0x8CD55591,0xC902DE4C,
		0xB90BACE1,0xBB8205D0,0x11A86248,0x7574A99E,0xB77F19B6,0xE0A9DC09,0x662D09A1,0xC4324633,
		0xE85A1F02,0x09F0BE8C,0x4A99A025,0x1D6EFE10,0x1AB93D1D,0x0BA5A4DF,0xA186F20F,0x2868F169,
		0xDCB7DA83,0x573906FE,0xA1E2CE9B,0x4FCD7F52,0x50115E01,0xA70683FA,0xA002B5C4,0x0DE6D027,
		0x9AF88C27,0x773F8641,0xC3604C06,0x61A806B5,0xF0177A28,0xC0F586E0,0x006058AA,0x30DC7D62,
		0x11E69ED7,0x2338EA63,0x53C2DD94,0xC2C21634,0xBBCBEE56,0x90BCB6DE,0xEBFC7DA1,0xCE591D76,
		0x6F05E409,0x4B7C0188,0x39720A3D,0x7C927C24,0x86E3725F,0x724D9DB9,0x1AC15BB4,0xD39EB8FC,
		0xED545578,0x08FCA5B5,0xD83D7CD3,0x4DAD0FC4,0x1E50EF5E,0xB161E6F8,0xA28514D9,0x6C51133C,
		0x6FD5C7E7,0x56E14EC4,0x362ABFCE,0xDDC6C837,0xD79A3234,0x92638212,0x670EFA8E,0x406000E0
	},{
	0x3A39CE37,0xD3FAF5CF,0xABC27737,0x5AC52D1B,0x5CB0679E,0x4FA33742,0xD3822740,0x99BC9BBE,
		0xD5118E9D,0xBF0F7315,0xD62D1C7E,0xC700C47B,0xB78C1B6B,0x21A19045,0xB26EB1BE,0x6A366EB4,
		0x5748AB2F,0xBC946E79,0xC6A376D2,0x6549C2C8,0x530FF8EE,0x468DDE7D,0xD5730A1D,0x4CD04DC6,
		0x2939BBDB,0xA9BA4650,0xAC9526E8,0xBE5EE304,0xA1FAD5F0,0x6A2D519A,0x63EF8CE2,0x9A86EE22,
		0xC089C2B8,0x43242EF6,0xA51E03AA,0x9CF2D0A4,0x83C061BA,0x9BE96A4D,0x8FE51550,0xBA645BD6,
		0x2826A2F9,0xA73A3AE1,0x4BA99586,0xEF5562E9,0xC72FEFD3,0xF752F7DA,0x3F046F69,0x77FA0A59,
		0x80E4A915,0x87B08601,0x9B09E6AD,0x3B3EE593,0xE990FD5A,0x9E34D797,0x2CF0B7D9,0x022B8B51,
		0x96D5AC3A,0x017DA67D,0xD1CF3ED6,0x7C7D2D28,0x1F9F25CF,0xADF2B89B,0x5AD6B472,0x5A88F54C,
		0xE029AC71,0xE019A5E6,0x47B0ACFD,0xED93FA9B,0xE8D3C48D,0x283B57CC,0xF8D56629,0x79132E28,
		0x785F0191,0xED756055,0xF7960E44,0xE3D35E8C,0x15056DD4,0x88F46DBA,0x03A16125,0x0564F0BD,
		0xC3EB9E15,0x3C9057A2,0x97271AEC,0xA93A072A,0x1B3F6D9B,0x1E6321F5,0xF59C66FB,0x26DCF319,
		0x7533D928,0xB155FDF5,0x03563482,0x8ABA3CBB,0x28517711,0xC20AD9F8,0xABCC5167,0xCCAD925F,
		0x4DE81751,0x3830DC8E,0x379D5862,0x9320F991,0xEA7A90C2,0xFB3E7BCE,0x5121CE64,0x774FBE32,
		0xA8B6E37E,0xC3293D46,0x48DE5369,0x6413E680,0xA2AE0810,0xDD6DB224,0x69852DFD,0x09072166,
		0xB39A460A,0x6445C0DD,0x586CDECF,0x1C20C8AE,0x5BBEF7DD,0x1B588D40,0xCCD2017F,0x6BB4E3BB,
		0xDDA26A7E,0x3A59FF45,0x3E350A44,0xBCB4CDD5,0x72EACEA8,0xFA6484BB,0x8D6612AE,0xBF3C6F47,
		0xD29BE463,0x542F5D9E,0xAEC2771B,0xF64E6370,0x740E0D8D,0xE75B1357,0xF8721671,0xAF537D5D,
		0x4040CB08,0x4EB4E2CC,0x34D2466A,0x0115AF84,0xE1B00428,0x95983A1D,0x06B89FB4,0xCE6EA048,
		0x6F3F3B82,0x3520AB82,0x011A1D4B,0x277227F8,0x611560B1,0xE7933FDC,0xBB3A792B,0x344525BD,
		0xA08839E1,0x51CE794B,0x2F32C9B7,0xA01FBAC9,0xE01CC87E,0xBCC7D1F6,0xCF0111C3,0xA1E8AAC7,
		0x1A908749,0xD44FBD9A,0xD0DADECB,0xD50ADA38,0x0339C32A,0xC6913667,0x8DF9317C,0xE0B12B4F,
		0xF79E59B7,0x43F5BB3A,0xF2D519FF,0x27D9459C,0xBF97222C,0x15E6FC2A,0x0F91FC71,0x9B941525,
		0xFAE59361,0xCEB69CEB,0xC2A86459,0x12BAA8D1,0xB6C1075E,0xE3056A0C,0x10D25065,0xCB03A442,
		0xE0EC6E0E,0x1698DB3B,0x4C98A0BE,0x3278E964,0x9F1F9532,0xE0D392DF,0xD3A0342B,0x8971F21E,
		0x1B0A7441,0x4BA3348C,0xC5BE7120,0xC37632D8,0xDF359F8D,0x9B992F2E,0xE60B6F47,0x0FE3F11D,
		0xE54CDA54,0x1EDAD891,0xCE6279CF,0xCD3E7E6F,0x1618B166,0xFD2C1D05,0x848FD2C5,0xF6FB2299,
		0xF523F357,0xA6327623,0x93A83531,0x56CCCD02,0xACF08162,0x5A75EBB5,0x6E163697,0x88D273CC,
		0xDE966292,0x81B949D0,0x4C50901B,0x71C65614,0xE6C6C7BD,0x327A140A,0x45E1D006,0xC3F27B9A,
		0xC9AA53FD,0x62A80F00,0xBB25BFE2,0x35BDD2F6,0x71126905,0xB2040222,0xB6CBCF7C,0xCD769C2B,
		0x53113EC0,0x1640E3D3,0x38ABBD60,0x2547ADF0,0xBA38209C,0xF746CE76,0x77AFA1C5,0x20756060,
		0x85CBFE4E,0x8AE88DD8,0x7AAAF9B0,0x4CF9AA7E,0x1948C25C,0x02FB8A8C,0x01C36AE4,0xD6EBE1F9,
		0x90D4F869,0xA65CDEA0,0x3F09252D,0xC208E69F,0xB74E6132,0xCE77E25B,0x578FDFE3,0x3AC372E6
} };

/*********************** FUNCTION DEFINITIONS ***********************/
void blowfish_encrypt(const BYTE in[], BYTE out[], const _BLOWFISH_KEY *keystruct)
{
	UINT l,r,t; //,i;

	l = (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | (in[3]);
	r = (in[4] << 24) | (in[5] << 16) | (in[6] << 8) | (in[7]);

	ITERATION(l,r,t,0);
	ITERATION(l,r,t,1);
	ITERATION(l,r,t,2);
	ITERATION(l,r,t,3);
	ITERATION(l,r,t,4);
	ITERATION(l,r,t,5);
	ITERATION(l,r,t,6);
	ITERATION(l,r,t,7);
	ITERATION(l,r,t,8);
	ITERATION(l,r,t,9);
	ITERATION(l,r,t,10);
	ITERATION(l,r,t,11);
	ITERATION(l,r,t,12);
	ITERATION(l,r,t,13);
	ITERATION(l,r,t,14);
	l ^= keystruct->p[15]; BF(l,t); r^= t; //Last iteration has no swap()
	r ^= keystruct->p[16];
	l ^= keystruct->p[17];

	out[0] = l >> 24;
	out[1] = l >> 16;
	out[2] = l >> 8;
	out[3] = l;
	out[4] = r >> 24;
	out[5] = r >> 16;
	out[6] = r >> 8;
	out[7] = r;
}

void blowfish_decrypt(const BYTE in[], BYTE out[], const _BLOWFISH_KEY *keystruct)
{
	UINT l,r,t; //,i;

	l = (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | (in[3]);
	r = (in[4] << 24) | (in[5] << 16) | (in[6] << 8) | (in[7]);

	ITERATION(l,r,t,17);
	ITERATION(l,r,t,16);
	ITERATION(l,r,t,15);
	ITERATION(l,r,t,14);
	ITERATION(l,r,t,13);
	ITERATION(l,r,t,12);
	ITERATION(l,r,t,11);
	ITERATION(l,r,t,10);
	ITERATION(l,r,t,9);
	ITERATION(l,r,t,8);
	ITERATION(l,r,t,7);
	ITERATION(l,r,t,6);
	ITERATION(l,r,t,5);
	ITERATION(l,r,t,4);
	ITERATION(l,r,t,3);
	l ^= keystruct->p[2]; BF(l,t); r^= t; //Last iteration has no swap()
	r ^= keystruct->p[1];
	l ^= keystruct->p[0];

	out[0] = l >> 24;
	out[1] = l >> 16;
	out[2] = l >> 8;
	out[3] = l;
	out[4] = r >> 24;
	out[5] = r >> 16;
	out[6] = r >> 8;
	out[7] = r;
}

void blowfish_key_setup(const BYTE user_key[], _BLOWFISH_KEY *keystruct, size_t len)
{
	BYTE block[8];
	int idx,idx2;

	// Copy over the constant init array vals (so the originals aren't destroyed).
	memcpy(keystruct->p,p_perm,sizeof(UINT) * 18);
	memcpy(keystruct->s,s_perm,sizeof(UINT) * 1024);

	// Combine the key with the P box. Assume key is standard 448 bits (56 bytes) or less.
	for (idx = 0, idx2 = 0; idx < 18; ++idx, idx2 += 4)
		keystruct->p[idx] ^= (user_key[idx2 % len] << 24) | (user_key[(idx2+1) % len] << 16)
		| (user_key[(idx2+2) % len] << 8) | (user_key[(idx2+3) % len]);
	// Re-calculate the P box.
	memset(block, 0, 8);
	for (idx = 0; idx < 18; idx += 2) {
		blowfish_encrypt(block,block,keystruct);
		keystruct->p[idx] = (block[0] << 24) | (block[1] << 16) | (block[2] << 8) | block[3];
		keystruct->p[idx+1]=(block[4] << 24) | (block[5] << 16) | (block[6] << 8) | block[7];
	}
	// Recalculate the S-boxes.
	for (idx = 0; idx < 4; ++idx) {
		for (idx2 = 0; idx2 < 256; idx2 += 2) {
			blowfish_encrypt(block,block,keystruct);
			keystruct->s[idx][idx2] = (block[0] << 24) | (block[1] << 16) |
				(block[2] << 8) | block[3];
			keystruct->s[idx][idx2+1] = (block[4] << 24) | (block[5] << 16) |
				(block[6] << 8) | block[7];
		}
	}
}

// -------------------------------------------------- ROT-13 -------------------------------------------------- //

/*********************** FUNCTION DEFINITIONS ***********************/
void rot13(char str[])
{
	int case_type, idx, len;

	for (idx = 0, len = (int)strlen(str); idx < len; idx++) {
		// Only process alphabetic characters.
		if (str[idx] < 'A' || (str[idx] > 'Z' && str[idx] < 'a') || str[idx] > 'z')
			continue;
		// Determine if the char is upper or lower case.
		if (str[idx] >= 'a')
			case_type = 'a';
		else
			case_type = 'A';
		// Rotate the char's value, ensuring it doesn't accidentally "fall off" the end.
		str[idx] = (str[idx] + 13) % (case_type + 26);
		if (str[idx] < 26)
			str[idx] += case_type;
	}
}
