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
#pragma once

#include "autofd.h"
#include <stdint.h>
#include <utils/Trace.h>
#include <cutils/properties.h>
#include <time.h>

namespace android {

#define SVEP_MAGIC 0x83991906
#define SVEP_VERSION "Svep-1.1.1"

#define SVEP_MODE_NAME "persist.sys.svep.mode"

#define SVEP_CONTRAST_MODE_ENABLE 1
#define SVEP_CONTRAST_MODE_LINE_WIDTH 4;
#define SVEP_CONTRAST_MODE_NAME   "persist.sys.svep.contrast_mode"
#define SVEP_CONTRAST_MODE_OFFSET "persist.sys.svep.contrast_offset_ratio"

#define SVEP_AVG_COST_TIME_NAME "vendor.svep.avg_cost_time"

#define SVEP_SUBITLE_ENABLE 1
#define SVEP_SUBITLE_PATH "/vendor/etc/RKNPU-AI-892x136-RGBA.bin"
#define SVEP_SUBITLE_W 892
#define SVEP_SUBITLE_H 136


#define SVEP_ALOGE(x, ...)  \
    ALOGE("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define SVEP_ALOGW(x, ...)  \
    ALOGW("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define SVEP_ALOGI(x, ...)  \
    ALOGI("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define YUV_ALIGN 2
#define RGB_ALIGN 1

#define IS_ALIGN(val,align)    (((val)&(align-1))==0)
#ifndef ALIGN
#define ALIGN( value, base ) (((value) + ((base) - 1)) & ~((base) - 1))
#endif
#define ALIGN_DOWN( value, base)	(value & (~(base-1)) )

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


enum SvepError {
    None = 0,
    BadVersion,
    BadStage,
    BadParameter,
    UnSupported,
    UnSucess,
    UnInit,
};

enum SvepStage {
    SVEP_UN_INIT = 0,
    SVEP_INIT_SUCCESS,
    SVEP_VERITY_SRC_SUCCESS,
    SVEP_GET_REQUIRE_SUCCESS,
    SVEP_VERITY_DST_SUCCESS,
};

enum SvepMode {
    UN_SUPPORT = 0,
    SVEP_360p,
    SVEP_540p,
    SVEP_720p,
    SVEP_1080p,
};

enum SvepFeatureFunction {
    NONE = 0,
    SVEP_SUBTITLE_TIPS = 1 << 1,
    SVEP_AUTO_SLIDE    = 1 << 2,
    SVEP_CUSTOM_SLIDE  = 1 << 3,

};

struct SvepVersion{
  int iMajor_;
  int iMinor_;
  int iPatchLevel_;
};

struct SvepModeRequireInfo{
  SvepMode mode_;
  int iSvepSrcWidth_;
  int iSvepSrcHeight_;
  int iSvepSrcFormat_;
  int iSvepScale_;
  int iSvepDstFormat_;
  char cName[20];
};

class SvepRect{
public:
  int iLeft_;
  int iTop_;
  int iRight_;
  int iBottom_;

  int Width() const {return iRight_ - iLeft_; };
  int Height() const {return iBottom_ - iTop_; };

  SvepRect() = default;
  SvepRect(const SvepRect& rhs){
    iLeft_   = rhs.iLeft_;
    iTop_    = rhs.iTop_;
    iRight_  = rhs.iRight_;
    iBottom_ = rhs.iBottom_;
  };

  SvepRect& operator=(const SvepRect& rhs){
    iLeft_   = rhs.iLeft_;
    iTop_    = rhs.iTop_;
    iRight_  = rhs.iRight_;
    iBottom_ = rhs.iBottom_;
    return *this;
  };

  bool isValid() const {
    return Width() > 0 && Height() > 0;
  };
};

class SvepBufferInfo{
public:
  int iFd_;
  int iWidth_;
  int iHeight_;
  int iFormat_;
  int iStride_;
  uint64_t uBufferId_;
  uint64_t uDataSpace_;

  SvepBufferInfo() = default;
  SvepBufferInfo(const SvepBufferInfo& rhs){
    iFd_     = rhs.iFd_;
    iWidth_  = rhs.iWidth_;
    iHeight_ = rhs.iHeight_;
    iFormat_ = rhs.iFormat_;
    iStride_ = rhs.iStride_;
    uBufferId_ = rhs.uBufferId_;
    uDataSpace_ =  rhs.uDataSpace_;
  };

  SvepBufferInfo& operator=(const SvepBufferInfo& rhs){
    iFd_     = rhs.iFd_;
    iWidth_  = rhs.iWidth_;
    iHeight_ = rhs.iHeight_;
    iFormat_ = rhs.iFormat_;
    iStride_ = rhs.iStride_;
    uBufferId_ = rhs.uBufferId_;
    uDataSpace_ =  rhs.uDataSpace_;
    return *this;
  };

  bool isValid() const {
    return iFd_ > 0 &&
           iWidth_ > 0 &&
           iHeight_ > 0 &&
           iStride_ > 0 &&
           iFormat_ > 0;
  }
};

class SvepImageInfo{
public:
  SvepBufferInfo mBufferInfo_;
  SvepRect mCrop_;
  UniqueFd mAcquireFence_;
  bool mValid;
  SvepImageInfo() = default;
  SvepImageInfo(const SvepImageInfo& rhs){
    mBufferInfo_ = rhs.mBufferInfo_;
    mCrop_ = rhs.mCrop_;
    mAcquireFence_ = rhs.mAcquireFence_.Dup();
  };

  SvepImageInfo& operator=(const SvepImageInfo& rhs){
    mBufferInfo_ = rhs.mBufferInfo_;
    mCrop_ = rhs.mCrop_;
    mAcquireFence_ = rhs.mAcquireFence_.Dup();
    return *this;
  };
};

class SvepContext {
public:
  int mMagic_;
  SvepVersion mVersion_;
  SvepStage mStage_;
  SvepMode mSvepMode_;
  SvepImageInfo mSvepSrc_;
  SvepImageInfo mSvepDst_;
  SvepImageInfo mSrc_;
  SvepImageInfo mDst_;
};

class SvepBackendContext {
public:
  int mMagic_;
  SvepVersion mVersion_;
  SvepStage mStage_;
  SvepMode mSvepMode_;
  SvepImageInfo mSvepSrc_;
  SvepImageInfo mSvepDst_;
  SvepImageInfo mSrc_;
  SvepImageInfo mDst_;
  int iFenceTimeline_;
  UniqueFd ufCurrentFinishFence_;
  struct timeval mStartTime_;

  SvepBackendContext(const SvepContext ac):
    mMagic_(ac.mMagic_),
    mVersion_(ac.mVersion_),
    mStage_(ac.mStage_),
    mSvepMode_(ac.mSvepMode_),
    mSvepSrc_(ac.mSvepSrc_),
    mSvepDst_(ac.mSvepDst_),
    mSrc_(ac.mSrc_),
    mDst_(ac.mDst_),
    iFenceTimeline_(0){};
};

} // namespace android