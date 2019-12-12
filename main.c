#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

int main() {

    AVFormatContext	*pFormatCtx;
    int				i, videoindex;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    int count  = 0;


//    char filepath[]="http://ali-pull.v.momocdn.com/momo/m_15761178920835d1f48122f186c791.flv?uniqtype=1";
    char filepath[] = "/Users/momo/Documents/pkvideo/local.flv";
    av_register_all();//注册组件
    avformat_network_init();//支持网络流
    pFormatCtx = avformat_alloc_context();//初始化AVFormatContext
    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){//打开文件
        printf("无法打开文件\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)//查找流信息
    {
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++) //获取视频流ID
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    if(videoindex==-1)
    {
        printf("Didn't find a video stream.\n");
        return -1;
    }
    pCodecCtx=pFormatCtx->streams[videoindex]->codec;
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器
    if(pCodec==NULL)
    {
        printf("Codec not found.\n");
        return -1;
    }
//    pCodecCtx->flags2 |= CODEC_FLAG2_FAST;


    AVDictionary *opts = NULL;
    AVDictionary *d = NULL;
//    av_dict_set(&d, "name", "jhuster", 0);

//    av_dict_set(&opts, "refcounted_frames", "1", 0);
    av_dict_set(&opts, "threads", "auto", 0);
//    av_dict_set(&opts, "threads", "1", 0);
//    av_dict_set(&opts, "skip_loop_filter", "0", 0);


    if(avcodec_open2(pCodecCtx, pCodec, &opts)<0)//打开解码器
    {
        printf("Could not open codec.\n");
        return -1;
    }

    printf("avcodec thread count %d\n", pCodecCtx->thread_count);
    AVFrame	*pFrame,*pFrameYUV;
    pFrame=avcodec_alloc_frame();//存储解码后AVFrame
    pFrameYUV=avcodec_alloc_frame();//存储转换后AVFrame（为什么要转换？后文解释）
    uint8_t *out_buffer;
    out_buffer=(uint8_t*)malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));//分配AVFrame所需内存
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);//填充AVFrame
    //------------SDL初始化--------
//    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
//        printf( "Could not initialize SDL - %s\n", SDL_GetError());
//        return -1;
//    }
//    SDL_Surface *screen;
//    screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
//    if(!screen) {
//        printf("SDL: could not set video mode - exiting\n");
//        return -1;
//    }
//    SDL_Overlay *bmp;
//    bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height,SDL_YV12_OVERLAY, screen);
//    SDL_Rect rect;
    //-----------------------------
    int ret, got_picture;
    static struct SwsContext *img_convert_ctx;
    int y_size = pCodecCtx->width * pCodecCtx->height;

    AVPacket *packet=(AVPacket *)malloc(sizeof(AVPacket));//存储解码前数据包AVPacket
    av_new_packet(packet, y_size);
    //输出一下信息-----------------------------
    printf("文件信息-----------------------------------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");
    //------------------------------
    while(av_read_frame(pFormatCtx, packet)>=0)//循环获取压缩数据包AVPacket
    {
        if(packet->stream_index==videoindex)
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//解码。输入为AVPacket，输出为AVFrame
            if(ret < 0)
            {
                printf("解码错误\n");
                return -1;
            }
            printf("avcodec_decode_video2 ret %d, got picture %d , count %d\n", ret, got_picture, ++count);
            if(got_picture)
            {
                //像素格式转换。pFrame转换为pFrameYUV。
//                img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
//                sws_freeContext(img_convert_ctx);
//                //------------SDL显示--------
//                SDL_LockYUVOverlay(bmp);
//                bmp->pixels[0]=pFrameYUV->data[0];
//                bmp->pixels[2]=pFrameYUV->data[1];
//                bmp->pixels[1]=pFrameYUV->data[2];
//                bmp->pitches[0]=pFrameYUV->linesize[0];
//                bmp->pitches[2]=pFrameYUV->linesize[1];
//                bmp->pitches[1]=pFrameYUV->linesize[2];
//                SDL_UnlockYUVOverlay(bmp);
//                rect.x = 0;
//                rect.y = 0;
//                rect.w = pCodecCtx->width;
//                rect.h = pCodecCtx->height;
//                SDL_DisplayYUVOverlay(bmp, &rect);
//                //延时40ms
//                SDL_Delay(40);
                //------------SDL-----------
            }
        }
        av_free_packet(packet);
    }
//    delete[] out_buffer;
    free(out_buffer);
    av_free(pFrameYUV);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;


}