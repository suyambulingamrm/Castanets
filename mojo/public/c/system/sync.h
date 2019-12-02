// Copyright 2019 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_C_SYSTEM_SYNC_H_
#define MOJO_PUBLIC_C_SYSTEM_SYNC_H_

#include <stdint.h>

#include "mojo/public/c/system/system_export.h"
#include "mojo/public/c/system/types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum BrokerCompressionMode {
  NONE,
  ZLIB,
  WEBP
};

struct BrokerCompressionData {
  BrokerCompressionData()
      : compression_mode(ZLIB), buffer_width(0),
        buffer_height(0), buffer_stride(0), quality_factor(0) {}

  BrokerCompressionData(BrokerCompressionMode mode,
                        int width,
                        int height,
                        int stride,
                        int quality = 100)
      : compression_mode(mode), buffer_width(width),
        buffer_height(height), buffer_stride(stride),
        quality_factor(quality) {}

  uint32_t width() { return buffer_width; }
  uint32_t height() { return buffer_height; }
  uint32_t stride() { return buffer_stride; }
  uint32_t quality() { return quality_factor; }
  BrokerCompressionMode mode() { return compression_mode; }

  BrokerCompressionMode compression_mode;
  uint32_t buffer_width;
  uint32_t buffer_height;
  uint32_t buffer_stride;
  uint32_t quality_factor;
};

MOJO_SYSTEM_EXPORT MojoResult MojoSyncPlatformSharedMemoryRegion(
    const struct MojoSharedBufferGuid* guid,
    size_t offset,
    size_t sync_size,
    BrokerCompressionData compression_data = BrokerCompressionData());

MOJO_SYSTEM_EXPORT MojoResult
MojoSyncPlatformSharedMemoryRegion2d(
    const struct MojoSharedBufferGuid* guid,
    size_t offset,
    size_t sync_size,
    size_t width,
    size_t stride,
    BrokerCompressionData compression_data = BrokerCompressionData());

MOJO_SYSTEM_EXPORT MojoResult MojoWaitSyncPlatformSharedMemoryRegion(
    const struct MojoSharedBufferGuid* guid);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MOJO_PUBLIC_C_SYSTEM_SYNC_H_
