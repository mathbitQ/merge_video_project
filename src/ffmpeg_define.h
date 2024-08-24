#ifndef FFMPEG_DEFINE_H_INCLUDED
#define FFMPEG_DEFINE_H_INCLUDED

/*
    FFMpeg command definition
    Argument Passing:
    -b:v <amount>k|M : dynamic bitrate for the final video
    -maxrate <amount>k|M -bufsize <amount>k|M: constant bitrate for the final video
    -c:v <video_type_codec> Uses the video codec (default H.264 for .mp4 file)
    -r <amount>: Sets the framerate
    -c:a <audio_type_codec> -b:a <amount>k|M: Uses the audio codec with bitrate (default: AAC codec with 192k)
    -crf <amount>: Constant rate factor (default: 23) for quality and file size
    -c copy: Video and audio are copied without encoding (used for final merge)
    -i <filename>: Input file (default using .txt file which contains list of media files)
    -i <filename>: Input file (default using .mp4 to re-encoding for exact the same frame rate and bitrate)
    -f concat -safe 0: Concatenates media file without noticing the file name
*/

/*
    Default file for video list
    file '<path_to_file>'
    file '<path_to_file>'
    ...
*/

#define FFMPEG_INPUT                "ffmpeg -i "
#define CONCAT                      "-f concat "
#define NO_SAFE_CONCAT              "-safe 0 "
#define KILO                        "k "
#define MEGA                        "M "
#define BIT_RATE                    "-b:v "
#define MAX_BIT_RATE                "-maxrate "
#define BUFFER_SIZE                 "-bufsize "
#define VIDEO_CODEC                 "-c:v "
#define FRAME_RATE                  "-r "
#define CONSTANT_RATE_FACTOR        "-crf "
#define VIDEO_COPY                  "-c copy "
#define VIDEO_H264                  "libx264 "
#define CONST_RATE_FACTOR_DEFAULT   23

#endif // FFMPEG_DEFINE_H_INCLUDED
