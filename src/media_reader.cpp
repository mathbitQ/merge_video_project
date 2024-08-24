#include "media_reader.h"
#include <codecvt>
#include <iostream>
#include <string>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>

int64_t getMediaDuration(const wxString *filename)
{
    AVFormatContext *formatContext = avformat_alloc_context();
    if(!formatContext)
    {
        return -1;
    }
    std::string utf8filename = filename->ToUTF8().data();
    if(avformat_open_input(&formatContext, utf8filename.c_str(), nullptr, nullptr))
    {
        std::cerr<<"Could not open file: "<<filename<<"\n";
        avformat_free_context(formatContext);
        return -1;
    }

    if(avformat_find_stream_info(formatContext, nullptr) < 0)
    {
        std::cerr << "Could not retrieve stream info from file: " << filename << "\n";
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        return -1;
    }

    int64_t duration = (formatContext->duration / AV_TIME_BASE);

    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);

    return duration;
}

wxString getMediaTimerFormat(const int64_t timer)
{
    if(timer <= 0)
        return "00:00:00";

    wxString format = "";
    int64_t temp_second = timer;
    int64_t hour;
    int64_t minute;
    int64_t second;

    hour = temp_second / 3600;
    temp_second %= 3600;
    minute = temp_second / 60;
    temp_second %= 60;
    second = temp_second;

    if(hour >= 10)
    {
        format.Append(std::to_string(hour));
    }
    else
    {
        format.Append("0");
        format.Append(std::to_string(hour));
    }
    format.Append(":");
    if(minute >= 10)
    {
        format.Append(std::to_string(minute));
    }
    else
    {
        format.Append("0");
        format.Append(std::to_string(minute));
    }
    format.Append(":");
    if(second >= 10)
    {
        format.Append(std::to_string(second));
    }
    else
    {
        format.Append("0");
        format.Append(std::to_string(second));
    }
    return format;
}
