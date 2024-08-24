#ifndef MEDIA_READER_H_INCLUDED
#define MEDIA_READER_H_INCLUDED

#include <wx/wx.h>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

// Allows the avcodec and avformat from ffmpeg library to read metadata from media file
int64_t getMediaDuration(const wxString *filename);

// Allows to convert timer (as seconds) to format of hh:mm:ss
wxString getMediaTimerFormat(const int64_t timer);

#endif // MEDIA_READER_H_INCLUDED
