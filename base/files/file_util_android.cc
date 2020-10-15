// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#include "base/files/file_path.h"
#include "base/path_service.h"

namespace base {

bool GetShmemTempDir(bool executable, base::FilePath* path) {
#if defined(CASTANETS)
  *path = FilePath("/data/user/0/com.samsung.android.castanets/cache/");
  return true;
#else
  return PathService::Get(base::DIR_CACHE, path);
#endif
}

}  // namespace base
