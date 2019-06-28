// libjpeg_demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
extern "C" {
#include "jpeg/jpeglib.h"
#include "jpeg/cdjpeg.h"
}
#pragma comment (lib, "jpeg.lib")
bool  RGBToJPGCompress(const char * filename, unsigned char* image_buffer,
	int quality, int image_height, int image_width, unsigned long* bufferSize)
{

	if (filename == NULL || image_buffer == NULL) return false;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;        /* target file */
	JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
	int row_stride;        /* physical row width in image buffer */
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	  if ((outfile = fopen(filename, "wb")) == NULL) {
		  fprintf(stderr, "can't open %s\n", filename);
		  return false;
	  } 
	//  jpeg_stdio_dest(&cinfo, outfile);

	// ���ͼ�����ݻ�����
	// unsigned char* outBuffer = nullptr;
	// ���ͼ�����ݻ���������(ѹ�����ͼ���С)
	//unsigned long bufferSize = 1024*1024*5;
	  unsigned char * jpgMemery = nullptr;
	jpeg_mem_dest(&cinfo, &jpgMemery, (size_t *)bufferSize); // �����ڴ����������
	cinfo.image_width = image_width;     /* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;        /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;     /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	// compress_instance.start_output(cinfo);
	row_stride = image_width * 3;    /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
		//  printf("cinfo.next_scanline * row_stride:%d,row:%u\n", cinfo.next_scanline  , row_pointer[0]);
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	 fwrite(jpgMemery,1, (size_t )*bufferSize, outfile);
	 fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	return true;
}
unsigned char*  do_Stretch_Linear(int w_Dest, int h_Dest, int bit_depth, unsigned char *src, int w_Src, int h_Src)
{
	int sw = w_Src - 1, sh = h_Src - 1, dw = w_Dest - 1, dh = h_Dest - 1;
	int B, N, x, y;
	int nPixelSize = bit_depth / 8;
	unsigned char *pLinePrev, *pLineNext;
	unsigned char *pDest = new unsigned char[w_Dest*h_Dest*bit_depth / 8];
	unsigned char *tmp;
	unsigned char *pA, *pB, *pC, *pD;

	for (int i = 0; i <= dh; ++i)
	{
		tmp = pDest + i*w_Dest*nPixelSize;
		y = i*sh / dh;
		N = dh - i*sh%dh;
		pLinePrev = src + (y++)*w_Src*nPixelSize;
		//pLinePrev =(unsigned char *)aSrc->m_bitBuf+((y++)*aSrc->m_width*nPixelSize);
		pLineNext = (N == dh) ? pLinePrev : src + y*w_Src*nPixelSize;
		//pLineNext = ( N == dh ) ? pLinePrev : (unsigned char *)aSrc->m_bitBuf+(y*aSrc->m_width*nPixelSize);
		for (int j = 0; j <= dw; ++j)
		{
			x = j*sw / dw*nPixelSize;
			B = dw - j*sw%dw;
			pA = pLinePrev + x;
			pB = pA + nPixelSize;
			pC = pLineNext + x;
			pD = pC + nPixelSize;
			if (B == dw)
			{
				pB = pA;
				pD = pC;
			}

			for (int k = 0; k<nPixelSize; ++k)
			{
				*tmp++ = (unsigned char)(int)(
					(B * N * (*pA++ - *pB - *pC + *pD) + dw * N * *pB++
						+ dh * B * *pC++ + (dw * dh - dh * B - dw * N) * *pD++
						+ dw * dh / 2) / (dw * dh));
			}
		}
	}
	return pDest;
}
 
 unsigned char* readFile(const char* file, int *len)
{
	FILE *fp;
	fp = fopen(file, "rb");
	if (fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		unsigned long filesize = ftell(fp);
		*len = filesize;
		if (filesize > 0) {
			 unsigned char* fileBuffer =new unsigned char[filesize + 1];
			// unsigned char *fileBuffer = new unsigned char[filesize + 1];
			rewind(fp);//rewind�������õ�ͬ�� (void)fseek(stream, 0L, SEEK_SET);
			fileBuffer[filesize] = '\0';
			int ret = fread(fileBuffer , sizeof(char), filesize, fp);

			fclose(fp);
			fp = NULL;
			if (ret <= 0) {
				return NULL;
			}
			return fileBuffer;
		}

		fclose(fp);
		fp = NULL;
	}

	return NULL;
}

 int getRatioSize(int bitWidth, int bitHeight, int imageWidth, int imageHeight) {

	 // ���ű�
	 int ratio = 1;
	 // ���ű�,�����ǹ̶��������ţ�ֻ�ø߻��߿�����һ�����ݽ��м��㼴��
	 if (bitWidth > bitHeight && bitWidth > imageWidth) {
		 // ���ͼƬ��ȱȸ߶ȴ�,�Կ��Ϊ��׼
		 ratio = bitWidth / imageWidth;
	 }
	 else if (bitWidth < bitHeight && bitHeight > imageHeight) {
		 // ���ͼƬ�߶ȱȿ�ȴ��Ը߶�Ϊ��׼
		 ratio = bitHeight / imageHeight;
	 }
	 // ��С����Ϊ1
	 if (ratio <= 0)
		 ratio = 1;
	 return ratio;
 }
 
int main()
{
	int len = -1;
	unsigned char * fBuf = readFile("D:/rgb_1.bin", &len);
	unsigned char * sBuf = do_Stretch_Linear(320, 240, 24, fBuf, 640, 480);
	delete[]fBuf;
	unsigned long cBufSize = 0;
	RGBToJPGCompress("D:/rgb_01.jpg", sBuf, 50, 240, 320, &cBufSize);
	delete[]sBuf;
    return 0;
}

