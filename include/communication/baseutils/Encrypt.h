#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include "communication/baseutils/FSPublic.h"

#define MAC_ENCRYPT_STR "#encrypted by FileEncrypt"

/** 
 * @class Encrypt
 * @brief 加密类
 */
class Encrypt {
public:

    /** 
     * @brief 加密函数，默认密码为dmkxdev
     * @param byIn 输入明文和输出密文s
     * @param iLen 加密长度
     * @param bEncrypt 是否加密
     * @param byKey 密钥，默认为dmkxdev1
     * @param iKeyLen 密钥长度，默认为8
     */

    /*                       rdi        rsi        rdx              rcx                                        r8   */
    static void Encode(BYTE* byIn, int iLen, bool bEncrypt = true, BYTE* byKey = (unsigned char*) "dmkxdev1", int iKeyLen = 8) {//i add it
        if (bEncrypt) {
            assert(byIn && byKey && iKeyLen > 0 && iLen > 0);
            int len_idx = 0;
            do {
                *byIn = (*byIn)^(*(len_idx++ % iKeyLen + byKey));
                byIn++;
            } while (iLen > len_idx);
        }
    }

    /** 
     * @brief 解密函数，默认密码为dmkxdev
     * @param byIn 输入密文和输出明文
     * @param iLen 密文长度
     * @param bEncrypt 是否解密
     * @param byKey 密钥，默认为dmkxdev1
     * @param iKeyLen 密钥长度，默认为8
     */

    /*                     rdi          rsi           rdx                   rcx                                r8    */
    static void Decode(BYTE* byIn, int iLen, bool bEncrypt = true, BYTE* byKey = (unsigned char*) "dmkxdev1", int iKeyLen = 8) {//i add it 
        if (bEncrypt) {
            Encrypt::Encode(byIn, iLen, bEncrypt, byKey, iKeyLen);
        }
    }
};

#endif // ENCRYPT_H_

