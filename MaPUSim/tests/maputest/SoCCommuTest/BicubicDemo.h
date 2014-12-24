/*
 * BicubicDemo.h
 *
 *  Created on: Dec 4, 2013
 *      Author: wangl
 */

#ifndef BICUBICDEMO_H_
#define BICUBICDEMO_H_


#include <stdio.h>

typedef unsigned char  BYTE;

typedef u_int16_t WORD;





// BMPÕºœÒ∏˜≤ø∑÷Àµ√˜»Áœ¬



/***********

    µ⁄“ª≤ø∑÷    ŒªÕºŒƒº˛Õ∑

∏√Ω·ππµƒ≥§∂» «πÃ∂®µƒ£¨Œ™14∏ˆ◊÷Ω⁄£¨∏˜∏ˆ”Úµƒ“¿¥Œ»Áœ¬£∫

    2byte   £∫Œƒº˛¿‡–Õ£¨±ÿ–Î «0x4d42£¨º¥◊÷∑˚¥Æ"BM"°£

    4byte   £∫’˚∏ˆŒƒº˛¥Û–°

    4byte   £∫±£¡Ù◊÷£¨Œ™0

    4byte   £∫¥”Œƒº˛Õ∑µΩ µº µƒŒªÕºÕºœÒ ˝æ›µƒ∆´“∆◊÷Ω⁄ ˝°£

*************/



typedef struct

{

  //WORD resevered;
    int32_t imageSize;

    int32_t blank;

    int32_t startPosition;

}BmpHead;



/*********************

    µ⁄∂˛≤ø∑÷    ŒªÕº–≈œ¢Õ∑

∏√Ω·ππµƒ≥§∂»“≤ «πÃ∂®µƒ£¨Œ™40∏ˆ◊÷Ω⁄£¨∏˜∏ˆ”Úµƒ“¿¥ŒÀµ√˜»Áœ¬£∫

    4byte   £∫±æΩ·ππµƒ≥§∂»£¨÷µŒ™40

    4byte   £∫ÕºœÒµƒøÌ∂» «∂‡…ŸœÛÀÿ°£

    4byte   £∫ÕºœÒµƒ∏ﬂ∂» «∂‡…ŸœÛÀÿ°£

    2Byte   £∫±ÿ–Î «1°£

    2Byte   £∫±Ì æ—’…´ ±”√µΩµƒŒª ˝£¨≥£”√µƒ÷µŒ™1(∫⁄∞◊∂˛…´Õº)°¢4(16…´Õº)°¢8(256…´Õº)°¢24(’Ê≤ …´Õº)°£

    4byte   £∫÷∏∂®ŒªÕº «∑Ò—πÀı£¨”––ß÷µŒ™BI_RGB£¨BI_RLE8£¨BI_RLE4£¨BI_BITFIELDS°£WindowsŒªÕºø…≤…”√RLE4∫ÕRLE8µƒ—πÀı∏Ò Ω£¨BI_RGB±Ì æ≤ª—πÀı°£

    4byte   £∫÷∏∂® µº µƒŒªÕºÕºœÒ ˝æ›’º”√µƒ◊÷Ω⁄ ˝£¨ø…”√“‘œ¬µƒπ´ Ωº∆À„≥ˆ¿¥£∫

              ÕºœÒ ˝æ› = Width' * Height * ±Ì æ√ø∏ˆœÛÀÿ—’…´’º”√µƒbyte ˝(º¥—’…´Œª ˝/8,24bitÕºŒ™3£¨256…´Œ™1£©

              “™◊¢“‚µƒ «£∫…œ ˆπ´ Ω÷–µƒbiWidth'±ÿ–Î «4µƒ’˚ ˝±∂(≤ª «biWidth£¨∂¯ «¥Û”⁄ªÚµ»”⁄biWidthµƒ◊Ó–°4µƒ’˚ ˝±∂)°£

              »Áπ˚biCompressionŒ™BI_RGB£¨‘Ú∏√œÓø…ƒ‹Œ™0°£

    4byte   £∫ƒø±Í…Ë±∏µƒÀÆ∆Ω∑÷±Ê¬ °£

    4byte   £∫ƒø±Í…Ë±∏µƒ¥π÷±∑÷±Ê¬ °£

    4byte   £∫±æÕºœÒ µº ”√µΩµƒ—’…´ ˝£¨»Áπ˚∏√÷µŒ™0£¨‘Ú”√µΩµƒ—’…´ ˝Œ™2µƒ(—’…´Œª ˝)¥Œ√›,»Á—’…´Œª ˝Œ™8£¨2^8=256,º¥256…´µƒŒªÕº

    4byte   £∫÷∏∂®±æÕºœÒ÷–÷ÿ“™µƒ—’…´ ˝£¨»Áπ˚∏√÷µŒ™0£¨‘Ú»œŒ™À˘”–µƒ—’…´∂º «÷ÿ“™µƒ°£

***********************************/

typedef struct

{

    int32_t    Length;

    int32_t    width;

    int32_t    height;

    WORD    colorPlane;

    WORD    bitColor;

    int32_t    zipFormat;

    int32_t    realSize;

    int32_t    xPels;

    int32_t    yPels;

    int32_t    colorUse;

    int32_t    colorImportant;
}InfoHead;


/***************************

    µ⁄»˝≤ø∑÷    µ˜…´≈ÃΩ·ππ

    ∂‘”⁄256…´BMPŒªÕº£¨—’…´Œª ˝Œ™8£¨–Ë“™2^8 = 256∏ˆµ˜…´≈Ã£ª

    ∂‘”⁄24bitBMPŒªÕº£¨∏˜œÛÀÿRGB÷µ÷±Ω”±£¥Ê‘⁄ÕºœÒ ˝æ›«¯£¨≤ª–Ë“™µ˜…´≈Ã£¨≤ª¥Ê‘⁄µ˜…´≈Ã«¯

    rgbBlue£∫   ∏√—’…´µƒ¿∂…´∑÷¡ø°£

    rgbGreen£∫  ∏√—’…´µƒ¬Ã…´∑÷¡ø°£

    rgbRed£∫    ∏√—’…´µƒ∫Ï…´∑÷¡ø°£

    rgbReserved£∫±£¡Ù÷µ°£

************************/

typedef struct

{

         BYTE   rgbBlue;

         BYTE   rgbGreen;

         BYTE   rgbRed;

         BYTE   rgbReserved;

}RGBMixPlate;



/****************************

    µ⁄Àƒ≤ø∑÷    ÕºœÒ ˝æ›«¯

    ∂‘”⁄”√µΩµ˜…´∞ÂµƒŒªÕº£¨ÕºœÒ ˝æ›æÕ «∏√œÛÀÿ—’…´‘⁄µ˜…´∞Â÷–µƒÀ˜“˝÷µ£ª

    ∂‘”⁄’Ê≤ …´Õº£¨ÕºœÒ ˝æ›æÕ « µº µƒR°¢G°¢B÷µ°£

        2…´Õº£¨”√1ŒªæÕø…“‘±Ì æ∏√œÛÀÿµƒ—’…´£¨À˘“‘1∏ˆ◊÷Ω⁄ø…“‘±Ì æ8∏ˆœÛÀÿ°£

        16…´Õº£¨”√4Œªø…“‘±Ì æ“ª∏ˆœÛÀÿµƒ—’…´£¨À˘“‘1∏ˆ◊÷Ω⁄ø…“‘±Ì æ2∏ˆœÛÀÿ°£

        256…´Õº£¨1∏ˆ◊÷Ω⁄∏’∫√ø…“‘±Ì æ1∏ˆœÛÀÿ°£

        ’Ê≤ …´Õº£¨3∏ˆ◊÷Ω⁄≤≈ƒ‹±Ì æ1∏ˆœÛÀÿ°£

****************************/

//void WriteBmp(char* pFileName,unsigned char
  void WriteBMP(const char* pFileName,unsigned char** ppBuff,int nW,int nH);
  void WriteBMP(const char* pFileName,int** ppBuff,int nW,int nH);



#endif /* BICUBICDEMO_H_ */
