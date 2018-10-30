// Copyright 2018 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_SHARED_MEMORY_CASTANETS_HELPER_H_
#define BASE_MEMORY_SHARED_MEMORY_CASTANETS_HELPER_H_
#include "base/command_line.h"

namespace base {

namespace nfs_util {

#if defined(NETWORK_SHARED_MEMORY)
// This method flushes the changes on memory mapped region to
// the underlying network file system.
inline void FlushToDisk(int fd) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  std::string process_type =
      command_line.GetSwitchValueASCII("type");
  if (process_type != "renderer") {
    return;
  }
  fdatasync(fd);
}

// Temporary workaround to get nfs server->nfs client updates
// synched.
inline void Sync(int fd) {
  FILE* fp = fdopen(fd, "rw");
  fseek(fp, 0L, SEEK_END);
}

inline void EvictPagesInRam(int fd) {
  char filePath[1000];
  char proclnk[0xFFF];
  ssize_t r;
  sprintf(proclnk, "/proc/self/fd/%d", fd);
  r = readlink(proclnk, filePath, 0xFFF);
  filePath[r] = '\0';
  std::stringstream ss;
  ss<<"vmtouch -e "<<filePath<<" ;";
  system(ss.str().c_str());
}

inline void GpuSync(int fd) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  std::string process_type =
      command_line.GetSwitchValueASCII("type");
  if (process_type != "renderer") {
    return;
  }
  FILE* fp = fdopen(fd, "rw");
#if 1
  fseek(fp, 0L, SEEK_END);
  char c;
  fread(&c, 1, 1, fp);
#else
  char filePath[1000];
  char proclnk[0xFFF];
  int fno;
  ssize_t r;
  if (fp != NULL)
  {
    fno = fileno(fp);
    sprintf(proclnk, "/proc/self/fd/%d", fno);
    r = readlink(proclnk, filePath, 0xFFF);
    filePath[r] = '\0';
    std::stringstream ss2;
    ss2<<"tail -c 2 "<<filePath<<" > dummy ;";
    system(ss2.str().c_str());
  }
#endif
}

}  // namespace nfs_util

}  // namespace base

#endif

#endif  // BASE_MEMORY_SHARED_MEMORY_CASTANETS_HELPER_H_
