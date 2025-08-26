#pragma once

#include <filesystem>
#include <debugging/logger.h>

namespace lumi::sys
{
    /* High level access to filesystem operations */
    class FileSystem
    {
    public:
        static FileSystem& Instance()
        {
            static FileSystem inst;
            return inst;
        }

        /**
         * \brief Checks if the given path exists
         * 
         * \return true The path exists.
         * \return false The path doesn't exist.
         */
        bool Exists(const std::string& path)
        {
            return std::filesystem::exists(path);
        }
    };
}