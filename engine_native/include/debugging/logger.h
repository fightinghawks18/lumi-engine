#pragma once

#include <iostream>
#include <format>

namespace lumi::debugging
{
    /* Prints messages to console */
    class Logger
    {
        enum class LogType
        {
            Info,
            Warn,
            Error
        };

    public:
        static Logger& Instance()
        {
            static Logger inst;
            return inst;
        }

        /**
         * \brief Logs a message to the console with the info severity
         * \note Info is the lowest severity
         */
        template<typename ...Args>
        void LogInfo(const std::string& msg, Args&&... args)
        {
            std::string formatted = std::vformat(msg, std::make_format_args(args...));
            Write(LogType::Info, formatted);
        }

        /**
         * \brief Logs a message to the console with the warn severity
         */
        template<typename ...Args>
        void LogWarn(const std::string& msg, Args&&... args)
        {
            std::string formatted = std::vformat(msg, std::make_format_args(args...));
            Write(LogType::Warn, formatted);
        }

        /**
         * \brief Logs a message to the console with the error severity
         * \note Error is the highest severity
         */
        template<typename ...Args>
        void LogError(const std::string& msg, Args&&... args)
        {
            std::string formatted = std::vformat(msg, std::make_format_args(args...));
            Write(LogType::Error, formatted);
        }
    private:
        void Write(const LogType& logType, const std::string& msg)
        {
            // TODO: Add traits to severities
            std::string prefix;
            switch (logType)
            {
                case LogType::Info:
                    prefix = "INFO";
                    break;
                case LogType::Warn:
                    prefix = "WARN";
                    break;
                case LogType::Error:
                    prefix = "ERROR";
                    break;
            }
            std::cout << "[" << prefix << "]: " << msg << std::endl;
        }
    };
}