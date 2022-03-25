/*
 * Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
 *
 *
 * @Author: Randall Zhuo
 * @Date: 2021-10-20 14:08:09
 * @LastEditors: Randall
 * @LastEditTime: 2022-01-19 15:47:58
 * @Description: TODO
 */

#pragma once

typedef void*  VDLSSBUFFERHANDLE;

class VDLSSImpl;

class VDLSSBUFFER
{
public:
    int fd;
    int width;
    int height;
    int stride;
    int size;
    int fmt;
};

class VDLSS
{
private:
    VDLSSImpl *impl;
private:
    /* data */
   int init();
public:
    VDLSS(int input_width=1280, int input_height=720, int output_width=3840, int output_height=2160);
    ~VDLSS();
    bool verify();
    int run(VDLSSBUFFERHANDLE inputBufferHandle);
    int postProcess(VDLSSBUFFERHANDLE outputBufferHandle, bool enable_sharpening = true);

    VDLSSBUFFERHANDLE createInputBufferHandle(VDLSSBUFFER &inputBuffer);
    int destoryInputBufferHandle(VDLSSBUFFERHANDLE handle);

    VDLSSBUFFERHANDLE createOutputBufferHandle(VDLSSBUFFER &outBuffer);
    int destoryOutputBufferHandle(VDLSSBUFFERHANDLE handle);

};
