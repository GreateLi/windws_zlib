// gzip_demo.cpp : 定义控制台应用程序的入口点。
//

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

// gzDecompress: do the decompressing
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
bool   writeFileDex(const char * path, const unsigned char * data, long size)
{
	FILE *fp;
	if ((fp = fopen(path, "wb")) == NULL)
	{
		printf("file cannot open %s", path);
		return false;
	}
	else
	{
		fwrite(data, 1, size, fp);
	}
	if (fclose(fp) != 0)
	{
		printf("file cannot be closed \n");
	}
	else
	{
		printf("file is now closed \n");
	}

	return true;
}
 
int ReadFileMemory(const char* filename, long* plFileSize, unsigned char** pFilePtr)
{
	FILE* stream;
	unsigned char* ptr;
	int retVal = 1;
	stream = fopen(filename, "rb");
	if (stream == NULL)
		return 0;

	fseek(stream, 0, SEEK_END);

	*plFileSize = ftell(stream);
	fseek(stream, 0, SEEK_SET);
	ptr = (unsigned char*)malloc((*plFileSize) + 1);
	if (ptr == NULL)
		retVal = 0;
	else
	{
		if (fread(ptr, 1, *plFileSize, stream) != (*plFileSize))
			retVal = 0;
	}
	fclose(stream);
	*pFilePtr = ptr;
	return retVal;
}
int main(int argc, char *argv[])
{
	long lFileSize;
	unsigned char* FilePtr;
	if (argc <= 1)
	{
		printf("run TestZlib <File> [BlockSizeCompress] [BlockSizeUncompress] [compres. level]\n");
		return 0;
	}

	if (ReadFileMemory(argv[1], &lFileSize, &FilePtr) == 0)
	{
		printf("error reading %s\n", argv[1]);
		return 1;
	}
 
	unsigned char * compBuf = (unsigned char*)malloc(lFileSize );
	long comlen = gzCompress(FilePtr, lFileSize, compBuf, lFileSize);
	writeFileDex("D:\\abc.dll", compBuf, comlen);
    return 0;
}

