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

#define MEMC_MAGIC 0x83991906
#define MEMC_VERSION "Memc-1.3.1"
#define MEMC_VERSION_NAME "vendor.memc.version"
#define MEMC_DEBUG_NAME "vendor.memc.log"

// Vendor Storage ID.
#define MEMC_VENDOR_AUTHOR_ID "ro.vendor.memc.vsid"

#define MEMC_ALOGE(x, ...)  \
    ALOGE("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define MEMC_ALOGW(x, ...)  \
    ALOGW("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define MEMC_ALOGI(x, ...)  \
    ALOGI("%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define MEMC_ALOGD_IF(x, ...)  \
    ALOGD_IF(MemcLogLevel(), "%s,line=%d " x ,__FUNCTION__,__LINE__, ##__VA_ARGS__)

#define YUV_ALIGN 2
#define RGB_ALIGN 1

#define IS_ALIGN(val,align)    (((val)&(align-1))==0)
#ifndef ALIGN
#define ALIGN( value, base ) (((value) + ((base) - 1)) & ~((base) - 1))
#endif
#define ALIGN_DOWN( value, base)	(value & (~(base-1)) )

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int UpdateMemcLogLevel();
bool MemcLogLevel();

enum MemcError {
    MemcNone = 0,
    MemcBadVersion,
    MemcBadStage,
    MemcBadParameter,
    MemcUnLicence,
    MemcUnSupported,
    MemcUnSucess,
    MemcUnInit,
};

enum MemcStage {
    MEMC_UN_INIT = 0,
    MEMC_INIT_SUCCESS,
    MEMC_VERITY_SRC_SUCCESS,
    MEMC_GET_REQUIRE_SUCCESS,
    MEMC_VERITY_DST_SUCCESS,
};

enum MemcBufferMask {
    NONE = 0,
    MEMC_AFBC_FORMAT = 1 << 1,
};

enum MemcMode {
    MEMC_UN_SUPPORT = 0,
    MEMC_1080P,
    MEMC_4K,
};

struct MemcModeRequireInfo{
  MemcMode mode_;
  int iMemcSrcWidth_;
  int iMemcSrcHeight_;
  char cName[20];
};

struct MemcVersion{
  int iMajor_;
  int iMinor_;
  int iPatchLevel_;
};

enum MemcOsdMode {
    MEMC_OSD_DISABLE = 0,
    MEMC_OSD_ENABLE_VIDEO,
    MEMC_OSD_ENABLE_VIDEO_ONELINE,
};

class MemcParameter{
public:
  // Memc Osd Mode : 0 / 1 / 2
  MemcOsdMode mOsdMode_;
  const wchar_t* mOsdVideo_;
  bool enableMemcOutput_;
  bool enableMemcComparation_;

  MemcParameter() = default;
  MemcParameter(const MemcParameter& rhs){
    mOsdMode_ = rhs.mOsdMode_;
    mOsdVideo_ = rhs.mOsdVideo_;
    enableMemcOutput_ = rhs.enableMemcOutput_;
    enableMemcComparation_ = rhs.enableMemcComparation_;
  };

  MemcParameter& operator=(const MemcParameter& rhs){
    mOsdMode_ = rhs.mOsdMode_;
    mOsdVideo_ = rhs.mOsdVideo_;
    enableMemcOutput_ = rhs.enableMemcOutput_;
    enableMemcComparation_ = rhs.enableMemcComparation_;
    return *this;
  };
};

class MemcRect{
public:
  int iLeft_;
  int iTop_;
  int iRight_;
  int iBottom_;

  int Width() const {return iRight_ - iLeft_; };
  int Height() const {return iBottom_ - iTop_; };

  MemcRect() = default;
  MemcRect(const MemcRect& rhs){
    iLeft_   = rhs.iLeft_;
    iTop_    = rhs.iTop_;
    iRight_  = rhs.iRight_;
    iBottom_ = rhs.iBottom_;
  };

  MemcRect& operator=(const MemcRect& rhs){
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

class MemcBufferInfo{
public:
  int iFd_;
  int iWidth_;
  int iHeight_;
  int iFormat_;
  int iStride_;
  uint64_t uBufferId_;
  uint64_t uDataSpace_;
  uint64_t uBufferMask_;

  MemcBufferInfo() = default;
  MemcBufferInfo(const MemcBufferInfo& rhs){
    iFd_     = rhs.iFd_;
    iWidth_  = rhs.iWidth_;
    iHeight_ = rhs.iHeight_;
    iFormat_ = rhs.iFormat_;
    iStride_ = rhs.iStride_;
    uBufferId_ = rhs.uBufferId_;
    uDataSpace_ =  rhs.uDataSpace_;
    uBufferMask_ =  rhs.uBufferMask_;

  };
  MemcBufferInfo& operator=(const MemcBufferInfo& rhs){
    iFd_     = rhs.iFd_;
    iWidth_  = rhs.iWidth_;
    iHeight_ = rhs.iHeight_;
    iFormat_ = rhs.iFormat_;
    iStride_ = rhs.iStride_;
    uBufferId_ = rhs.uBufferId_;
    uDataSpace_ =  rhs.uDataSpace_;
    uBufferMask_ =  rhs.uBufferMask_;
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

class MemcImageInfo{
public:
  MemcBufferInfo mBufferInfo_;
  MemcRect mCrop_;
  UniqueFd mAcquireFence_;
  UniqueFd mReleaseFence_;
  bool mValid;
  MemcImageInfo() = default;
  MemcImageInfo(const MemcImageInfo& rhs){
    mBufferInfo_ = rhs.mBufferInfo_;
    mCrop_ = rhs.mCrop_;
    mAcquireFence_ = rhs.mAcquireFence_.Dup();
    mReleaseFence_ = rhs.mReleaseFence_.Dup();
    mValid = rhs.mValid;
  };

  MemcImageInfo& operator=(const MemcImageInfo& rhs){
    mBufferInfo_ = rhs.mBufferInfo_;
    mCrop_ = rhs.mCrop_;
    mAcquireFence_ = rhs.mAcquireFence_.Dup();
    mReleaseFence_ = rhs.mReleaseFence_.Dup();
    mValid = rhs.mValid;
    return *this;
  };
};

class MemcContext {
public:
  int mMagic_;
  MemcVersion mVersion_;
  MemcStage mStage_;
  MemcImageInfo mSrc_;     //输入 image0, 必须设置
  MemcImageInfo mSrcSub_;  //输入 image1, 在 MEMC_SELF_MANAGE_MODE 工作状态下不用设置
  MemcImageInfo mMemcDst_; //Memc 输出 image, 在 MEMC_SELF_MANAGE_MODE 工作状态下不用设置
  MemcParameter mParam_;
  MemcMode mMemcMode_;     //可以设置 Memc 模式，若不设置则内部自动选择最合适的模式
  float mInputFps_;
  int mCtxSequence_;
};

} // namespace android
