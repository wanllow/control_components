/******************************************************************************
 * Copyright 2022  Authors. All Rights Reserved.
 *****************************************************************************/

#include "platform_log.h"

#include <boost/filesystem.hpp>

#include "common/config/config.h"
#include "common/config/config_gflags.h"
#include "gflags/gflags.h"

#ifndef MODULE_NAME
#define MODULE_NAME mfn::common::Binary::GetName().c_str()
#endif

using namespace boost;

std::unique_ptr<LogWorker> logworker{LogWorker::createLogWorker()};
static std::unique_ptr<g3::SinkHandle<LogRotate>> rotate_sink_handle_;
static std::string module_name;
struct ColorCoutSink {
    enum FG_Color {
        YELLOW = 33,
        RED = 31,
        GREEN = 32,
        MAGENTA = 35,
        Cyan = 36,
        WHITE = 97
    };

    FG_Color GetColor(const LEVELS level) const
    {
        if (level.value == WARNING.value) {
            return YELLOW;
        }
        if (level.value == DEBUG.value) {
            return GREEN;
        }
        if (level.value == G3LOG_ERROR.value) {
            return RED;
        }
        if (level.value == G3LOG_SYSTEM.value) {
            return MAGENTA;
        }
        if (g3::internal::wasFatal(level)) {
            return RED;
        }
        return WHITE;
    }

    void ReceiveLogMessage(g3::LogMessageMover logEntry)
    {
        auto level = logEntry.get()._level;
        auto color = GetColor(level);

        if (color == MAGENTA) {
            std::cout << "\033[" << color << "m"
                      << "\033[1m" << logEntry.get().toString() << "\033[m";
        }
        else {
            std::cout << "\033[" << color << "m" << logEntry.get().toString()
                      << "\033[m";
        }
    }
};

void SetDataLogPath(std::string &data_log_path)
{
    mfn::common::config::GetDataLogPath(data_log_path);
}

void AppInit(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    std::cout << "FLAGS_global_enable_log:" << FLAGS_global_enable_log
              << std::endl;
    if (!FLAGS_global_enable_log) {
        return;
    }
    bool enable_save_files = FLAGS_global_enable_log_save_files;
    if (!enable_save_files && !FLAGS_mdc_alsologtostderr) {
        return;
    }

    std::unique_ptr<g3::SinkHandle<FileSink>> sinkHandle;

    if (FLAGS_mdc_alsologtostderr) {
        auto sinkHandle2 =
            logworker->addSink(std::make_unique<ColorCoutSink>(),
                               &ColorCoutSink::ReceiveLogMessage);
    }

    initializeLogging(logworker.get());
    std::string log_path;
    mfn::common::config::GetDataLogPath(log_path);

    filesystem::path dirpath2(log_path);

    if (!boost::filesystem::exists(dirpath2)) {
        std::cout << "cannot find global_log_dir:" << log_path
                  << " and create dir:" << log_path << std::endl;
        boost::filesystem::create_directories(dirpath2);
    }

    filesystem::space_info log_space_info = filesystem::space(dirpath2);

    int64_t bytesToG = 1024 * 1024 * 1024;
    int64_t space_c = log_space_info.capacity / bytesToG;
    int64_t space_f = log_space_info.free / bytesToG;
    int64_t space_a = log_space_info.available / bytesToG;

    filesystem::path abspath = system_complete(dirpath2);

    std::string path_file = abspath.string();

    std::string module_name_a = argv[0];

    filesystem::path module_name_p(module_name_a);
    module_name = filesystem::basename(module_name_p);

    int minloglevel = FLAGS_mdc_minloglevel;
    int slog_v = FLAGS_mdc_v;

    if (FLAGS_use_global_log_level_config) {
        minloglevel = FLAGS_global_minloglevel;
        slog_v = FLAGS_global_slog_v;
    }

    if (minloglevel <= 0) {
        g3::log_levels::setHighest(DEBUG);
    }
    else if (minloglevel == 1) {
        g3::log_levels::setHighest(INFO);
    }
    else if (minloglevel == 2) {
        g3::log_levels::setHighest(WARNING);
    }
    else if (minloglevel == 3) {
        g3::log_levels::setHighest(G3LOG_ERROR);
    }
    else if (minloglevel >= 4) {
        g3::log_levels::setHighest(G3LOG_SYSTEM);
    }

    g3::log_levels::setSquirrelLogLevel(slog_v);
    size_t max_bytes_before_rotating_file = 1024 * 1024 * FLAGS_single_log_size;
    std::string rotate_file_log = module_name;

    rotate_sink_handle_ = logworker->addSink(
        std::make_unique<LogRotate>(rotate_file_log, log_path),
        &LogRotate::save);
    rotate_sink_handle_
        ->call(&LogRotate::setMaxLogSize, max_bytes_before_rotating_file)
        .wait();
    rotate_sink_handle_
        ->call(&LogRotate::setMaxArchiveLogCount, FLAGS_max_archive_log_size)
        .wait();
    // 压缩日志中忽略计数的日志包，默认是1M
    rotate_sink_handle_->call(&LogRotate::SetIgnoreLogSize, 2048).wait();

    // 设置获取日志目录的函数（时间跳变后需要重写日志）
    rotate_sink_handle_->call(&LogRotate::setLogDirectoryHandle, SetDataLogPath)
        .wait();

    // 设置日志软链接目录
    std::string log_link_directory = "../data/log";
    filesystem::path link_path(log_link_directory);
    if (!boost::filesystem::exists(link_path)) {
        SINFO << "cannot find global_log_dir:" << link_path
              << " and create dir:" << link_path;
        boost::filesystem::create_directories(link_path);
    }
    rotate_sink_handle_
        ->call(&LogRotate::setLogSymlinkDirectory, log_link_directory)
        .wait();

    if (space_a > FLAGS_global_log_warning_size) {
        SSYSTEM << "space_info capacity:" << space_c
                << " GB    free:" << space_f << " GB    available:" << space_a
                << " GB    ";
    }
    else if (space_a < FLAGS_global_log_fatal_size) {
        SFATAL << "space_info capacity:" << space_c << " GB    free:" << space_f
               << " GB    available:" << space_a
               << " GB    global_log_fatal_size:"
               << FLAGS_global_log_fatal_size;
    }
    else {
        SWARN << "space_info capacity:" << space_c << " GB    free:" << space_f
              << " GB    available:" << space_a
              << " GB    global_log_warning_size:"
              << FLAGS_global_log_warning_size;
    }
    return;
}