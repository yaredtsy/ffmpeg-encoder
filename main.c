#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avstring.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE* pFIle;
    char sFile[32];
    int y;

    sprintf(sFile,"frame%d.ppm",iFrame);
    pFIle=fopen(sFile,"wb");
    if(pFIle==NULL)
        return;
    fprintf(pFIle,"P6\n%d %d\n255\n", width, height);
    printf("\n");
    int ww=0;
    for(y=0;y<height;y++){


        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFIle);
    }
    fclose(pFIle);

}

void encode(FILE* f,const AVCodec *codec,AVCodecContext *c,AVFrame *frame){

    int i, ret, x, y, got_output;
    AVPacket pkt;

    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;

    fflush(stdout);
    /* encode the image */

    if(!av_codec_is_encoder(codec))
        printf("\nis not coder ");
    ret = codec->encode2(c, &pkt, frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }
    else
        fprintf(stderr,"\nWORks");
    if (got_output) {
        printf("Write frame %3d (size=%5d)\n", i, pkt.size);
        fwrite(pkt.data, 1, pkt.size, f);
        av_free_packet(&pkt);
    }
    /* add sequence end code to have a real mpeg file */

}

int main(int argc, char *argv[])
{
   av_register_all();
   AVFormatContext* pForm=NULL;
   AVCodecContext * pCodorg=NULL;
   AVCodecContext* pCod=NULL;
   AVCodec* code=NULL;
   AVFrame* pFrame=NULL;
   AVFrame* pFrameRGB=NULL;
   AVFrame* enframe=NULL;
   uint8_t* buffer=NULL;
   int bytes;
   uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    AVDictionary    *optionsDict = NULL;
   char* d="/home/yared/Videos/cc.mp4";

/////--encoder variable---\\\\\\\\\\\

   FILE *f;
   const char *filename, *codec_name;
   const AVCodec *codec;
   AVCodecContext *c= NULL;

   filename="/media/yared/file/programing/---Kygo/ -/ Stole/ The/ Show/ feat./ Parson/ James/ [Official/ Music/ Video/ -/ YTMAs]/ -/ YouTube.mp4";

 /////////////////////////////////////

   if(avformat_open_input(&pForm,d,NULL,NULL)!=0)
       return -1;
   if(avformat_find_stream_info(pForm,NULL)<0)
       return -1;

   av_dump_format(pForm,0,d,0);

   int i=0;

   int videostream=-1;

   for(i=0;i<pForm->nb_streams;i++){
       if(pForm->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
           videostream=i;
            break;
       }

   }

   if(videostream==-1)
       return -1;
   pCod=pForm->streams[videostream]->codec;

   code=avcodec_find_decoder(AV_CODEC_ID_4XM);

   printf("\n codec %s",code->name);
    if(code==NULL){
        fprintf(stderr,"unsported codec\n");
        return -1;
      }

    if(avcodec_open2(pCod,code,&optionsDict)<0)
        return -1;

    pFrame=av_frame_alloc();
    pFrameRGB=av_frame_alloc();
    if(pFrameRGB==NULL)
        return -1;
    bytes=avpicture_get_size(AV_PIX_FMT_RGB24,pCod->width,pCod->height);
    buffer=(uint8_t*)av_malloc(bytes*sizeof(uint8_t));
    printf("\nbuffer =%i",buffer);
    avpicture_fill((AVPicture*)pFrameRGB,buffer,AV_PIX_FMT_RGB24,pCod->width,pCod->height);

    struct SwsContext *sws_ctk=NULL;
    int framefinished;
    AVPacket packet;

    sws_ctk = sws_getContext(pCod->width,
                             pCod->height,
                             pCod->pix_fmt,
                             pCod->width,
                             pCod->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
                             );
//////////-------------- encoder body -----------\\\\\\\\\//


    codec = avcodec_find_decoder(AV_CODEC_ID_4XM);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

//    c = avcodec_alloc_context3(codec);
//    if (!c) {
//        fprintf(stderr, "Could not allocate video codec context\n");
//        exit(1);
//    }
//    else
//        printf("\nalocted");
//    /* put sample parameters */
//    c->bit_rate = pCod->bit_rate;
//    /* resolution must be a multiple of two */
//    c->width = pCod->width;
//    c->height = pCod->height;
//    /* frames per second */
//    c->time_base = pCod->time_base;
//    c->framerate = pCod->framerate;

//    /* emit one intra frame every ten frames
//     * check frame pict_type before passing frame
//     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
//     * then gop_size is ignored and the output of encoder
//     * will always be I frame irrespective to gop_size
//     */
//    c->gop_size = pCod->gop_size;
//    c->max_b_frames = pCod->max_b_frames;
//    c->pix_fmt = AV_PIX_FMT_YUV420P;
//    if (avcodec_open2(c, codec, &optionsDict) < 0) {
//            fprintf(stderr, "Could not open codec\n");
//            exit(1);
//     }

//    f = fopen(filename, "wb");
//    if (!f) {
//        fprintf(stderr, "Could not open %s\n", filename);
//        exit(1);
//    }

///////////////////////////////////////////////////////////////////

    while(av_read_frame(pForm,&packet)>=0){

        if(packet.stream_index==videostream){

            avcodec_decode_video2(pCod,pFrame,&framefinished,&packet);


            if(framefinished){

              // encode(f,codec,c,pFrame);
                sws_scale(sws_ctk, (uint8_t const * const *)pFrame->data,
                         pFrame->linesize, 0, pCod->height,
                         pFrameRGB->data, pFrameRGB->linesize);
                if(70<i && i<80){
                   SaveFrame(pFrameRGB,pFrame->width,pFrame->height,i);
                }
                i++;

            }
        }
        av_free_packet(&packet);
    }
    printf("\nframe %d",i);
    av_free(buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCod);

    // Close the video file
    avformat_close_input(&pForm);

//    fwrite(endcode, 1, sizeof(endcode), f);
//    fclose(f);

    avcodec_close(c);
    av_free(c);


}
