/*! \file lzowrapper.h
\brief lzo压缩库的包装类文件

  本头文件包装了lzo库，包含文件压缩和内存压缩类
  
    add by tang.chun@zte.com.cn
 */


#ifndef LZOWRAPPER_H_
#define LZOWRAPPER_H_

#ifdef _WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4018)
#endif
#include "communication/lzo/lzoconf.h"
#include "communication/lzo/lzo1x.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <map>
using namespace std;

#define BLOCKSIZE (64*1024) /**< 默认块大小64k */
#define COMPRESS_FAIL -99999 /**< 压缩失败返回值 */
#define COMPRESS_LEVEL_NO_COMPRESS 0
#define COMPRESS_LEVEL_FAST 1
#define COMPRESS_LEVEL_MAX  9

static const unsigned char magiclzop[] = {0xe9, 0x4c, 0x5a, 0x4f}; //!< 压缩块魔术字

/**
 *  压缩块结构体
 */
struct CompressBlock {
    unsigned char magic[sizeof (magiclzop)]; //!< 魔术字0
    unsigned char level; //!< 压缩级别 //4
    unsigned char checksum[4]; //!< 校验和  //5
    unsigned char orglen[4]; //!< 原始长度// //9
    unsigned char len[4]; //!< 压缩后长度    //0d
    unsigned char totallen[4]; //!< 压缩后包括结构体的长度//0x11h
    unsigned char buf; //!< 指向压缩后buffer的第一字节// // 0x15h  //0x16h

    /**
     * 写入一个32位的整数到内存中
     * @param sDest 目标内存地址.
     * @param in 要写入的32位整数
     */
    static void write32(unsigned char* sDest, unsigned int in) {
        assert(sDest != NULL);

        /*
        sDest[3] = (unsigned char) ((in >>  0) & 0xff);
        sDest[2] = (unsigned char) ((in >>  8) & 0xff);
        sDest[1] = (unsigned char) ((in >> 16) & 0xff);
        sDest[0] = (unsigned char) ((in >> 24) & 0xff);
         */
        UINT dst = htonl_ex(in);
        memcpy(sDest, &dst, 4);
    }

    /**
     * 从内存中读出一个32位整数
     * @param sSrc 目标内存地址.
     * @return 读出的32位整数
     */
    static unsigned int read32(const unsigned char* sSrc) {
        unsigned int out = 0;
        /*
        out  = (unsigned int) sSrc[3] <<  0;
        out |= (unsigned int) sSrc[2] <<  8;
        out |= (unsigned int) sSrc[1] << 16;
        out |= (unsigned int) sSrc[0] << 24;
         */
        memcpy(&out, sSrc, 4);
        out = ntohl_ex(out);
        return out;
    }
};

/**
 *  CLzoWrapper LZO库的包装类
 */
class CLzoWrapper {
    unsigned char m_byLevel; /*!< 压缩级别 */
    unsigned char* m_wrkmem; /*!< 内部使用的内存区 */
public:

    /**
     *  构造函数
     */
    CLzoWrapper() : m_wrkmem(0) {
    }

    /**
     *  析构函数
     */
    ~CLzoWrapper() {
        if (m_wrkmem)
            free(m_wrkmem);
    }

    /**
     * 初始化设置压缩的级别(0,1或9),并分配内部使用内存
     *    0 不压缩
     *    1 最快压缩
     *    9 最大压缩
     * @param byLevel 级别.
     * @return <0 失败，=0 成功
     */
    int Init(unsigned char byLevel) {
        if (lzo_init() != LZO_E_OK) {
            return -1;
        }

        if (byLevel != 0 && byLevel != 1 && byLevel != 9) {
            return -2;
        }
        m_byLevel = byLevel;

        int wrk_len;
        if (byLevel == COMPRESS_LEVEL_MAX) {
            wrk_len = LZO1X_999_MEM_COMPRESS;
        } else if (byLevel == COMPRESS_LEVEL_FAST) {
            wrk_len = LZO1X_1_MEM_COMPRESS;
        } else {
            wrk_len = 1;
        }

        m_wrkmem = (unsigned char*) malloc(wrk_len);
        if (m_wrkmem == NULL) {
            return -3;
        }

        return 0;
    }

    /**
     * 获取压缩后需要的长度,以便使用者分配内存
     * @param in_len 要压缩内容长度.
     * @return 压缩后最大长度
     */
    int getCompressLen(int in_len) {
        return in_len + in_len / 16 + 64 + 3 + sizeof (CompressBlock);
    }

    /**
     * 压缩一块内存
     * @param in 要压缩内容
     * @param in_len 要压缩内容长度
     * @param out 压缩后内容输出内存，由使用者分配
     * @param out_len 压缩后内容的长度
     * @return <0 失败， =0 成功
     */
    int Compress(const unsigned char* in, int in_len, unsigned char* out, int& out_len) {
        assert(in != 0 && out != 0 && in_len > 0 && out_len > 0);

        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            out_len = in_len;
            memcpy(out, in, in_len);
            return 0;
        }

        int r = 0;
        CompressBlock *pCompressBlock = (CompressBlock *) out;
        unsigned int checksum = 0;

        // 写入配置信息
        memcpy(pCompressBlock->magic, magiclzop, sizeof (magiclzop));
        pCompressBlock->level = m_byLevel; // 压缩级别 

        unsigned long realoutlen = out_len;
        unsigned char* outbuf = &(pCompressBlock->buf);
        if (m_byLevel == COMPRESS_LEVEL_MAX) {
            r = lzo1x_999_compress(in, in_len, outbuf, &realoutlen, m_wrkmem);
        } else if (m_byLevel == COMPRESS_LEVEL_FAST) {
            r = lzo1x_1_compress(in, in_len, outbuf, &realoutlen, m_wrkmem);
        }

        if (r != LZO_E_OK || realoutlen > in_len + in_len / 16 + 64 + 3 + sizeof (CompressBlock)) {
            return -1;
        }

        int nLen = realoutlen;
        checksum = 0; //lzo_adler32(checksum, outbuf, realoutlen);

        // 填充结构体
        CompressBlock::write32(pCompressBlock->checksum, checksum);
        CompressBlock::write32(pCompressBlock->orglen, in_len);
        CompressBlock::write32(pCompressBlock->len, nLen);

        out_len = sizeof (CompressBlock) + nLen; // 最终长度

        // 压缩块整个长度
        CompressBlock::write32(pCompressBlock->totallen, out_len);

        return 0;
    }

    /**
     * 获取压缩块原始长度
     * @param in 压缩内容
     * @param in_len 压缩内容长度
     * @return 原始内容长度
     */
    int getOrgLen(const unsigned char* in, int in_len) {
        assert(in_len > sizeof (CompressBlock));
        CompressBlock *pCompressBlock = (CompressBlock *) in;

        //  非压缩数据
        if (memcmp(pCompressBlock->magic, magiclzop, sizeof (magiclzop)) != 0) {
            return in_len;
        }

        return CompressBlock::read32(pCompressBlock->orglen);
    }

    /**
     * 获取压缩块自身长度，用于读取时确定需要读多长
     * @param in 压缩内容
     * @param in_len 已有压缩内容长度，必须大于sizeof(CompressBlock)
     * @return 整个压缩块长度 
     * -1 非压缩数据长度由用户自行决定
     */
    int getPkgLen(const unsigned char* in, int in_len) {
        assert(in_len >= sizeof (CompressBlock));
        CompressBlock *pCompressBlock = (CompressBlock *) in;
        //  非压缩数据，由用户自行处理长度
        if (memcmp(pCompressBlock->magic, magiclzop, sizeof (magiclzop)) != 0) {
            return -1;
        }
        return CompressBlock::read32(pCompressBlock->totallen);
    }

    /**
     * 解压
     * @param in 压缩内容
     * @param in_len 压缩内容长度
     * @param out 解压后内容输出内存，由使用者分配
     * @param out_len 解压后内容的长度
     * @return <0 失败， =0 成功
     */
    int DeCompress(const unsigned char* in, int in_len, unsigned char* out, int& out_len) {
        assert(in != 0 && out != 0 && in_len > 0 && out_len > 0);

        CompressBlock *pCompressBlock = (CompressBlock *) in;

        //  非压缩数据
        if (memcmp(pCompressBlock->magic, magiclzop, sizeof (magiclzop)) != 0) {
            out_len = in_len;
            memcpy(out, in, in_len);
            return 0;
        }

        unsigned char* data = &(pCompressBlock->buf);
        int datalen = CompressBlock::read32(pCompressBlock->len);
        unsigned int blockchecksum = CompressBlock::read32(pCompressBlock->checksum);
        int orglen = CompressBlock::read32(pCompressBlock->orglen);

        if (memcmp(pCompressBlock->magic, magiclzop, sizeof (magiclzop)) != 0) {
            return -1; // 数据不正确
        }

        unsigned checksum = 0;
        checksum = 0; //lzo_adler32(checksum, data, datalen);
        if (checksum != blockchecksum) {
            return -2; // 数据损坏无法解开
        }

        unsigned long new_len = out_len;
        int r = lzo1x_decompress_safe(data, datalen, out, &new_len, NULL);
        if (r != LZO_E_OK || new_len != (unsigned int) orglen) {
            cout << "new len is" << new_len << "orglen" << orglen << endl;
            return -3; // 解压不正确
        }

        out_len = new_len;

        return 0;
    }

};


typedef int (*OutDataHandler)(void* arg, const unsigned char* outBuf, int outlen); //!< 内存压缩使用的输出函数

/**
 *  CLzoMem 内存压缩类
 */
class CLzoMem {
    unsigned char *m_pInBuf; //!< 用来放置用户待压缩或解压的数据
    int m_nInBufUsedLen; //!< 已经放入多少数据
    int m_nMaxBufLen; //!< 缓冲区最大大小

    unsigned char *m_pOutBuf; //!< 在压缩时用来放置压缩好的数据，在解压时放置解开后的数据
    int m_nOutBufLen; //!< OutBuf中数据大小

    int m_nBlockSize; //!< 块大小，超过这个大小就要开始压缩

    CLzoWrapper m_lzo; //!< 压缩类
    OutDataHandler m_outhandler; //!< 输出处理函数

    unsigned char m_byLevel;

public:

    /**
     * 构造函数
     */
    CLzoMem() : m_pInBuf(0),
    m_nInBufUsedLen(0),
    m_nMaxBufLen(64 * 1024),
    m_pOutBuf(0),
    m_nOutBufLen(0),
    m_nBlockSize(64 * 1024) {
    }

    /**
     * 析构函数
     */
    ~CLzoMem() {
        free(m_pInBuf);
        free(m_pOutBuf);
    }

    /**
     * 初始化
     * @param pFunc 输出函数指针
     * @param blocksize 压缩块大小，即待压缩内容达到此大小后开始压缩
     * @param byLevel 压缩级别
     * @return <0 失败， =0 成功
     */
    int Init(OutDataHandler pFunc = NULL, unsigned int blocksize = 64 * 1024, unsigned char byLevel = COMPRESS_LEVEL_FAST) {
        m_byLevel = byLevel;
        m_outhandler = pFunc;
        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return 0;
        }

        if (m_lzo.Init(byLevel) != 0) {
            return -1;
        }
        m_nBlockSize = blocksize;

        m_nMaxBufLen = m_lzo.getCompressLen(blocksize) * 2;
        m_pInBuf = (unsigned char*) malloc(m_nMaxBufLen);
        if (m_pInBuf == NULL) {
            return -2;
        }
        m_nInBufUsedLen = 0;

        m_pOutBuf = (unsigned char*) malloc(m_nMaxBufLen);
        if (m_pOutBuf == NULL) {
            return -3;
        }
        m_nOutBufLen = 0;
        return 0;
    }

    // 

    /**
     * 如果压缩出错，此函数使对象重新可用
     */
    void reset() {
        m_nInBufUsedLen = 0;
        m_nOutBufLen = 0;
    }

    /**
     * \brief 压缩函数，用户调用此函数往压缩缓冲区添加内容，一旦待压缩缓冲区大于块长度，则开始压缩，
     *  压缩后的内容传递给输出函数，也可以使用forceout为真来强制压缩不满缓冲区内容
     * @param in 待压缩内容
     * @param in_len 待压缩内容长度
     * @param forceout 当为真时，不等满足block大小就开始输出，否则，必须大于blocksize时才输出
     * @param arg 传递给输出函数的指针
     * @return 
     *         -# -99999压缩本身错误，
     *         -# 其他!=0为输出函数产生的错误 
     *         -# =0 成功
     */
    int Compress(const unsigned char *in, int in_len, bool forceout = false, void * arg = 0) {
        if (m_outhandler == NULL) {
            return -1;
        }

        // 不压缩
        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return m_outhandler(arg, in, in_len);
        }

        int nRet = 0;
        const unsigned char *pin = in;
        int nRemainLen = in_len;

        // 超过块大小，压缩此块
        while (m_nInBufUsedLen + nRemainLen >= m_nBlockSize) {
            // 拷贝一段放入压缩缓冲区
            if (m_nInBufUsedLen < m_nBlockSize) {
                int copylen = m_nBlockSize - m_nInBufUsedLen;
                memcpy(m_pInBuf + m_nInBufUsedLen,
                        pin,
                        copylen);
                nRemainLen -= copylen;
                pin += copylen;
                m_nInBufUsedLen += copylen;
            }

            assert(pin - in <= in_len);

            // 压缩
            nRet = CompressOutPut(arg);
            if (nRet != 0) {
                return nRet;
            }
        }


        // 如果没有满则放入缓冲区
        if (nRemainLen > 0) {
            memcpy(m_pInBuf + m_nInBufUsedLen, pin, nRemainLen);
            m_nInBufUsedLen += nRemainLen;

        }

        // 强制输出
        if (forceout) {
            nRet = CompressOutPut(arg);
            if (nRet != 0) {
                return nRet;
            }
        }

        return 0;
    }

    /**
     * \brief 解压函数，用户调用此函数往待解压缓冲区添加内容，一旦待解压缓冲区可解压，则开始解压，
     *  解压后的内容传递给输出函数
     * @param in 待解压内容
     * @param in_len 待压缩内容长度
     * @param arg 传递给输出函数的指针
     * @return 
     *         -# -99999压缩本身错误，
     *         -# 其他!=0为输出函数产生的错误 
     *         -# =0 成功
     */
    int DeCompress(const unsigned char* in, int in_len, void *arg = 0) {
        if (in == NULL || m_outhandler == NULL || in_len <= 0) {
            return -1;
        }

        // 不压缩
        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return m_outhandler(arg, in, in_len);
        }


        int nRet = 0;
        const unsigned char *pin = in;
        int nRemainLen = in_len;

        // 超过块大小，压缩此块
        while (m_nInBufUsedLen + nRemainLen > sizeof (CompressBlock)) {
            // 填满头部
            if (m_nInBufUsedLen < sizeof (CompressBlock)) {
                int copylen = sizeof (CompressBlock) - m_nInBufUsedLen;
                memcpy(m_pInBuf + m_nInBufUsedLen,
                        pin,
                        copylen);
                nRemainLen -= copylen;
                pin += copylen;
                m_nInBufUsedLen += copylen;
            }

            int nTotalLen = m_lzo.getPkgLen(m_pInBuf, m_nInBufUsedLen);
            if (m_nInBufUsedLen + nRemainLen < nTotalLen) // 如果长度不够解压，则退出
            {
                break;
            }

            // 如果未压缩，则使用m_nInBufUsedLen
            if (nTotalLen < 0) {
                nTotalLen = m_nInBufUsedLen;
            }

            // 拷贝部分长度以使得满足一个包的长度
            if (m_nInBufUsedLen < nTotalLen) {
                int copylen = nTotalLen - m_nInBufUsedLen;
                memcpy(m_pInBuf + m_nInBufUsedLen,
                        pin,
                        copylen);
                nRemainLen -= copylen;
                pin += copylen;
                m_nInBufUsedLen += copylen;
            }

            nRet = DeCompressOutPut(arg, nTotalLen);
            if (nRet != 0) {
                return nRet;
            }
        }

        // 如果没有满则放入缓冲区
        memcpy(m_pInBuf + m_nInBufUsedLen, pin, nRemainLen);
        m_nInBufUsedLen += nRemainLen;

        return 0;
    }

    // 当返回值==2时，表示有压缩或解压好的数据，可以取outBuff，如果不取，下次调用会冲掉
    //         < 0 错误，每次放入数据必须小于blocksize，否则缓冲区放不下
    //         == 0, 表示数据放入了缓冲区，等待解压或压缩，
    int Compress(const unsigned char *in, int in_len, unsigned char* &outBuf, int &outlen);

    // 将缓冲区中不满blocksize的数据压缩并输出
    int CompressFlush(unsigned char* &outBuf, int &outlen);

    // 解压
    // 当返回值==2时，表示有压缩或解压好的数据，可以取outBuff，多余的数据放入缓冲区中
    //         < 0 错误，每次放入数据必须小于blocksize，否则缓冲区放不下
    //         == 0, 表示数据放入了缓冲区，等待解压或压缩，
    int DeCompress(const unsigned char* in, int in_len, unsigned char* &out, int& out_len);

    // 将缓冲区中已经解开的数据输出
    int DeCompressFlush(unsigned char* &outBuf, int &outlen);

private:

    int CompressBuffer() {
        int nOutLen = m_nMaxBufLen;

        // 压缩
        if (m_lzo.Compress(m_pInBuf, m_nInBufUsedLen, m_pOutBuf, nOutLen) != 0) {
            return -1;
        }

        m_nOutBufLen = nOutLen;
        m_nInBufUsedLen = 0;

        return 0;
    }

    int CompressOutPut(void *arg) {
        int nRet = 0;
        // 压缩
        if (CompressBuffer() != 0) {
            return COMPRESS_FAIL;
        } else {
            // 调用输出函数
            nRet = m_outhandler(arg, m_pOutBuf, m_nOutBufLen);
            if (nRet != 0) {
                return nRet;
            } else {
                m_nOutBufLen = 0;
            }
        }

        return 0;
    }

    int DeCompressOutPut(void* arg, int len) {
        int nRet = 0;
        // 压缩
        if (DeCompressBuffer(len) != 0) {
            return COMPRESS_FAIL;
        } else {
            // 调用输出函数
            nRet = m_outhandler(arg, m_pOutBuf, m_nOutBufLen);
            if (nRet != 0) {
                return nRet;
            } else {
                m_nOutBufLen = 0;
            }
        }

        return 0;
    }

    int DeCompressBuffer(int usedlen) {
        int nOutLen = m_nMaxBufLen;

        // 压缩
        if (m_lzo.DeCompress(m_pInBuf, usedlen, m_pOutBuf, nOutLen) != 0) {
            return -1;
        }

        m_nOutBufLen = nOutLen;

        // 将未使用的移到前面
        memmove(m_pInBuf, m_pInBuf + usedlen, m_nInBufUsedLen - usedlen);
        m_nInBufUsedLen = m_nInBufUsedLen - usedlen;
        return 0;
    }
};

// 压缩
// 当返回值==2时，表示有压缩或解压好的数据，可以取outBuff，如果不取，下次调用会冲掉
//         < 0 错误，每次放入数据必须小于blocksize，否则缓冲区放不下
//         == 0, 表示数据放入了缓冲区，等待解压或压缩，

inline int CLzoMem::Compress(const unsigned char *in, int in_len, unsigned char* &outBuf, int &outlen) {
    if (in == NULL || in_len > m_nBlockSize || in_len < 0) {
        return -1;
    }

    int nRet = 0;
    outBuf = NULL;
    outlen = 0;
    const unsigned char* pin = in;
    int nRemainLen = in_len;

    // 超过块大小，压缩此块并将写入文件
    if ((m_nInBufUsedLen + in_len) >= m_nBlockSize) {
        // 拷贝一段放入压缩缓冲区
        if (m_nInBufUsedLen < m_nBlockSize) {
            int copylen = m_nBlockSize - m_nInBufUsedLen;
            memcpy(m_pInBuf + m_nInBufUsedLen,
                    pin,
                    copylen);
            nRemainLen -= copylen;
            pin += copylen;
            m_nInBufUsedLen += copylen;
        }

        if (CompressBuffer() != 0) {
            return -2;
        } else {
            outBuf = m_pOutBuf;
            outlen = m_nOutBufLen;
            nRet = 2;
        }
    }

    // 如果没有满则放入缓冲区
    memcpy(m_pInBuf + m_nInBufUsedLen, pin, nRemainLen);
    m_nInBufUsedLen += nRemainLen;

    return nRet;
}

// 将缓冲区中不满blocksize的数据压缩并输出

inline int CLzoMem::CompressFlush(unsigned char* &outBuf, int &outlen) {
    outBuf = NULL;
    outlen = 0;

    if (m_nInBufUsedLen == 0) {
        return 0;
    }

    if (CompressBuffer() != 0) {
        return -2;
    } else {
        outBuf = m_pOutBuf;
        outlen = m_nOutBufLen;
        return 2;
    }
}

// 解压
// 当返回值==2时，表示有压缩或解压好的数据，可以取outBuff，多余的数据放入缓冲区中
//         -1 错误，每次放入数据必须小于等于blocksize，否则缓冲区放不下
//          -2 需要调用 DeCompressFlush将已经解压数据取出
//         == 0, 表示数据放入了缓冲区，等待解压或压缩，

inline int CLzoMem::DeCompress(const unsigned char* in, int in_len, unsigned char* &out, int& out_len) {
    out = 0;
    out_len = 0;

    // 如果长度足够解压，则解开，否则放入缓冲区
    int nTotalLen = 0;
    if (m_nInBufUsedLen + in_len > sizeof (CompressBlock)
            && m_nInBufUsedLen + in_len < m_nMaxBufLen) {
        memcpy(m_pInBuf + m_nInBufUsedLen, in, in_len);
        m_nInBufUsedLen += in_len;

        nTotalLen = m_lzo.getPkgLen(m_pInBuf, m_nInBufUsedLen);
        if (nTotalLen == -1) // 未压缩
        {
            nTotalLen = m_nInBufUsedLen;
        }

        if (m_nInBufUsedLen >= nTotalLen) // 可以解开
        {
            if (DeCompressBuffer(nTotalLen) != 0) // 解压失败
            {
                return -1;
            } else {
                out = m_pOutBuf;
                out_len = m_nOutBufLen;
                return 2;
            }
        }
    }

    // 超过缓冲区长度
    if (m_nInBufUsedLen + in_len > m_nMaxBufLen) {
        return -2;
    }

    return 0;
}

// 将缓冲区中已经解开的数据输出

inline int CLzoMem::DeCompressFlush(unsigned char* &outBuf, int &outlen) {
    outBuf = 0;
    outlen = 0;
    if (m_nInBufUsedLen > sizeof (CompressBlock)) {
        int nTotalLen = m_lzo.getPkgLen(m_pInBuf, m_nInBufUsedLen);
        if (nTotalLen == -1) {
            nTotalLen = m_nInBufUsedLen;
        }

        if (m_nInBufUsedLen >= nTotalLen) // 可以解开
        {
            if (DeCompressBuffer(nTotalLen) != 0) // 解压失败
            {
                return -1;
            } else {
                outBuf = m_pOutBuf;
                outlen = m_nOutBufLen;
                return 2;
            }
        }
    }

    return 0;
}

/**
 *  LZOFile 文件类，不直接使用
 */
class LZOFile {
public:

    LZOFile() : m_fp(NULL), m_errno(0), m_closed(true), m_pInBuf(NULL), m_pOutBuf(NULL) {
    }

    virtual ~LZOFile() {
    }

protected:
    unsigned int m_nTotalSize; // 被压缩数据的总大小
    unsigned int m_nBlockSize; // 每块大小,默认64k
    unsigned int m_currentBlock; // 当前块
    unsigned int m_BlockOffsetBegin; // 块偏移记录的开始地址
    unsigned int m_blockoffset[65536][2]; // 每块偏移，默认最大文件4G大小，64k，则需要4*1024*1024/64= 
    unsigned char m_byLevel; // 级别
    FILE* m_fp; // 文件句柄
    int m_errno; // 错误

    unsigned char *m_pInBuf; // 用来放置用户待压缩的数据，在解压时放置从压缩文件中读取的数据块
    int m_nInBufUsedLen; // 已经放入多少数据

    unsigned char *m_pOutBuf; // 在压缩时用来放置压缩好的数据，在解压时放置解开后的数据
    int m_nOutBufLen; //OutBuf中数据大小

    CLzoWrapper m_lzo; // 压缩类

    bool m_closed; // 是否关闭

    int write32(unsigned int v) {
        /*
        unsigned char b[4];
        
        b[3] = (unsigned char) ((v >>  0) & 0xff);
        b[2] = (unsigned char) ((v >>  8) & 0xff);
        b[1] = (unsigned char) ((v >> 16) & 0xff);
        b[0] = (unsigned char) ((v >> 24) & 0xff);
         */
        UINT dst = htonl_ex(v);
        return fwrite(&dst, 4, 1, m_fp);
    }

    unsigned int read32() {
        //unsigned char b[4];
        unsigned int v = 0;

        fread(&v, 4, 1, m_fp);
        v = ntohl_ex(v);
        /*
        v  = (unsigned int) b[3] <<  0;
        v |= (unsigned int) b[2] <<  8;
        v |= (unsigned int) b[1] << 16;
        v |= (unsigned int) b[0] << 24;
         */
        return v;
    }

    unsigned int write32FromMem(unsigned int v) {
        unsigned int a = v;
        /*
        unsigned char *b = (unsigned char*)&a;
        
        b[3] = (unsigned char) ((v >>  0) & 0xff);
        b[2] = (unsigned char) ((v >>  8) & 0xff);
        b[1] = (unsigned char) ((v >> 16) & 0xff);
        b[0] = (unsigned char) ((v >> 24) & 0xff);
         */
        a = htonl_ex(a);
        return a;
    }

    unsigned int read32FromMem(unsigned int memvalue) {
        /*
        unsigned char *b = (unsigned char *)&memvalue;
        unsigned int v = 0;
        v  = ((unsigned int) b[3]) <<  0;
        v |= ((unsigned int) b[2]) <<  8;
        v |= ((unsigned int) b[1]) << 16;
        v |= ((unsigned int) b[0]) << 24;
         */
        unsigned int v = ntohl_ex(memvalue);
        return v;
    }
};

/**
 *  LZOFileWriter 压缩文件类
 */
class LZOFileWriter : private LZOFile {
public:

    /** 构造函数
     */
    LZOFileWriter() {
        m_nTotalSize = 0;
    }

    /** 析构函数
     */
    ~LZOFileWriter() {
        Close();
    }

    /**
     * \brief 初始化函数，传入和每块大小
     * @param fp 打开的写文件句柄FILE*（使用wb打开）
     * @param blocksize 压缩块大小(默认64k)
     * @param byLevel 压缩级别(一般不使用)
     * @return 
     *         -# -1 传入文件句柄为NULL，
     *         -# -2 写入出错
     *         -# -4 压缩对象初始化错
     *         -# -5 分配内存错误
     *         -# =0 成功
     */
    int Init(FILE* fp, unsigned int blocksize = BLOCKSIZE, unsigned char byLevel = COMPRESS_LEVEL_FAST) {
        if (fp == NULL) // 文件打开出错
        {
            m_errno = errno;
            return -1;
        }
        
        m_fp = fp;
        m_nBlockSize = blocksize;
        m_byLevel = byLevel;
        m_currentBlock = 0;
        m_nInBufUsedLen = 0;
        m_nTotalSize = 0;
        m_closed = false;

        // 不压缩
        if (byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return 0;
        }

        // 写入魔术字
        if (fwrite(magiclzop, sizeof (magiclzop), 1, m_fp) != 1) {
            m_errno = errno;
            return -2;
        }

        // 写入块大小
        write32(m_nBlockSize);

        // 写入块个数
        write32(0);

        m_BlockOffsetBegin = ftell(m_fp);

        // 写入偏移
        memset(m_blockoffset, 0, sizeof (m_blockoffset));
        if (fwrite(m_blockoffset, sizeof (m_blockoffset), 1, m_fp) != 1) {
            m_errno = errno;
            return -2;
        }

        if (m_lzo.Init(byLevel) != 0) {
            return -4;
        }


        m_pInBuf = (unsigned char*) malloc(blocksize);
        if (m_pInBuf == NULL) {
            return -5;
        }

        m_nOutBufLen = m_lzo.getCompressLen(blocksize);
        m_pOutBuf = (unsigned char*) malloc(m_nOutBufLen);
        if (m_pOutBuf == NULL) {
            return -5;
        }

        return 0;
    }

    /**
     * \brief 写函数（一般不直接使用），用户调用此函数往压缩文件中写入内容，只有满一个块大小才会写入，否则放入缓冲区
     * @param buf 待压内容首指针
     * @param size 待压内容大小
     * @param count 待压内容个数
     * @return 
     *         - -1 错误，
     *         - =0 成功
     */
    int Write(const void* buf, size_t size, size_t count) {
        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return fwrite(buf, size, count, m_fp);
        }

        if (m_currentBlock >= 65535) // 超过块大小了
        {
            return -1;
        }

        size_t isize = size * count;
        if (isize > m_nBlockSize) {
            size_t itowrite = 0;
            size_t iwrite = 0;
            if (m_nInBufUsedLen > 0) {
                itowrite = m_nBlockSize - m_nInBufUsedLen;
                memcpy(m_pInBuf + m_nInBufUsedLen, (const BYTE*) buf + iwrite, itowrite);
                m_nInBufUsedLen += itowrite;
                if (CompressWrite() != 0) {
                    return -1;
                }
                iwrite += itowrite;
            }

            while (iwrite < isize) {
                itowrite = ((isize - iwrite) > m_nBlockSize) ? m_nBlockSize : (isize - iwrite);
                memcpy(m_pInBuf + m_nInBufUsedLen, (const BYTE*) buf + iwrite, itowrite);
                m_nInBufUsedLen += itowrite;
                if (CompressWrite() != 0) {
                    return -1;
                }
                iwrite += itowrite;
            }
        } else {
            // 超过块大小，压缩此块并将写入文件
            if ((m_nInBufUsedLen + isize) > m_nBlockSize) {
                if (CompressWrite() != 0) {
                    return -1;
                }
            }

            // 如果没有满则放入缓冲区
            memcpy(m_pInBuf + m_nInBufUsedLen, buf, isize);
            m_nInBufUsedLen += isize;
        }
        return count;
    }

    /**
     * \brief 关闭文件
     * @return 
     *         - -1 错误，
     *         - =0 成功
     */
    int Close() {
        if (m_closed) {
            return 0;
        }
        m_closed = true;

        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return fclose(m_fp);
        }

        if (m_nInBufUsedLen > 0 && CompressWrite() != 0) {
            return -1;
        }

        // 计算最后的偏移
        unsigned int offset = ftell(m_fp);

        m_blockoffset[m_currentBlock][0] = write32FromMem(offset);
        m_blockoffset[m_currentBlock][1] = write32FromMem(m_nTotalSize);

        // 写入块个数
        fseek(m_fp, m_BlockOffsetBegin - 4, SEEK_SET);
        write32(m_currentBlock);

        // 写入最后的偏移地址
        if (fwrite(m_blockoffset, sizeof (m_blockoffset), 1, m_fp) != 1) {
            m_errno = errno;
            return -3;
        }

        free(m_pInBuf);
        m_pInBuf = 0;
        free(m_pOutBuf);
        m_pOutBuf = 0;

        fclose(m_fp);
        m_fp = NULL;
        return 0;
    }

private:
    // 压缩并写入

    int CompressWrite() {
        int nOutLen = m_nOutBufLen;
        // 压缩
        if (m_lzo.Compress(m_pInBuf, m_nInBufUsedLen, m_pOutBuf, nOutLen) != 0) {
            return -1;
        }

        // 记录本块偏移量
        unsigned int offset = ftell(m_fp);
        m_blockoffset[m_currentBlock][0] = write32FromMem(offset);
        m_blockoffset[m_currentBlock][1] = write32FromMem(m_nTotalSize);
        m_nTotalSize += m_nInBufUsedLen;
        m_currentBlock += 1;

        if (fwrite(m_pOutBuf, nOutLen, 1, m_fp) != 1) {
            m_errno = errno;
            return -1;
        }

        m_nInBufUsedLen = 0;
        return 0;
    }

    int Tell() {
        if (m_byLevel == COMPRESS_LEVEL_NO_COMPRESS) {
            return ftell(m_fp);
        }

        return m_nInBufUsedLen + m_nTotalSize;
    }

public:
    friend int fwrite(const void* buf, size_t size, size_t count, LZOFileWriter& fp);
    friend int fclose(LZOFileWriter &fp);
    friend int ftell(LZOFileWriter &fp);
};

/**
 * \brief 写函数, 和C语言fwrite对应
 * @param buf 待压内容首指针
 * @param size 待压内容大小
 * @param count 待压内容个数
 * @param fp   LZOFileWriter对象
 * @return 
 *         - -1 错误，
 *         - >0 实际写入的count
 */
inline int fwrite(const void* buf, size_t size, size_t count, LZOFileWriter& fp) {
    return fp.Write(buf, size, count);
}

/**
 * \brief 文件关闭函数, 和C语言fclose对应
 * @param fp   LZOFileWriter对象
 * @return 
 *         - -1 错误，
 *         - =0 成功
 */
inline int fclose(LZOFileWriter &fp) {
    return fp.Close();
}

/**
 * \brief 返回当前文件位置, 和C语言ftell对应，此位置不是压缩文件实际的位置，而是用户已经写入了多少未压缩的数据
 * @param fp   LZOFileWriter对象
 * @return 
 *         - -1 错误，
 *         - >=0 当前位置
 */
inline int ftell(LZOFileWriter &fp) {
    return fp.Tell();
}


typedef map<unsigned int, unsigned int> MAPOFFSET; //!< 偏移对应关系，即未压缩数据偏移对应压缩数据的偏移

/**
 *  压缩文件读取类，如果文件是未压缩文件，也可以使用本类来操作，支持压缩文件的随机读取
 */
class LZOFileReader : private LZOFile {
    bool m_isCompressed; //!< 根据magic判断是否是压缩文件
    unsigned int m_MaxBlocklen; //!< 每块最长字节数
    unsigned int m_virtualOffset; //!< 对应解压后的文件偏移量，用户seek和tell都使用这个偏移量，用于随机读取
    MAPOFFSET m_offsetmap; //!< 解压后对应文件偏移量对应实际偏移量
    MAPOFFSET m_offset2BlockNum; //!< 偏移量对应的块数
    unsigned int m_blockNum; //!< 块个数

    unsigned int m_nOutBufOffset; //!<当前已解开块读取的偏移
    bool m_isEnd; //!< 是否为文件尾

public:

    /**
     * \brief 构造函数
     */
    LZOFileReader() {
        m_nBlockSize = 0;
        m_byLevel = 1;
        m_currentBlock = 0;
        m_nInBufUsedLen = 0;
        m_nOutBufLen = 0;
        m_virtualOffset = 0;
        m_nOutBufOffset = 0;
        m_closed = true;
    }

    /**
     * \brief 析构函数，会调用Close
     */
    ~LZOFileReader() {
        Close();
    }

    /**
     * \brief 初始化函数，传入和每块大小
     * @param fp 打开的写文件句柄FILE*（使用rb打开）
     * @return 
     *         -# -1 传入文件句柄为NULL，
     *         -# -2 文件读取出错
     *         -# -4 压缩对象初始化错
     *         -# -5 分配内存错误
     *         -# =0 成功
     */
    int Init(FILE* fp) {
        if (fp == NULL) // 文件打开出错
        {
            m_errno = errno;
            return -1;
        }

        m_nBlockSize = 0;
        m_byLevel = 1;
        m_currentBlock = 0;
        m_nInBufUsedLen = 0;
        m_nOutBufLen = 0;
        m_virtualOffset = 0;
        m_nOutBufOffset = 0;

        m_fp = fp;
        m_closed = false;
        m_isEnd = false;

        unsigned char sMagic[sizeof (magiclzop)];
        // 魔术字
        if (fread(sMagic, sizeof (sMagic), 1, m_fp) != 1) {
            m_errno = errno;
            return -2;
        }

        if (memcmp(sMagic, magiclzop, sizeof (magiclzop)) == 0) {
            m_isCompressed = true;
        } else // 非压缩
        {
            m_isCompressed = false;

            fseek(m_fp, 0, SEEK_SET);
            return 0;
        }

        // 读出块大小
        m_nBlockSize = read32();

        // 读出块个数
        m_blockNum = read32();

        // 读出偏移量
        m_BlockOffsetBegin = ftell(m_fp);
        memset(m_blockoffset, 0, sizeof (m_blockoffset));
        if (fread(m_blockoffset, sizeof (m_blockoffset), 1, m_fp) != 1) {
            m_errno = errno;
            return -2;
        }

        if (m_lzo.Init(1) != 0) {
            return -4;
        }

        GenOffsetMap();

        m_MaxBlocklen = m_lzo.getCompressLen(m_nBlockSize);
        m_pInBuf = (unsigned char*) malloc(m_MaxBlocklen);
        if (m_pInBuf == NULL) {
            return -5;
        }

        m_pOutBuf = (unsigned char*) malloc(m_nBlockSize);
        if (m_pOutBuf == NULL) {
            return -5;
        }

        return 0;
    }

    /**
     * \brief 读函数（一般不直接使用）
     * @param buf 读取结构数组首指针
     * @param size 读取结构大小
     * @param count 读取结构个数
     * @return 
     *         - -1 错误，
     *         - =0 成功
     */
    int Read(void* buf, size_t size, size_t count) {
        if (!m_isCompressed) {
            return fread(buf, size, count, m_fp);
        }

        if (buf == 0) {
            return -1;
        }

        int nNeedReadSize = size * count;
        char* pos = (char*) buf;
        while (nNeedReadSize > 0) {
            // 查找buffer中数据是否足够，如果足够，则直接返回 
            if (m_nOutBufLen - m_nOutBufOffset >= nNeedReadSize) {
                memcpy(pos, &m_pOutBuf[m_nOutBufOffset], nNeedReadSize);
                m_nOutBufOffset += nNeedReadSize;
                return count;
            }

            // 如果不够，则先取当前块部分，然后读取下一个块并写入缓冲区
            if ((m_nOutBufLen - m_nOutBufOffset) > 0) {
                memcpy(pos, &m_pOutBuf[m_nOutBufOffset], (m_nOutBufLen - m_nOutBufOffset));
                nNeedReadSize -= (m_nOutBufLen - m_nOutBufOffset);
                pos += (m_nOutBufLen - m_nOutBufOffset);
            }

            if (isEof()) // 读完了，返回实际读到的个数
            {
                return (size * count - nNeedReadSize) / size;
            }

            // 读取下一块
            if (DeCompressRead() != 0) // 读完了，返回错误
            {
                return -1;
            }
        }

        return 0;
    }

    /**
     * \brief 关闭对象
     * @return 
     *         - -1 错误，
     *         - =0 成功
     */
    int Close() {
        if (m_closed) {
            return 0;
        }
        m_closed = true;

        if (!m_isCompressed) {
            return fclose(m_fp);
        }

        free(m_pInBuf);
        m_pInBuf = NULL;
        free(m_pOutBuf);
        m_pOutBuf = NULL;
        fclose(m_fp);
        m_fp = NULL;
        return 0;
    }

private:

    int DeCompressRead() {
        m_nOutBufLen = 0;
        m_nOutBufOffset = 0;

        if (m_currentBlock > 65535) {
            m_isEnd = true;
            return 0;
        }

        // 获取本块长度
        unsigned int begin = read32FromMem(m_blockoffset[m_currentBlock][0]);
        unsigned int end = read32FromMem(m_blockoffset[m_currentBlock + 1][0]);
        int len = end - begin;
        if (len <= 0 || len > m_MaxBlocklen) // 读到文件尾了
        {
            m_isEnd = true;
            return 0;
        }

        if (fread(m_pInBuf, len, 1, m_fp) != 1) {
            m_isEnd = true;
            return 0;
        }

        // 解压
        int nOutLen = m_nBlockSize;
        if (m_lzo.DeCompress(m_pInBuf, len, m_pOutBuf, nOutLen) != 0) {
            return -1;
        }

        m_currentBlock++;
        m_nOutBufLen = nOutLen;
        m_nOutBufOffset = 0;

        return 0;
    }

    // 产生实际文件和压缩文件块偏移对应关系

    void GenOffsetMap() {
        unsigned int realOffset = 0;
        unsigned int virtualOffset = 0;
        for (int i = 0; i < m_blockNum + 1; i++) {
            realOffset = read32FromMem(m_blockoffset[i][0]);
            virtualOffset = read32FromMem(m_blockoffset[i][1]);
            m_offsetmap.insert(MAPOFFSET::value_type(virtualOffset, realOffset));
            m_offset2BlockNum.insert(MAPOFFSET::value_type(virtualOffset, i));
        }

        // 实际文件总大小
        m_nTotalSize = virtualOffset;
    }

    int Tell() {
        if (!m_isCompressed) {
            return ftell(m_fp);
        }

        if (m_currentBlock > 0) {
            return read32FromMem(m_blockoffset[m_currentBlock - 1][1]) + m_nOutBufOffset;
        } else {
            return 0;
        }
    }

    int Seek(int offset, int pos) {
        if (!m_isCompressed) {
            return fseek(m_fp, offset, pos);
        }

        long beginoffset = offset;
        if (pos == SEEK_END) {
            beginoffset = Size() + offset;
        } else if (pos == SEEK_CUR) {
            beginoffset = Tell() + offset;
        } else {
            beginoffset = offset;
        }

        if (beginoffset > Size()) {
            m_isEnd = true;
            return -1;
        }

        // 查找是哪一块
        MAPOFFSET::iterator ItFileOffset = m_offsetmap.upper_bound(beginoffset); // 压缩文件偏移量
        MAPOFFSET::iterator ItBlockNum = m_offset2BlockNum.upper_bound(beginoffset); // 块数
        --ItFileOffset;
        --ItBlockNum;

        int seekedBlockNum = ItBlockNum->second;
        if (seekedBlockNum > 65536) {
            return -1;
        }

        // 如果查询的块数和当前块相同,则只需修改偏移量
        if (seekedBlockNum == m_currentBlock - 1) {
            m_nOutBufOffset = beginoffset - ItFileOffset->first;
            m_isEnd = false;
            return 0;
        }

        // 如果不同，则需要解压对应块，并修改偏移量
        m_currentBlock = seekedBlockNum;
        int nRet = fseek(m_fp, ItFileOffset->second, SEEK_SET);
        if (nRet != 0) {
            return nRet;
        }

        if (DeCompressRead() != 0) // 解压出错
        {
            return -1;
        }

        m_nOutBufOffset = beginoffset - ItFileOffset->first;

        return 0;
    }

    bool isEof() {
        if (!m_isCompressed) {
            return feof(m_fp) != 0;
        }

        return m_isEnd;
    }



public:

    /**
     * \brief 获取文件大小，指解压后的总大小
     * @return 解压后文件总大小
     */
    int Size() {
        if (!m_isCompressed) {
            size_t pos = ftell(m_fp);

            fseek(m_fp, 0, SEEK_END);
            m_nTotalSize = ftell(m_fp);
            fseek(m_fp, pos, SEEK_SET);

        }

        return m_nTotalSize;
    }

    friend int fread(const void* buf, size_t size, size_t count, LZOFileReader& fp);
    friend int ftell(LZOFileReader& fp);
    friend int fseek(LZOFileReader& fp, long offset, int pos);
    friend int fclose(LZOFileReader &fp);
    friend int fEof(LZOFileReader &fp); // feof是个宏，无法替换
};

/**
 * \brief 读函数, 和C语言fread对应
 * @param buf 读取结构数组首指针
 * @param size 读取结构大小
 * @param count 读取结构个数
 * @param fp   LZOFileWriter对象
 * @return 
 *         - -1 错误，
 *         - >0 实际读取结构体的个数
 */
inline int fread(void* buf, size_t size, size_t count, LZOFileReader& fp) {
    return fp.Read(buf, size, count);
}

/**
 * \brief 获取位置函数, 和C语言ftell对应
 * @param fp   LZOFileWriter对象
 * @return 当前对应未压缩文件的位置         
 */
inline int ftell(LZOFileReader& fp) {
    return fp.Tell();
}

/**
 * \brief 设置位置函数, 和C语言fseek对应
 * @param offset 位置偏移
 * @param pos 从哪里开始偏移 SEEK_SET, SEEK_END, SEEK_CUR
 * @param fp   LZOFileWriter对象
 * @return 设置当前对应未压缩文件的位置         
 */
inline int fseek(LZOFileReader& fp, long offset, int pos) {
    return fp.Seek(offset, pos);
}

/**
 * \brief 关闭文件, 和C语言fclose对应
 * @param fp   LZOFileWriter对象
 * @return 设置当前对应未压缩文件的位置         
 */
inline int fclose(LZOFileReader &fp) {
    return fp.Close();
}

/**
 * \brief 判断是否是文件尾, 和C语言feof对应
 * @param fp   LZOFileWriter对象
 * @return 如果是文件尾，返回1，否则返回0         
 */
inline int fEof(LZOFileReader &fp) {
    if (fp.isEof()) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * \brief 判断是否是文件尾, 主要是为了使用FILE*和使用LZOFileReader一样
 * @param fp   FILE*
 * @return 如果是文件尾，返回1，否则返回0         
 */
inline int fEof(FILE* fp) {
    return feof(fp);
}

#endif // LZOWRAPPER_H_
