#include "md5.h"

#define GET_UINT32(n,b,i) {  \
	(n) = ( (unsigned long int) (b)[(i)    ]     )  \
		| ( (unsigned long int) (b)[(i) + 1] <<  8 )  \
		| ( (unsigned long int) (b)[(i) + 2] << 16 )  \
		| ( (unsigned long int) (b)[(i) + 3] << 24 ); \
}

#define PUT_UINT32(n,b,i) {  \
	(b)[(i)    ] = (unsigned char) ( (n)       );  \
	(b)[(i) + 1] = (unsigned char) ( (n) >>  8 );  \
	(b)[(i) + 2] = (unsigned char) ( (n) >> 16 );  \
	(b)[(i) + 3] = (unsigned char) ( (n) >> 24 );  \
}

#define F(x,y,z) ((z) ^ ((x) & ((y) ^ (z))))

#define G(x,y,z) ((y) ^ ((z) & ((x) ^ (y))))

#define H(x,y,z) ((x) ^ (y) ^ (z))

#define I(x,y,z) ((y) ^ ((x) | ~(z)))

#define S(x,n) (((x) << (n)) | (((x) & 0xFFFFFFFF) >> (32 - (n))))

#define P(r,i,f,k,s,t) {  \
	r[i] += f(r[((i)+1)%4],r[((i)+2)%4],r[((i)+3)%4]) + X[k] + t; \
	r[i] = S(r[i],s) + r[((i)+1)%4];  \
}


Jabber::md5::md5()
{
	Reset();
}

Jabber::md5::~md5()
{
}

void Jabber::md5::Reset()
{
	memset(this, 0, sizeof(md5));
	state[0] = 0x67452301;
	state[1] = 0xEFCDAB89;
	state[2] = 0x98BADCFE;
	state[3] = 0x10325476;
}

void Jabber::md5::Hash(const unsigned char *data, size_t slen, int finish)
{
	int i, j;
	int len = slen;

	i = (64 - blen);
	j = (len < i) ? (len) : (i);
	memcpy(buffer + blen, data, j);
	blen += j;
	len -= j;
	data += j;
	while (len > 0) {
		Compute();
		blen = 0;
		total[0] += 8*64;
		total[1] += (total[0] < 8*64);
		j = (len < 64) ? (len) : (64);
		memcpy(buffer, data, j);
		blen = j;
		len -= j;
		data += j;
	}
	if (finish) {
		total[0] += 8*blen;
		total[1] += (total[0] < 8*blen);
		buffer[(blen)++] = 0x80;
		if (blen > 56) {
			while (blen < 64)
				buffer[(blen)++] = 0x00;
			Compute();
			blen = 0;
		}
		while (blen < 56)
			buffer[(blen)++] = 0x00;
		PUT_UINT32(total[0], buffer, 56);
		PUT_UINT32(total[1], buffer, 60);
		Compute();
	}
}

void Jabber::md5::Digest(unsigned char *digest)
{
	PUT_UINT32(state[0], digest,  0);
	PUT_UINT32(state[1], digest,  4);
	PUT_UINT32(state[2], digest,  8);
	PUT_UINT32(state[3], digest, 12);
}

void Jabber::md5::Print(char *buf)
{
  int i;
	unsigned char digest[16];

	Digest(digest);
	for (i = 0; i < 16; i++) {
		sprintf (buf, "%02x", digest[i]);
		buf += 2;
	}
}

/*void cos_md5(const char *data, char *buf)
{
	md5 *_md5 = md5_new();

	md5_hash(_md5, (const unsigned char*)data, strlen(data), 1);
	md5_print(_md5, buf);
	md5_delete(_md5);
}*/

void Jabber::md5::Compute()
{
	unsigned long int X[16], R[4];
	unsigned char RS1[] = { 7, 12 ,17, 22 };
	unsigned char RS2[] = { 5, 9 ,14, 20 };
	unsigned char RS3[] = { 4, 11 ,16, 23 };
	unsigned char RS4[] = { 6, 10 ,15, 21 };
	int i, j, k, p;

	for (i = 0; i < 16; ++i)
		 GET_UINT32(X[i], buffer, i*4);

	for (i = 0; i < 4; ++i)
		R[i] = state[i];

	for (i = j = k = 0; i < 16; ++i, j = i%4, k = (k+3)%4)
		P(R, k, F, i, RS1[j], T[i]);

	for (i = j = k = 0, p = 1; i < 16; ++i, j = i%4, k = (k+3)%4, p = (p+5)%16)
		P(R, k, G, p, RS2[j], T[i+16]);

	for (i = j = k = 0, p = 5; i < 16; ++i, j = i%4, k = (k+3)%4, p = (p+3)%16)
		P(R, k, H, p, RS3[j], T[i+32]);

	for (i = j = k = p = 0; i < 16; ++i, j = i%4, k = (k+3)%4, p = (p+7)%16)
		P(R, k, I, p, RS4[j], T[i+48]);

	for (i = 0; i < 4; ++i)
		state[i] += R[i];
}

