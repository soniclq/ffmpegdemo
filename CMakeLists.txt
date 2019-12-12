cmake_minimum_required(VERSION 3.12)
project(Project C)

set(CMAKE_C_STANDARD 99)

include_directories(/Users/momo/Downloads/ffmpeg-2.8.15)
link_directories("/Users/momo/Downloads/ffmpeg-2.8.15/staticlib") #库文件目录
#link_libraries(libavutil.a libavformat.a  libavcodec.a ) #要包含的库名称
link_libraries(z iconv bz2 lzma   avutil avformat avcodec swscale swresample)
#target_link_libraries(
#
#        libavformat.a
#        avcodec
#        avutil
#        swscale)

add_executable(Project main.c)
if (APPLE)
    target_link_libraries(Project "-framework VideoDecodeAcceleration")
    target_link_libraries(Project "-framework Security")
    target_link_libraries(Project "-framework CoreFoundation")
    target_link_libraries(Project "-framework CoreVideo")
    target_link_libraries(Project "-framework CoreMedia")
    target_link_libraries(Project "-framework VideoToolbox")
endif()