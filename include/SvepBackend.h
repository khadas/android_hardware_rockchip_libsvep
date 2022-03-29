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

#include "RKSVEP.h"
#include "SvepType.h"
#include "Buffer.h"
#include "BufferQueue.h"

#include "NpuWorker.h"
#include "GpuWorker.h"

#include <map>
#include <queue>
#include <memory>

namespace android {

// SvepBackend 主要职责为申请中间缓存，调用NPU/GPU实现AIEE
class SvepBackend {
public:
  SvepBackend();
  ~SvepBackend();
  int Init();

  int QueueCtxAndRun(const SvepContext &ctx);
  int QueueCtxAndRunAsync(const SvepContext &ctx, int *outFence);

private:
  std::shared_ptr<Buffer> DequeueSvepBuffer(std::shared_ptr<SvepBackendContext> ctx);
  int QueueSvepBuffer(std::shared_ptr<SvepBackendContext> abCtx, const std::shared_ptr<Buffer> buffer);
  int ConvertToRgb888(const SvepImageInfo &srcBuffer, std::shared_ptr<Buffer> dstBuffer);
  bool mInit_;
  std::shared_ptr<BufferQueue> mPtrBQ360p_;
  std::shared_ptr<BufferQueue> mPtrBQ540p_;
  std::shared_ptr<BufferQueue> mPtrBQ720p_;
  std::shared_ptr<BufferQueue> mPtrBQ1080p_;
  std::shared_ptr<Buffer> mSubtitleBuffer_;
  NpuWorker NpuWorker_;
};

} //namespace android