/******************************************************************************
 * Copyright 2022  Authors. All Rights Reserved.
 *****************************************************************************/

#pragma once

#include <g3sinks/LogRotate.h>
#include <g3sinks/LogRotateWithFilter.h>
#include <stdarg.h>

#include <fstream>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iostream>
#include <string>

#include "common/timestamp_util.h"
using namespace g3;
using mfn::common::GetDuration;
using mfn::common::GetSynchronizedTimestamp;

#define SINFO LOG(INFO)
#define SDEBUG LOG(DEBUG)
#define SWARN LOG(WARNING)
#define SERROR LOG(G3LOG_ERROR)
#define SFATAL LOG(FATAL)
#define SSYSTEM LOG(G3LOG_SYSTEM)

// 调试日志，建议使用
#define SLOG(cond) LOG_LEVEL(DEBUG, cond) << "[" << cond << "]"

// will be deleted
#define VLOG(cond) SLOG(cond)

#define SCHECK(cond) SDEBUG_IF(0)

#define CHECK(cond) SCHECK(cond)

#define SDEBUG_IF(cond) LOG_IF(DEBUG, cond)
#define SINFO_IF(cond) LOG_IF(INFO, cond)
#define SWARN_IF(cond) LOG_IF(WARNING, cond)
#define SERROR_IF(cond) LOG_IF(G3LOG_ERROR, cond)
#define SSYSTEM_IF(cond) LOG_IF(G3LOG_SYSTEM, cond)

#define LOG_EVERY2(LEVEL, n)                                       \
  static int G3LOG_OCCURRENCES = 0, G3LOG_OCCURRENCES_MOD_N = 0;   \
  ++G3LOG_OCCURRENCES;                                             \
  if (++G3LOG_OCCURRENCES_MOD_N > n) G3LOG_OCCURRENCES_MOD_N -= n; \
  if (G3LOG_OCCURRENCES_MOD_N != 1)                                \
  {                                                                \
  }                                                                \
  else                                                             \
    LOG(LEVEL) << "[" << G3LOG_OCCURRENCES << "]"

// 在同一定义域内只能使用一个，如果需要使用多个，请加上{}
#define SDEBUG_EVERY(cond) LOG_EVERY2(DEBUG, cond)
#define SINFO_EVERY(cond) LOG_EVERY2(INFO, cond)
#define SWARN_EVERY(cond) LOG_EVERY2(WARNING, cond)
#define SERROR_EVERY(cond) LOG_EVERY2(G3LOG_ERROR, cond)
#define SSYSTEM_EVERY(cond) LOG_EVERY2(G3LOG_SYSTEM, cond)

// 所有的check都不进行程序中断，请改为功能逻辑处理
#define CHECK_GE(x, y) \
  SERROR_IF(x < y) << "CHECK_GE x:" << x << " y:" << y << " "
#define CHECK_GT(x, y) \
  SERROR_IF(x <= y) << "CHECK_GT x:" << x << " y:" << y << " "
#define CHECK_EQ(x, y) \
  SERROR_IF(x != y) << "CHECK_EQ x:" << x << " y:" << y << " "
#define CHECK_NOTNULL(x) SERROR_IF(x == NULL) << "CHECK_NOTNULL "
#define CHECK_LE(x, y) \
  SERROR_IF(x > y) << "CHECK_LE x:" << x << " y:" << y << " "
#define CHECK_LT(x, y) \
  SERROR_IF(x >= y) << "CHECK_LT x:" << x << " y:" << y << " "

#define LOG_SEC(LEVEL, n)                                            \
  static double SEC = (#n[0]) ? n : 1;                               \
  static uint64_t count = 0;                                         \
  static uint64_t last_count = 0;                                    \
  count++;                                                           \
  double G3LOG_CURRY_TIME = GetSynchronizedTimestamp();              \
  static double G3LOG_LAST_TIME = 0;                                 \
  double TIME_DIFF = GetDuration(G3LOG_LAST_TIME, G3LOG_CURRY_TIME); \
  uint64_t diff_num = count - last_count;                            \
  if (TIME_DIFF >= SEC)                                              \
  {                                                                  \
    last_count = count;                                              \
    G3LOG_LAST_TIME = G3LOG_CURRY_TIME;                              \
  }                                                                  \
  if (TIME_DIFF < SEC)                                               \
  {                                                                  \
  }                                                                  \
  else                                                               \
    LOG(LEVEL) << "[" << diff_num << "/" << SEC << "s]"

// 在同一定义域内只能使用一个，如果需要使用多个，请加上{}
// 每隔一段时间打印日志(单位s)
// SDEBUG_EVERY_SECOND() 默认间隔1秒打印一次 SDEBUG_EVERY_SECOND(N)
// 默认间隔N秒打印一次
#define SDEBUG_EVERY_SECOND(cond) LOG_SEC(DEBUG, cond)
#define SINFO_EVERY_SECOND(cond) LOG_SEC(INFO, cond)
#define SWARN_EVERY_SECOND(cond) LOG_SEC(WARNING, cond)
#define SERROR_EVERY_SECOND(cond) LOG_SEC(G3LOG_ERROR, cond)
#define SSYSTEM_EVERY_SECOND(cond) LOG_SEC(G3LOG_SYSTEM, cond)

void AppInit(int argc, char **argv);
void AppShutdown();