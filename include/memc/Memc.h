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

#include "MemcType.h"
#include <utils/RefBase.h>

namespace android {

class MemcBackend;

class Memc {
public:
  static Memc* Get(bool async=false) { return GetInstance(async); }
  ~Memc();
  Memc(const Memc&)=delete;
  Memc& operator=(const Memc&)=delete;

  // Init Context info
  int InitCtx(MemcContext &ctx);
  //set input Sequence
  int SetCtxSequence(MemcContext &ctx, int inputCtxSeq = 0);

  /**
   * MEMC_PROXY_MODE
   * @brief: input 1 image, output 1 image from internal bufferqueue
   * Customer don't need to manage buffer and fence.
   */
  //set input fps
  int MpSetInputFps(MemcContext &ctx, float inputFps = 30.0);
  // Set next-image info
  int MpSetNextImage(MemcContext &ctx, const MemcImageInfo &src);
  // Run async return immediately
  int MpQueueCtxAndRunAsync(const MemcContext &ctx);
  // Get output image from bufferquque
  int MpGetOutputImage(const MemcContext &ctx, MemcImageInfo *outImage);
  // Clear the cached information
  int MpClearInternalCacheInfo();

  /**
   * MEMC_NATIVE_MODE
   * @brief: input 2 src images, output the MEMC image and fence
   * Customer should manage buffer and fence proactively.
   */
  // Set two src-image info
  int MnSetSrcImage(MemcContext &ctx, const MemcImageInfo &src0, const MemcImageInfo &src1);
  // Get dst-image require info
  int MnGetDstRequireInfo(MemcContext &ctx, MemcImageInfo &require);
  // Set dst-image info
  int MnSetDstImage(MemcContext &ctx, const MemcImageInfo &dst);
  // Run and get FinishFence
  int MnRunMemcAsync(const MemcContext &ctx, int *outFence);

private:
  Memc();
  int Init(bool async=false);
  static Memc* GetInstance(bool async = false){
    static Memc memc;
    if(memc.Init(async))
      return NULL;
    else
      return &memc;
  }

  // Verify dlss context
  int VerifyCtx(const MemcContext &ctx, const MemcStage stage);
  // Verify src info
  int VerifySrcInfo(MemcContext &ctx, const MemcImageInfo &src);
  // Verify MemcMode
  int VerifyMode(MemcContext &ctx, const MemcImageInfo &src);
  // Verify dst info
  int VerifyDstInfo(MemcContext &ctx, const MemcImageInfo &dst);
  //check if need to convert src to 1080p/4k rgb888 buffer
  bool CheckIfConvertSrc(const MemcContext &ctx);
  // Dump Ctx info
  int DumpCtx(const MemcContext &ctx);
  MemcError bInitState_ = MemcError::MemcUnInit;
  MemcVersion mVersion_;
  std::shared_ptr<MemcBackend> mMemcBackend_;
  mutable pthread_mutex_t mLock_;
};
} //namespace android