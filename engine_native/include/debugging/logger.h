#pragma once

#include <iostream>
#include <format>

#ifdef _WIN32
    #include <windows.h>
    #include <comdef.h>
#endif

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

        #ifdef _WIN32
        /**
         * \brief Checks if a HRESULT failed and logs an error if failed
         * 
         * \param hr The HRESULT to check
         * \param msg Message to output if HRESULT failed
         * \return true HRESULT failed and a error message was thrown
         * \return false HRESULT succeeded 
         */
        template<typename ...Args>
        bool LogIfHRESULTFailure(HRESULT hr, const std::string& msg, Args&&... args)
        {
            if (FAILED(hr))
            {
                _com_error err(hr);
                LogError(
                    "{} (HRESULT: 0x{:08X}): {}", 
                    msg, static_cast<unsigned int>(hr), 
                    err.ErrorMessage()
                );
                return true;
            }
            return false;
        }
        #endif

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