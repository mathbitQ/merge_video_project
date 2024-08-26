# Merge Video Project Source Code

This merge video project aims to merge multiple videos with ".mp4" file using H264 codec.
## Libraries:
wxWidgets (C++): For developing friendly GUI
ffmpeg (C): For processing media file and read metadata from media file
## Tools (optional - depending on your usage):
CodeBlock - Tutorial for linking with [wxWidgets](https://www.youtube.com/watch?v=KHkb1XrtseY)
MinGW (Windows version and MSYS2 version)
MSYS2
## Operating System: 
I use Windows as the main environment. I haven't tested Linux yet.
## Source code tree simple view:
Note that I do not upload the ffmpeg and wxWidgets source code, so you have to find them)
	merge_video_project/
			|bin/
   			    |ffmpeg.exe (You can install FFmpeg executable file [here](https://www.ffmpeg.org/download.html))
			|lib/
   			    |ffmpeg/ (You can install FFmpeg source code [here](https://github.com/FFmpeg/FFmpeg))
			|resource/ (Images file)
			|src/ (Source file for merge video project)
	wxWidgets/ (You can install wxWidgets for C++ [here](https://www.wxwidgets.org/downloads/))
## Compiling Guide:
1. Compiles wxWidgets (You can use [MinGW](https://winlibs.com/) compiler with MSCVRT environment
or use [Visual Studio](https://visualstudio.microsoft.com/downloads/))
 
2. Compiles FFmpeg (If you use MinGW, you should install [MSYS2](https://www.msys2.org/) to configure
FFmpeg source code and install them. Note that you should use MINGW32 or MINGW64 of MSYS2 to match
the MSCVRT environment with wxWidgets. Using UCRT might not compile)
 
3. (Optional if linking failed while compiling) Installs [zlib](https://www.zlib.net/) source code, 
[libiconv](https://www.gnu.org/software/libiconv/) source code and 
uses MSYS2 to configure and compile. Then copies libraries (usually ".dll", ".o", ".a" files) to your compiler.
(These libraries are not available in Windows and these are required from FFmpeg library)
 
4. Compiles all sources code from **src**. Notes that you should have linked all the required libraries and includes files from
both wxWidgets and ffmpeg libraries.
	
## Linking parameter
	`-lavformat -lavcodec -lswresample -lswscale -lavutil -lavdevice -lavfilter -lz -lWs2_32 -liconv -lSecur32 -lole32 -lbcrypt -lstrmiids`

	
