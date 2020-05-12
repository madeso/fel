#include "logger/logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"


namespace fel
{
    struct LoggerImpl
    {
        static constexpr std::size_t max_size = 1048576 * 5;
        static constexpr std::size_t max_files = 3;

        std::shared_ptr<spdlog::logger> logger;

        LoggerImpl(const std::string& log_file)
            : logger
            (
                spdlog::rotating_logger_mt
                (
                    "fel-lsp",
                    log_file,
                    max_size,
                    max_files
                )
            )
        {
            spdlog::set_default_logger(logger);
            spdlog::flush_every(std::chrono::seconds(3));
        }
    };


    Logger::Logger(const std::string& log_file)
        : impl(new LoggerImpl(log_file))
    {
    }


    Logger::~Logger()
    {
    }


    void
    Logger::WriteError(const std::string& err)
    {
        SPDLOG_ERROR("{}", err);
    }


    void
    Logger::WriteInfo(const std::string& inf)
    {
        SPDLOG_INFO("{}", inf);
    }
}



