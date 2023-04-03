/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Memc.h"
#include "hardware/hardware_rockchip.h"
#include "hardware/gralloc_rockchip.h"

#include <sync/sync.h>
#include <libsync/sw_sync.h>
#include <ui/GraphicBuffer.h>
#include "Buffer.h"

using namespace android;

// #define MEMC_PROXY_MODE

int get_format(char *input_format, int *output_format, 
                    uint64_t *output_usage, int *output_mask){
    if(strcmp(input_format,"rgb_565")==0){
        *output_format = HAL_PIXEL_FORMAT_RGB_565;
    }else if(strcmp(input_format,"rgb_888")==0){
        *output_format = HAL_PIXEL_FORMAT_RGB_888;
    }else if(strcmp(input_format,"rgba_8888")==0){
        *output_format = HAL_PIXEL_FORMAT_RGBA_8888;
    }else if(strcmp(input_format,"nv12")==0){
        *output_format = HAL_PIXEL_FORMAT_YCrCb_NV12;
    }else if(strcmp(input_format,"nv12_10")==0){
        *output_format = HAL_PIXEL_FORMAT_YCrCb_NV12_10;
        *output_usage = RK_GRALLOC_USAGE_STRIDE_ALIGN_64 | MALI_GRALLOC_USAGE_NO_AFBC;
    }else{
        printf("set a invalid format: %s\n",input_format);
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    //0. get arguments
    char src0_path[500] = "/data/memc_test_data/input0_4k.bin";
    char src1_path[500] = "/data/memc_test_data/input1_4k.bin";
    int src_width = 3840;
    int src_height = 2160;
    int src_format = HAL_PIXEL_FORMAT_YCrCb_NV12;
    uint64_t src_usage = RK_GRALLOC_USAGE_STRIDE_ALIGN_16 | MALI_GRALLOC_USAGE_NO_AFBC;
    int src_buffer_mask = MemcBufferMask::NONE;

#ifdef MEMC_PROXY_MODE
    int dst_width = 3840;
    int dst_height = 2160; 
    int dst_format = HAL_PIXEL_FORMAT_YCrCb_NV12;
    uint64_t dst_usage = RK_GRALLOC_USAGE_STRIDE_ALIGN_16 | MALI_GRALLOC_USAGE_NO_AFBC;
    int dst_buffer_mask = MemcBufferMask::NONE;
    bool memc_comparation_mode = true;
    MemcOsdMode osd_mode = MEMC_OSD_ENABLE_VIDEO;
    if(argc > 11){
        sprintf(src0_path,"%s/memc_test_data/input0_4k.bin",argv[1]);
        sprintf(src1_path,"%s/memc_test_data/input1_4k.bin",argv[1]);
        printf("src0_path=%s\n",src0_path);

        if(strcmp(argv[2],"-i") == 0){
            src_width = atoi(argv[3]);
            src_height = atoi(argv[4]);
            int ret = get_format(argv[5], &src_format, &src_usage, &src_buffer_mask);
            if(ret){
                printf("get args fail!\n");
                return 1;
            }
        }

        if(strcmp(argv[6],"-o") == 0){
            dst_width = atoi(argv[7]);
            dst_height = atoi(argv[8]);
            int ret = get_format(argv[9], &dst_format, &dst_usage, &dst_buffer_mask);
            if(ret){
                printf("get args fail!\n");
                return 1;
            }
        }
        printf("src[w=%d,h=%d,f=%d,mask=%d] dst[w=%d,h=%d,f=%d,mask=%d]\n",
                            src_width,src_height,src_format,src_buffer_mask,
                            dst_width,dst_height,dst_format,dst_buffer_mask);

        if(strcmp(argv[10],"-true") == 0){
            memc_comparation_mode = true;
        }else{
            memc_comparation_mode = false;
        }

        if(strcmp(argv[11],"-enable") == 0){
            osd_mode = MEMC_OSD_ENABLE_VIDEO;
        }else if(strcmp(argv[11],"-online")==0){
            osd_mode = MEMC_OSD_ENABLE_VIDEO_ONELINE;
        }else{
            osd_mode = MEMC_OSD_DISABLE;
        }
    }else{
        printf("please set the right arguments! "
                "Example: memc-test /data -i 3840 2160 nv12 -o 3840 2160 nv12 -true -enable\n");
        return 0;
    }
#else
    if(argc == 6){
        sprintf(src0_path,"%s/memc_test_data/input0_4k.bin",argv[1]);
        sprintf(src1_path,"%s/memc_test_data/input1_4k.bin",argv[1]);
        printf("src0_path=%s\n",src0_path);

        if(strcmp(argv[2],"-i") == 0){
            src_width = atoi(argv[3]);
            src_height = atoi(argv[4]);
            int ret = get_format(argv[5], &src_format, &src_usage, &src_buffer_mask);
            if(ret){
                printf("get args fail!\n");
                return 1;
            }
            printf("src[w=%d,h=%d,f=%d,mask=%d]\n",src_width,src_height,src_format,src_buffer_mask);
        }
    }else{
        printf("please set the right arguments! "
                "Example: memc-test /data -i 3840 2160 rgb_888\n");
        return 0;
    }
#endif

    //1. get a memc instance
    Memc* memc = Memc::Get();
    if(memc == NULL){
        printf("Memc init check fail\n");
        return -1;
    }

    // 2. Alloc src buffer
    Buffer src_buffer0(src_width,
                    src_height,
                    src_format,
                    src_usage,
                    "MemcTestSrcBuffer");
    if (src_buffer0.Init()) {
        printf("GraphicBuffer check error : %s\n",strerror(errno));
        return -1;
    }else{
        printf("GraphicBuffer check %s \n","ok");
    }
    if(src_buffer0.FillFromFile(src0_path)){
        printf("buffer: FillFromFile fail! path=%s\n",src0_path);
        return 1;
    }
    src_buffer0.DumpData();

    Buffer src_buffer1(src_width,
                    src_height,
                    src_format,
                    src_usage,
                    "MemcTestSrcBuffer");
    if (src_buffer1.Init()) {
        printf("GraphicBuffer check error : %s\n",strerror(errno));
        return -1;
    }else{
        printf("GraphicBuffer check %s \n","ok");
    }
    if(src_buffer1.FillFromFile(src1_path)){
        printf("buffer: FillFromFile fail! path=%s\n",src1_path);
        return 1;
    }
    src_buffer1.DumpData();

#ifdef MEMC_PROXY_MODE

    // 3. Alloc dst buffer
    Buffer dst_buffer(dst_width,
                    dst_height,
                    dst_format,
                    dst_usage,
                    "MemcTestDstBuffer");
    if (dst_buffer.Init()) {
        printf("GraphicBuffer check error : %s\n",strerror(errno));
        return -1;
    }else{
        printf("GraphicBuffer check %s \n","ok");
    }

    MemcContext ctx;
    MemcImageInfo src0;
    MemcImageInfo src1;
    MemcImageInfo dst;
    int input_img_count = 0;
    wchar_t test_osd[] = L"online osd: 测试";
    for(int i = 0; i < 10; i++){
        // 4. Init Ctx and set flags
        int ret = memc->InitCtx(ctx);
        if(ret){
            printf("Memc ctx init fail!\n");
            return ret;
        }
        ctx.mMemcMode_ = MEMC_4K; //if not set, the program will automatically select the most appropriate mode
        ctx.mParam_.enableMemcOutput_ = true;  //use MEMC
        ctx.mParam_.enableMemcComparation_ = memc_comparation_mode;  //set compare Mode
        ctx.mParam_.mOsdMode_ = osd_mode; //set OSD mode
        if(ctx.mParam_.mOsdMode_ == MEMC_OSD_ENABLE_VIDEO_ONELINE){
            ctx.mParam_.mOsdVideo_ = test_osd;
        }

        ret = memc->SetCtxSequence(ctx, i); //alternative, if not set, run the default way
        if(ret){
            printf("Memc SetCtxSequence fail!\n");
            return ret;
        }

        ret = memc->MpSetInputFps(ctx, 30.0); //alternative, if not set, the default is 30.0
        if(ret){
            printf("Memc MpSetInputFps fail!\n");
            return ret;
        }

        // 5. Set memc dst buffer Info
        dst.mBufferInfo_.iFd_     = dst_buffer.GetFd();
        dst.mBufferInfo_.iWidth_  = dst_buffer.GetWidth();
        dst.mBufferInfo_.iHeight_ = dst_buffer.GetHeight();
        dst.mBufferInfo_.iFormat_ = dst_buffer.GetFormat();
        dst.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
        dst.mBufferInfo_.iStride_ = dst_buffer.GetStride();
        dst.mBufferInfo_.uBufferId_ = dst_buffer.GetBufferId();

        dst.mCrop_.iLeft_  = 0;
        dst.mCrop_.iTop_   = 0;
        dst.mCrop_.iRight_ = dst_buffer.GetWidth();
        dst.mCrop_.iBottom_= dst_buffer.GetHeight();

        if(i %2 == 0){
            // 6. Set memc src buffer Info
            if(input_img_count %2 == 0){
                src0.mBufferInfo_.iFd_     = src_buffer0.GetFd();
                src0.mBufferInfo_.iWidth_  = src_buffer0.GetWidth();
                src0.mBufferInfo_.iHeight_ = src_buffer0.GetHeight();
                src0.mBufferInfo_.iFormat_ = src_buffer0.GetFormat();
                src0.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
                src0.mBufferInfo_.iStride_ = src_buffer0.GetStride();
                src0.mBufferInfo_.uBufferId_ = src_buffer0.GetBufferId();

                src0.mCrop_.iLeft_  = 0;
                src0.mCrop_.iTop_   = 0;
                src0.mCrop_.iRight_ = src_buffer0.GetWidth();
                src0.mCrop_.iBottom_= src_buffer0.GetHeight();

                ret = memc->MpSetNextImage(ctx, src0);
                if(ret){
                    printf("Memc MpSetNextImage fail!\n");
                    return ret;
                }
            }else{
                src1.mBufferInfo_.iFd_     = src_buffer1.GetFd();
                src1.mBufferInfo_.iWidth_  = src_buffer1.GetWidth();
                src1.mBufferInfo_.iHeight_ = src_buffer1.GetHeight();
                src1.mBufferInfo_.iFormat_ = src_buffer1.GetFormat();
                src1.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
                src1.mBufferInfo_.iStride_ = src_buffer1.GetStride();
                src1.mBufferInfo_.uBufferId_ = src_buffer1.GetBufferId();

                src1.mCrop_.iLeft_  = 0;
                src1.mCrop_.iTop_   = 0;
                src1.mCrop_.iRight_ = src_buffer1.GetWidth();
                src1.mCrop_.iBottom_= src_buffer1.GetHeight();

                ret = memc->MpSetNextImage(ctx, src1);
                if(ret){
                    printf("Memc MpSetNextImage fail!\n");
                    return ret;
                }
            }
            input_img_count++;

            // 7. Run Async, then get an output image from memc instance
            ret = memc->MpQueueCtxAndRunAsync(ctx);
            if(ret){
                printf("Memc Run fail!\n");
                return ret;
            }
            printf("Memc MpQueueCtxAndRunAsync success!\n");

            ret = memc->MpGetOutputImage(ctx, &dst);
            if(ret){
                printf("Memc MpGetOutputImage fail!");
                return ret;
            }
            printf("Memc MpGetOutputImage success!\n");
        }else{
            // 8. Get an output image from memc instance
            int ret = memc->MpGetOutputImage(ctx, &dst);
            if(ret){
                printf("Memc MpGetOutputImage fail!");
                return ret;
            }
            printf("Memc MpGetOutputImage success!\n");
        }

        // 9. Check the dst_buffer
        dst_buffer.DumpData();
        printf("Memc dump data to /data/dump success!\n");
    }

    //10. clear cache info, and ready for a new round of memc process
    //you can reset the format\size\etc. of input or output buffer after this
    memc->MpClearInternalCacheInfo(ctx);
    printf("Memc end!\n");

#else
    Buffer *dst_buffers[3];
    for(int i = 0; i < 5; i++){
        MemcContext ctx;
        MemcImageInfo src0;
        MemcImageInfo src1;
        MemcImageInfo require_dst;
        MemcImageInfo dst;
        int memc_fence = 0;
        // 3. Init Ctx and set flags
        int ret = memc->InitCtx(ctx);
        if(ret){
            printf("Memc ctx init fail!\n");
            return ret;
        }
        ctx.mMemcMode_ = MEMC_4K; //if not set, the program will automatically select the most appropriate mode

        ret = memc->SetCtxSequence(ctx, i); //alternative, if not set, run the default way
        if(ret){
            printf("Memc SetCtxSequence fail!\n");
            return ret;
        }

        // 4. Set two src-image info
        src0.mBufferInfo_.iFd_     = src_buffer0.GetFd();
        src0.mBufferInfo_.iWidth_  = src_buffer0.GetWidth();
        src0.mBufferInfo_.iHeight_ = src_buffer0.GetHeight();
        src0.mBufferInfo_.iFormat_ = src_buffer0.GetFormat();
        src0.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
        src0.mBufferInfo_.iStride_ = src_buffer0.GetStride();
        src0.mBufferInfo_.uBufferId_ = src_buffer0.GetBufferId();
        src0.mCrop_.iLeft_  = 0;
        src0.mCrop_.iTop_   = 0;
        src0.mCrop_.iRight_ = src_buffer0.GetWidth();
        src0.mCrop_.iBottom_= src_buffer0.GetHeight();

        src1.mBufferInfo_.iFd_     = src_buffer1.GetFd();
        src1.mBufferInfo_.iWidth_  = src_buffer1.GetWidth();
        src1.mBufferInfo_.iHeight_ = src_buffer1.GetHeight();
        src1.mBufferInfo_.iFormat_ = src_buffer1.GetFormat();
        src1.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
        src1.mBufferInfo_.iStride_ = src_buffer1.GetStride();
        src1.mBufferInfo_.uBufferId_ = src_buffer1.GetBufferId();
        src1.mCrop_.iLeft_  = 0;
        src1.mCrop_.iTop_   = 0;
        src1.mCrop_.iRight_ = src_buffer1.GetWidth();
        src1.mCrop_.iBottom_= src_buffer1.GetHeight();

        ret = memc->MnSetSrcImage(ctx, src0, src1);
        if(ret){
            printf("Memc MnSetSrcImage fail!\n");
            return ret;
        }

        // 5. Get Dst buffer info, must use the info to alloc dst buffer
        ret = memc->MnGetDstRequireInfo(ctx, require_dst);
        if(dst_buffers[i%3] == NULL){
            dst_buffers[i%3] = new Buffer(require_dst.mBufferInfo_.iWidth_,
                                        require_dst.mBufferInfo_.iHeight_,
                                        require_dst.mBufferInfo_.iFormat_,
                                        RK_GRALLOC_USAGE_STRIDE_ALIGN_16 | MALI_GRALLOC_USAGE_NO_AFBC,
                                        "MemcTestDstBuffer");
        }
        if(dst_buffers[i%3]->Init()){
            printf("GraphicBuffer check error : %s\n",strerror(errno));
            return -1;
        }else{
            printf("GraphicBuffer check %s \n","ok");
        }

        // 6. Set dst image info
        dst.mBufferInfo_.iFd_     = dst_buffers[i%3]->GetFd();
        dst.mBufferInfo_.iWidth_  = dst_buffers[i%3]->GetWidth();
        dst.mBufferInfo_.iHeight_ = dst_buffers[i%3]->GetHeight();
        dst.mBufferInfo_.iFormat_ = dst_buffers[i%3]->GetFormat();
        dst.mBufferInfo_.uBufferMask_ = MemcBufferMask::NONE;  //Not AFBC
        dst.mBufferInfo_.iStride_ = dst_buffers[i%3]->GetStride();
        dst.mBufferInfo_.uBufferId_ = dst_buffers[i%3]->GetBufferId();
        dst.mCrop_.iLeft_  = require_dst.mCrop_.iLeft_;
        dst.mCrop_.iTop_   = require_dst.mCrop_.iTop_;
        dst.mCrop_.iRight_ = require_dst.mCrop_.iRight_;
        dst.mCrop_.iBottom_= require_dst.mCrop_.iBottom_;

        ret = memc->MnSetDstImage(ctx, dst);
        if(ret){
            printf("Memc MnSetDstImage fail!\n");
            return ret;
        }

        // 7. Run memc and get a fence of dst buffer
        ret = memc->MnRunMemcAsync(ctx, &memc_fence);
        if(ret){
            printf("Memc MnRunMemcAsync fail!\n");
            return ret;
        }
        printf("Memc MnRunMemcAsync success! memc_fence=%d\n", memc_fence);

        // 8. Wait the memc fence and check output
        if(memc_fence > 0){
            int sync_ret = sync_wait(memc_fence, 1500);
            if (sync_ret) {
                printf("Failed to wait fence %d/%d 1500ms!\n", memc_fence, sync_ret);
            }else{
                printf("wait fence %d success!\n", memc_fence);
            }
            close(memc_fence);
        }
        dst_buffers[i%3]->DumpData();
        printf("Memc dump data to /data/dump success!\n");
    }
    printf("Memc end!\n");
#endif

    return 0;
}

