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
 
