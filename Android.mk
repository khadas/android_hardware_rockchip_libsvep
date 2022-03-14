#
# Copyright (C) 2018 Fuzhou Rockchip Electronics Co.Ltd.
#
# Modification based on code covered by the Apache License, Version 2.0 (the "License").
# You may not use this software except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS TO YOU ON AN "AS IS" BASIS
# AND ANY AND ALL WARRANTIES AND REPRESENTATIONS WITH RESPECT TO SUCH SOFTWARE, WHETHER EXPRESS,
# IMPLIED, STATUTORY OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF TITLE,
# NON-INFRINGEMENT, MERCHANTABILITY, SATISFACTROY QUALITY, ACCURACY OR FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.
#
# IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# BOARD_USES_LIBSVEP=false
# BOARD_USES_LIBSVEP=true
ifeq ($(strip $(BOARD_USES_LIBSVEP)),true)

include $(CLEAR_VARS)
LOCAL_MODULE := libOpenCL
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_STEM := $(LOCAL_MODULE)
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := true
LOCAL_PROPRIETARY_MODULE := true
ifneq ($(strip $(TARGET_2ND_ARCH)), )
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/libOpenCL.so
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/$(TARGET_2ND_ARCH)/libOpenCL.so
else
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/libOpenCL.so
endif
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := librknnrt-svep
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
##LOCAL_VENDOR_MODULE := true
LOCAL_VENDOR_MODULE := true
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SHARED_LIBRARIES := libc libdl liblog libm libstdc++ libz
ifneq ($(strip $(TARGET_2ND_ARCH)), )
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/librknnrt-svep.so
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/$(TARGET_2ND_ARCH)/librknnrt-svep.so
else
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/librknnrt-svep.so
endif
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librkvdlss
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SHARED_LIBRARIES := libOpenCL libc libdl libm librknnrt-svep liblog libz
LOCAL_VENDOR_MODULE := true
LOCAL_PROPRIETARY_MODULE := true
ifneq ($(strip $(TARGET_2ND_ARCH)), )
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/librkvdlss.so
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/$(TARGET_2ND_ARCH)/librkvdlss.so
else
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/librkvdlss.so
endif
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsvep
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SHARED_LIBRARIES := \
  libcutils \
  liblog \
  libui \
  libutils \
  libsync_vendor \
  librga \
  libOpenCL \
  librkvdlss \
  librknnrt-svep \
  libhidlbase \
  libgralloctypes \
  android.hardware.graphics.mapper@4.0

LOCAL_PROPRIETARY_MODULE := true
ifneq ($(strip $(TARGET_2ND_ARCH)), )
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/libsvep.so
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/$(TARGET_2ND_ARCH)/libsvep.so
else
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib/$(TARGET_ARCH)/libsvep.so
endif
include $(BUILD_PREBUILT)

## copy init.qcom.test.rc from etc to /vendor/etc/init/hw
include $(CLEAR_VARS)
LOCAL_MODULE := RKNPU-AI-892x136-RGBA.bin
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := res/RKNPU-AI-892x136-RGBA.bin
LOCAL_MODULE_PATH :=$(PRODUCT_OUT)/vendor/etc/
include $(BUILD_PREBUILT)

endif