#pragma once

// -------------------------------------------------- BASE64 -------------------------------------------------- //

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
size_t base64_encode(const BYTE in[], BYTE out[], size_t len, int newline_flag);

// Returns the size of the output. If called with out = NULL, will just return
// the size of what the output would have been (without a terminating NULL).
size_t base64_decode(const BYTE in[], BYTE out[], size_t len);

// -------------------------------------------------- URL -------------------------------------------------- //

int url_encode(const char* src, const int src_size, char* dest, const int dest_size);
int url_decode(const char* src, const int src_size, char* dest, const int dest_size);

// -------------------------------------------------- AES -------------------------------------------------- //

/****************************** MACROS ******************************/
#define AES_BLOCK_SIZE 16               // AES operates on 16 bytes at a time

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;            // 8-bit byte
typedef unsigned int UINT;             // 32-bit word, change to "long" for 16-bit machines

/*********************** FUNCTION DECLARATIONS **********************/
///////////////////
// AES
///////////////////
// Key setup must be done before any AES en/de-cryption functions can be used.
void aes_key_setup(const BYTE key[],          // The key, must be 128, 192, or 256 bits
	UINT w[],                  // Output key schedule to be used later
	int keysize);              // Bit length of the key, 128, 192, or 256

void aes_encrypt(const BYTE in[],             // 16 bytes of plaintext
	BYTE out[],                  // 16 bytes of ciphertext
	const UINT key[],            // From the key setup
	int keysize);                // Bit length of the key, 128, 192, or 256

void aes_decrypt(const BYTE in[],             // 16 bytes of ciphertext
	BYTE out[],                  // 16 bytes of plaintext
	const UINT key[],            // From the key setup
	int keysize);                // Bit length of the key, 128, 192, or 256

///////////////////
// AES - CBC
///////////////////
int aes_encrypt_cbc(const BYTE in[],          // Plaintext
	size_t in_len,            // Must be a multiple of AES_BLOCK_SIZE
	BYTE out[],               // Ciphertext, same length as plaintext
	const UINT key[],         // From the key setup
	int keysize,              // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);         // IV, must be AES_BLOCK_SIZE bytes long

// Only output the CBC-MAC of the input.
int aes_encrypt_cbc_mac(const BYTE in[],      // plaintext
	size_t in_len,        // Must be a multiple of AES_BLOCK_SIZE
	BYTE out[],           // Output MAC
	const UINT key[],     // From the key setup
	int keysize,          // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);     // IV, must be AES_BLOCK_SIZE bytes long

///////////////////
// AES - CTR
///////////////////
void increment_iv(BYTE iv[],                  // Must be a multiple of AES_BLOCK_SIZE
	int counter_size);          // Bytes of the IV used for counting (low end)

void aes_encrypt_ctr(const BYTE in[],         // Plaintext
	size_t in_len,           // Any byte length
	BYTE out[],              // Ciphertext, same length as plaintext
	const UINT key[],        // From the key setup
	int keysize,             // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);        // IV, must be AES_BLOCK_SIZE bytes long

void aes_decrypt_ctr(const BYTE in[],         // Ciphertext
	size_t in_len,           // Any byte length
	BYTE out[],              // Plaintext, same length as ciphertext
	const UINT key[],        // From the key setup
	int keysize,             // Bit length of the key, 128, 192, or 256
	const BYTE iv[]);        // IV, must be AES_BLOCK_SIZE bytes long

///////////////////
// AES - CCM
///////////////////
// Returns True if the input parameters do not violate any constraint.
int aes_encrypt_ccm(const BYTE plaintext[],              // IN  - Plaintext.
	UINT plaintext_len,                  // IN  - Plaintext length.
	const BYTE associated_data[],        // IN  - Associated Data included in authentication, but not encryption.
	unsigned short associated_data_len,  // IN  - Associated Data length in bytes.
	const BYTE nonce[],                  // IN  - The Nonce to be used for encryption.
	unsigned short nonce_len,            // IN  - Nonce length in bytes.
	BYTE ciphertext[],                   // OUT - Ciphertext, a concatination of the plaintext and the MAC.
	UINT *ciphertext_len,                // OUT - The length of the ciphertext, always plaintext_len + mac_len.
	UINT mac_len,                        // IN  - The desired length of the MAC, must be 4, 6, 8, 10, 12, 14, or 16.
	const BYTE key[],                    // IN  - The AES key for encryption.
	int keysize);                        // IN  - The length of the key in bits. Valid values are 128, 192, 256.

// Returns True if the input parameters do not violate any constraint.
// Use mac_auth to ensure decryption/validation was preformed correctly.
// If authentication does not succeed, the plaintext is zeroed out. To overwride
// this, call with mac_auth = NULL. The proper proceedure is to decrypt with
// authentication enabled (mac_auth != NULL) and make a second call to that
// ignores authentication explicitly if the first call failes.
int aes_decrypt_ccm(const BYTE ciphertext[],             // IN  - Ciphertext, the concatination of encrypted plaintext and MAC.
	UINT ciphertext_len,                 // IN  - Ciphertext length in bytes.
	const BYTE assoc[],                  // IN  - The Associated Data, required for authentication.
	unsigned short assoc_len,            // IN  - Associated Data length in bytes.
	const BYTE nonce[],                  // IN  - The Nonce to use for decryption, same one as for encryption.
	unsigned short nonce_len,            // IN  - Nonce length in bytes.
	BYTE plaintext[],                    // OUT - The plaintext that was decrypted. Will need to be large enough to hold ciphertext_len - mac_len.
	UINT *plaintext_len,                 // OUT - Length in bytes of the output plaintext, always ciphertext_len - mac_len .
	UINT mac_len,                        // IN  - The length of the MAC that was calculated.
	int *mac_auth,                       // OUT - TRUE if authentication succeeded, FALSE if it did not. NULL pointer will ignore the authentication.
	const BYTE key[],                    // IN  - The AES key for decryption.
	int keysize);                        // IN  - The length of the key in BITS. Valid values are 128, 192, 256.

// -------------------------------------------------- DES -------------------------------------------------- //

/****************************** MACROS ******************************/
#define DES_BLOCK_SIZE 8                // DES operates on 8 bytes at a time

/**************************** DATA TYPES ****************************/

typedef enum {
	DES_ENCRYPT,
	DES_DECRYPT
} DES_MODE;

/*********************** FUNCTION DECLARATIONS **********************/
void des_key_setup(const BYTE key[], BYTE schedule[][6], DES_MODE mode);
void des_crypt(const BYTE in[], BYTE out[], const BYTE key[][6]);

void three_des_key_setup(const BYTE key[], BYTE schedule[][16][6], DES_MODE mode);
void three_des_crypt(const BYTE in[], BYTE out[], const BYTE key[][16][6]);

// -------------------------------------------------- MD2 -------------------------------------------------- //

/****************************** MACROS ******************************/
#define MD2_BLOCK_SIZE 16

/**************************** DATA TYPES ****************************/

typedef struct {
	BYTE data[16];
	BYTE state[48];
	BYTE checksum[16];
	int len;
} _MD2_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void md2_init(_MD2_CTX *ctx);
void md2_update(_MD2_CTX *ctx, const BYTE data[], size_t len);
void md2_final(_MD2_CTX *ctx, BYTE hash[]);   // size of hash must be MD2_BLOCK_SIZE

// -------------------------------------------------- MD5 -------------------------------------------------- //

/****************************** MACROS ******************************/
#define MD5_BLOCK_SIZE 16               // MD5 outputs a 16 byte digest

/**************************** DATA TYPES ****************************/

typedef struct {
	BYTE data[64];
	UINT datalen;
	unsigned long long bitlen;
	UINT state[4];
} _MD5_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void md5_init(_MD5_CTX *ctx);
void md5_update(_MD5_CTX *ctx, const BYTE data[], size_t len);
void md5_final(_MD5_CTX *ctx, BYTE hash[]);

// -------------------------------------------------- SHA1 -------------------------------------------------- //

/****************************** MACROS ******************************/
#define SHA1_BLOCK_SIZE 20              // SHA1 outputs a 20 byte digest

/**************************** DATA TYPES ****************************/

typedef struct {
	BYTE data[64];
	UINT datalen;
	unsigned long long bitlen;
	UINT state[5];
	UINT k[4];
} _SHA1_CTX;

/*********************** FUNCTION DECLARATIONS **********************/

void sha1_init(_SHA1_CTX *ctx);
void sha1_update(_SHA1_CTX *ctx, const BYTE data[], size_t len);
void sha1_final(_SHA1_CTX *ctx, BYTE hash[]);

// -------------------------------------------------- SHA256 -------------------------------------------------- //

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/

typedef struct {
	BYTE data[64];
	UINT datalen;
	unsigned long long bitlen;
	UINT state[8];
} _SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(_SHA256_CTX *ctx);
void sha256_update(_SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(_SHA256_CTX *ctx, BYTE hash[]);

// -------------------------------------------------- ARCFOUR -------------------------------------------------- //

/**************************** DATA TYPES ****************************/

/*********************** FUNCTION DECLARATIONS **********************/
// Input: state - the state used to generate the keystream
//        key - Key to use to initialize the state
//        len - length of key in bytes (valid lenth is 1 to 256)
void arcfour_key_setup(BYTE state[], const BYTE key[], int len);

// Pseudo-Random Generator Algorithm
// Input: state - the state used to generate the keystream
//        out - Must be allocated to be of at least "len" length
//        len - number of bytes to generate
void arcfour_generate_stream(BYTE state[], BYTE out[], size_t len);

// -------------------------------------------------- BLOWFISH -------------------------------------------------- //

/****************************** MACROS ******************************/
#define BLOWFISH_BLOCK_SIZE 8           // Blowfish operates on 8 bytes at a time

/**************************** DATA TYPES ****************************/

typedef struct {
	WORD p[18];
	WORD s[4][256];
} _BLOWFISH_KEY;

/*********************** FUNCTION DECLARATIONS **********************/
void blowfish_key_setup(const BYTE user_key[], _BLOWFISH_KEY *keystruct, size_t len);
void blowfish_encrypt(const BYTE in[], BYTE out[], const _BLOWFISH_KEY *keystruct);
void blowfish_decrypt(const BYTE in[], BYTE out[], const _BLOWFISH_KEY *keystruct);

// -------------------------------------------------- ROT-13 -------------------------------------------------- //

/*********************** FUNCTION DECLARATIONS **********************/
// Performs IN PLACE rotation of the input. Assumes input is NULL terminated.
// Preserves each charcter's case. Ignores non alphabetic characters.
void rot13(char str[]);
