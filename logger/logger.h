#ifndef FEL_LOGGER_H
#define FEL_LOGGER_H

// temporary fix since spdlog has compile errors
#include <memory>
#include <string>

namespace fel
{
    struct LoggerImpl;

    struct Logger
    {
        Logger(const std::string& log_file);
        ~Logger();

        std::unique_ptr<LoggerImpl> impl;

        void
        WriteError(const std::string& err);

        void
        WriteInfo(const std::string& inf);
    };
}

#endif  // FEL_LOGGER_H

