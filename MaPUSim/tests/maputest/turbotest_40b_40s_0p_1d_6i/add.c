const unsigned int sys[] __attribute__((section (".SDA0.DM0"))) = {
0xc0700000,
0x3fa00000,
0xc0400000,
0x3f000000,
0xc0000000,
0x40600000,
0xbfa00000,
0xc0300000,
0xc0d00000,
0xbfa00000,
0x40880000,
0x3f400000,
0x3e800000,
0xc0980000,
0xbfa00000,
0xbf000000,
0xc0500000,
0xc0e00000,
0xbfe00000,
0x3fe00000,
0x40600000,
0x3fc00000,
0xbfc00000,
0x40900000,
0x3fc00000,
0xc0000000,
0xbe800000,
0x40100000,
0x3f000000,
0xbf400000,
0xbf400000,
0xbf000000,
0x3f800000,
0x40200000,
0x3fc00000,
0xc0700000,
0xc0d80000,
0xc0300000,
0x40400000,
0x3f400000

};
const unsigned int par1[] __attribute__((section (".SDA0.DM0"))) = {
0xc0100000,
0xc0100000,
0xbfc00000,
0x3f800000,
0000000000,
0x40700000,
0x40200000,
0x3fc00000,
0x40200000,
0x40200000,
0xbf000000,
0xbf400000,
0000000000,
0x40400000,
0xc0000000,
0xc0000000,
0x40000000,
0xc0a80000,
0x40200000,
0x40b00000,
0x40000000,
0xbfc00000,
0x3f000000,
0xc0800000,
0x3f000000,
0xc0600000,
0xc0400000,
0x3fc00000,
0x40000000,
0x3fe00000,
0x40700000,
0xc0300000,
0xc0a80000,
0x3fa00000,
0xc0200000,
0x3fa00000,
0xc0b00000,
0xc0880000,
0x40000000,
0xc0800000,

};
const unsigned int par2[] __attribute__((section (".SDA0.DM0"))) = {
0xbfe00000,
0xbf400000,
0xbe800000,
0xc0980000,
0xc0900000,
0xc0600000,
0xc0100000,
0xbf800000,
0xc0100000,
0xbfc00000,
0xc0100000,
0xc0000000,
0xbe800000,
0x3f000000,
0x3e800000,
0x3e800000,
0x40b80000,
0xc0600000,
0x40300000,
0xbf000000,
0x40100000,
0x40400000,
0x40b00000,
0xc0e00000,
0xc0400000,
0x3fa00000,
0xbe800000,
0x40800000,
0xc0b00000,
0xc0100000,
0xbf000000,
0xc0400000,
0xbf000000,
0x3f800000,
0xc0d00000,
0x3e800000,
0xbfe00000,
0x40e00000,
0xbf400000,
0xc0200000,

};
char out[] = {
  0,0,1,0,1,0,0,0,
    1,0,0,0,0,0,0,0,
    0,0,0,0,1,1,1,0,
    0,0,1,0,0,0,0,0,
    0,0,0,0,0,1,1,1
};
const unsigned int beta_initial_value_cb1[] = {
0x41da0000,
0x41c80000,
0x41da0000,
0x41c40000,
0x41a00000,
0x41b60000,
0x41ec0000,
0x41fe0000
/*
  0x3fe00000,
  0x40000000,
  0xbf400000,
  0xbf800000,
  0x3f000000,
  0x3f400000,
  0xbfc00000,
  0xbfe00000,
*/
};
const unsigned int beta_initial_value_cb2[] = {
  0x40700000,
  0000000000,
  0x3fe00000,
  0x40f00000,
  0xc0600000,
  0xc1140000,
  0xc0000000,
  0x3fe00000,
};
static int as_initial[188][4]={
{0,0,0,0},
{0,0,0,0},
{1,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,0,0,0},
{1,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,1,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{2,2,2,2},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,1,1,1},
{0,0,0,0},
{1,1,1,1},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{1,1,1,1},
{0,0,0,0},
{1,1,1,1},
{1,1,1,1},
{1,1,1,1},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0}
};
static int ts_initial[188][4]={
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,0,0},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,1,2,3},
{0,3,2,1},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,1,2,3},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1},
{0,3,2,1}
};

static int e1e2_initial[188][2] = {
{0, 20},
{0, 24},
{0, 28},
{0, 32},
{0, 36},
{0, 40},
{0, 44},
{0, 48},
{0, 52},
{0, 56},
{0, 60},
{0, 64},
{0, 68},
{0, 72},
{0, 76},
{0, 80},
{0,  0},
{0, 88},
{0, 92},
{0, 96},
{0,100},
{0,104},
{0, 72},
{0,112},
{0,116},
{0,120},
{0,124},
{0, 64},
{0,132},
{0,136},
{0,140},
{0, 72},
{0,148},
{0,152},
{0,156},
{0,240},
{0,164},
{0,168},
{0,172},
{0, 88},
{0,180},
{0, 92},
{0,188},
{0, 96},
{0,196},
{0, 80},
{0,204},
{0,104},
{0,212},
{0,144},
{0,220},
{0,336},
{0,228},
{0,116},
{0,236},
{0,360},
{0,244},
{0,124},
{0,168},
{0,128},
{0,132},
{0,136},
{0,280},
{0,192},
{0,148},
{0,152},
{0,468},
{0,160},
{0,164},
{0,504},
{0,172},
{0, 88},
{0,240},
{0,184},
{0,188},
{0, 96},
{0,196},
{0,160},
{0,204},
{0,104},
{0,212},
{0, 96},
{0,220},
{0,224},
{0,228},
{0,116},
{0,236},
{0,120},
{0,244},
{0,248},
{0,168},
{0,128},
{0,132},
{0,408},
{0,280},
{0,144},
{0,148},
{0,152},
{0,156},
{0,480},
{0,164},
{0,504},
{0,172},
{0,176},
{0,120},
{0,184},
{0,188},
{0, 96},
{0, 56},
{0,160},
{0,204},
{0,208},
{0,212},
{0,192},
{0,220},
{0,224},
{0,228},
{0,232},
{1,236},
{0,240},
{2,244},
{0,248},
{1,336},
{0,128},
{0,132},
{0,272},
{1,280},
{0,432},
{1,296},
{1,304},
{1,312},
{0,160},
{0,328},
{1,336},
{0,344},
{0,176},
{0,600},
{0,184},
{0,376},
{0,192},
{0, 56},
{0,480},
{0,408},
{0,208},
{0,424},
{0,384},
{0,440},
{0,672},
{0,456},
{0,464},
{0,472},
{0,240},
{0,488},
{0,496},
{0,336},
{0,128},
{0,260},
{0,528},
{0,268},
{0,816},
{0,276},
{0,560},
{0,284},
{0,960},
{0,292},
{0,888},
{0,240},
{0,304},
{0,924},
{0,468},
{0,316},
{0,160},
{0,192},
{1,492},
{0,332},
{0,672},
{0,340},
{0,172},
{0,348},
{0,352},
{0,356},
{0,240},
{0,364},
{0,368},
{0,372},
{0,188},
{0,380},
{0,960}
};
static int gi_initial[188] = {
  13,
  19,
   5,
  23,
  25,
  31,
  27,
  35,
  33,
  13,
  73,
  47,
  43,
 125,
  47,
 141,
  17,
  65,
 103,
  71,
  63,
  79,
  47,
  83,
 143,
  89,
  95,
  47,
 215,
 101,
  33,
  55,
  93,
 113,
  97,
 141,
 103,
 199,
 279,
  65,
 223,
 127,
 139,
  71,
 341,
 191,
 257,
  77,
 157,
 119,
 201,
 197,
 143,
 305,
 147,
 269,
 213,
 219,
 139,
  95,
  83,
 103,
  87,
 161,
  93,
 113,
 275,
 119,
 267,
 295,
 107,
 199,
 199,
 231,
 117,
 265,
 123,
  97,
 229,
  77,
 345,
  65,
 247,
 327,
 143,
  73,
 265,
  89,
 181,
 189,
 139,
  95,
  83,
 375,
 207,
 107,
  93,
 115,
  97,
 439,
 103,
 463,
 107,
 131,
 209,
 137,
 895,
 119,
  41,
  97,
 127,
 287,
1009,
 223,
 137,
 141,
 143,
 173,
 399,
 151,
 669,
 309,
 533,
  95,
  83,
 307,
 629,
 469,
 811,
 721,
 649,
 119,
 191,
 631,
 315,
 131,
 329,
 137,
 345,
 143,
  41,
 351,
 647,
 155,
 263,
 643,
 477,
 393,
 541,
 503,
 415,
 451,
 607,
 623,
 295,
  95,
 163,
 307,
 167,
 885,
 173,
 513,
 499,
 817,
 183,
 515,
 191,
 189,
 501,
 361,
 197,
 119,
 127,
1015,
 207,
 587,
 213,
 107,
 217,
 221,
 223,
 281,
 271,
 507,
 233,
 117,
 237,
 743
};
const int f2[188] = {
	10,
	12,
	42,
	16,
	18,
	20,
	22,
	24,
	26,
	84,
	90,
	32,
	34,
	108,
	38,
	120,
	84,
	44,
	46,
	48,
	50,
	52,
	36,
	56,
	58,
	60,
	62,
	32,
	198,
	68,
	210,
	36,
	74,
	76,
	78,
	120,
	82,
	84,
	86,
	44,
	90,
	46,
	94,
	48,
	98,
	40,
	102,
	52,
	106,
	72,
	110,
	168,
	114,
	58,
	118,
	180,
	122,
	62,
	84,
	64,
	66,
	68,
	420,
	96,
	74,
	76,
	234,
	80,
	82,
	252,
	86,
	44,
	120,
	92,
	94,
	48,
	98,
	80,
	102,
	52,
	106,
	48,
	110,
	112,
	114,
	58,
	118,
	60,
	122,
	124,
	84,
	64,
	66,
	204,
	140,
	72,
	74,
	76,
	78,
	240,
	82,
	252,
	86,
	88,
	60,
	92,
	846,
	48,
	28,
	80,
	102,
	104,
	954,
	96,
	110,
	112,
	114,
	116,
	354,
	120,
	610,
	124,
	420,
	64,
	66,
	136,
	420,
	216,
	444,
	456,
	468,
	80,
	164,
	504,
	172,
	88,
	300,
	92,
	188,
	96,
	28,
	240,
	204,
	104,
	212,
	192,
	220,
	336,
	228,
	232,
	236,
	120,
	244,
	248,
	168,
	64,
	130,
	264,
	134,
	408,
	138,
	280,
	142,
	480,
	146,
	444,
	120,
	152,
	462,
	234,
	158,
	80,
	96,
	902,
	166,
	336,
	170,
	86,
	174,
	176,
	178,
	120,
	182,
	184,
	186,
	94,
	190,
	480
};
void bit2byte(unsigned char *byte_stream, char *bit_stream,
	      unsigned int bit_len) {
  unsigned int i;
  unsigned char tmp = 0x0;
  for (i = 1; i <= bit_len; i++) {
    if (i % 8 != 0)
      tmp |= (*(bit_stream + i - 1) & 0x1) << (8 - i % 8);
    else {
      *(byte_stream + i / 8 - 1) = tmp | (*(bit_stream + i - 1) & 0x1);
      tmp = 0x0;
      }
  }
}
#define idx(x) x <= 512 ? (x -40)/8 : x <= 1024 ? (59 + (x - 512)/16) : x <= 2048 ? (91 + (x - 1024)/32) : (123 + (x - 2048)/64) 
int main()
{
  const unsigned int turbo_base = 0x80000000;
  volatile unsigned int *turbo_addr = (volatile unsigned int *)turbo_base;
  turbo_addr[1] = sys;
  turbo_addr[2] = par1;
  turbo_addr[3] = par2;
  turbo_addr[4] = 0xC00000;
  turbo_addr[5] = 8;  // iter
  turbo_addr[6] = 40; // cb len
  turbo_addr[7] = 40; // sb len
  turbo_addr[8] = 1;   // sb num
  turbo_addr[9] = 40; // sw len1
  turbo_addr[10] = 40;  // sw len2
  turbo_addr[11] = 1;  // sw num
  turbo_addr[12] = 0; // pre len
  *(float*)&turbo_addr[13] = 0.75; // scale
  turbo_addr[14] = f2[idx(40)];  // f2
  turbo_addr[15] = e1e2_initial[idx(40)][0]; // e1
  turbo_addr[16] = e1e2_initial[idx(40)][1]; // e2
  for (unsigned i = 0; i < 8; i++)
    turbo_addr[17 + i] = beta_initial_value_cb1[i];
  for (unsigned i = 0; i < 8; i++)
    turbo_addr[25 + i] = beta_initial_value_cb2[i];
  turbo_addr[33] = gi_initial[idx(40)] | (ts_initial[idx(40)][0]<<16) | (as_initial[idx(40)][0]<<24);
  turbo_addr[34] = gi_initial[idx(40)] | (ts_initial[idx(40)][1]<<16) | (as_initial[idx(40)][1]<<24);
  turbo_addr[35] = gi_initial[idx(40)] | (ts_initial[idx(40)][2]<<16) | (as_initial[idx(40)][2]<<24);
  turbo_addr[36] = gi_initial[idx(40)] | (ts_initial[idx(40)][3]<<16) | (as_initial[idx(40)][3]<<24);
  turbo_addr[37] = 0; //crc type
  turbo_addr[38] = 0; //crc switch
  turbo_addr[0] = 1;
  while(turbo_addr[0]);
  const unsigned int out_base = 0xC00000;
  volatile unsigned char *out_addr = (volatile unsigned char *)out_base;
  unsigned char out1[40/8];
  bit2byte(&out1, &out, 40);
  int j = 0;
  for (unsigned i = 0; i < 40/8; i++) {
    if (out_addr[i] != out1[i])
      printf("%d: %x, %x\n", i, out_addr[i], out1[i]);
    else
      j++;
  }
  if (j == 40/8)
    printf("passed!\n");
  return 0;
}

