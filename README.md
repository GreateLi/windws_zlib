# windws_zlib
windws zlib
zlib (gzip) (一) winodws 编译及遇到的问题
一、下载源代码

https://www.zlib.net/fossils/

前往zlib官网下载zlib1.2.11源代码
二、编译汇编代码

使用VS工具运行下面两个批处理 ，切记用不同的本地工具：

zlib\contrib\masmx86\bld_ml32.bat
zlib\contrib\masmx64\bld_ml64.bat
三、配置静态库项目

使用VS打开解决方案文件zlib\contrib\vstudio\vc14\zlibvc.sln
设置zlibstat项目编译方式(MT/MD)，以及使用的WindowsSDK等信息

四、遇到的问题:

1.

1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflate
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflateEnd
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflate
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflateEnd
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflateInit2_
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflateInit2_

解决方法：在 exe 或dll工程中添加宏 ZLIB_WINAPI

2. 出现

1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflate@8
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflateEnd@4
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflate@8
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflateEnd@4
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _deflateInit2_@32
1>utils_idcardverify.obj : error LNK2001: 无法解析的外部符号 _inflateInit2_@16

解决方法：在 zlib工程中添加宏 ZLIB_WINAPI
 
3.其它问题一般设置 设置zlibstat项目编译方式(MT/MD) 和EXE /DLL一致，一般可以解决。

zlib (gzip) (二) winodws gzib使用
#include "stdafx.h"
#include "zlib/zlib.h"
#include <stdlib.h>
#include <windows.h>
#pragma comment (lib, "zlibstatic.lib")
static int  gzCompress(const unsigned char *src, int srcLen, unsigned char *dest, int destLen)
{
	z_stream c_stream;
	int err = 0;
	int windowBits = 15;
	int GZIP_ENCODING = 16;

	/* 压缩级别 */
//#define Z_NO_COMPRESSION         0  
//#define Z_BEST_SPEED             1  
//#define Z_BEST_COMPRESSION       9  
//#define Z_DEFAULT_COMPRESSION  (-1)  
	if (src && srcLen > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = (Bytef *)src;
		c_stream.avail_in = srcLen;
		c_stream.next_out = (Bytef *)dest;
		c_stream.avail_out = destLen;
		while (c_stream.avail_in != 0 && c_stream.total_out < destLen)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		return c_stream.total_out;
	}
	return -1;
}

解压：

static int  gzDecompress(const char *src, int srcLen, const char *dst, int dstLen,
    int * outLen) {
    z_stream strm;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;

    strm.avail_in = srcLen;
    strm.avail_out = dstLen;
    strm.next_in = (Bytef *)src;
    strm.next_out = (Bytef *)dst;

    int err = -1, ret = -1;
    err = inflateInit2(&strm, MAX_WBITS + 16);
    if (err == Z_OK) {
        err = inflate(&strm, Z_FINISH);
        if (err == Z_STREAM_END) {
            ret = strm.total_out;
        }
        else {
            inflateEnd(&strm);
            return err;
        }
    }
    else {
        inflateEnd(&strm);
        return err;
    }
    inflateEnd(&strm);
    (*outLen) = ret;
    return err;
}
 
