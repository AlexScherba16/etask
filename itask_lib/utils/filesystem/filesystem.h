#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <nlohmann/json.hpp>
#include <fstream>

namespace itask::util::filesystem
{
    /**
     * @class TmpJsonFile
     * @brief A temporary file that stores JSON objects.
     *
     * This class creates a temporary file in `/tmp/`, writes the provided JSON objects to it,
     * and automatically deletes the file upon destruction.
     */
    class TmpJsonFile
    {
    public:
        /**
         * @brief Creates a temporary file and writes JSON objects to it.
         *
         * Generates a unique temporary file in `/tmp/` using `mkstemp`, writes each JSON object
         * to a new line in the file, and closes it.
         *
         * @param jsonObjects A vector of JSON strings to be written to the temporary file.
         *
         * @throws If the temporary file cannot be created, opened or JSON object fails to parse.
         */
        explicit TmpJsonFile(std::vector<std::string> jsonObjects)
        {
            char tempFileName[] = "/tmp/jsonXXXXXX";
            int fd = mkstemp(tempFileName);
            if (fd == -1)
            {
                throw std::runtime_error("Failed to create tmp json file");
            }
            path_ = tempFileName;
            outFile_.open(path_);
            if (!outFile_)
            {
                throw std::runtime_error("Failed to open tmp json file");
            }

            for (const auto& jsonString : jsonObjects)
            {
                outFile_ << nlohmann::json::parse(jsonString).dump() << "\n";
            }
            outFile_.close();

            fileSize_= std::filesystem::file_size(path_);
        }

        /**
         * @brief Returns the path of the temporary file.
         * @return A string containing the file path.
         */
        const std::string& path() const
        {
            return path_;
        }

        /**
         * @brief Returns size of the temporary file.
         * @return file size.
         */
        size_t size() const
        {
            return fileSize_;
        }

        /**
         * @brief Destructor that removes the temporary file.
         */
        ~TmpJsonFile()
        {
            std::remove(path_.c_str());
        }

    private:
        std::string path_;
        std::ofstream outFile_;
        uint64_t fileSize_{0};
    };

    /**
     * @class TmpEmptyFile
     * @brief A temporary empty file.
     *
     * This class creates an empty temporary file in `/tmp/`, which can be used as a placeholder.
     * The file is automatically deleted upon destruction.
     */
    class TmpEmptyFile
    {
    public:
        /**
         * @brief Creates an empty temporary file.
         *
         * Generates a unique temporary file in `/tmp/` using `mkstemp`, opens it for writing,
         * and then immediately closes it.
         *
         * @throws If the temporary file cannot be created or opened.
         */
        TmpEmptyFile()
        {
            char tempFileName[] = "/tmp/emptyXXXXXX";
            int fd = mkstemp(tempFileName);
            if (fd == -1)
            {
                throw std::runtime_error("Failed to create tmp empty file");
            }
            path_ = tempFileName;
            outFile_.open(path_);
            if (!outFile_)
            {
                throw std::runtime_error("Failed to open tmp empty file");
            }
            outFile_.close();
        }

        /**
         * @brief Returns the path of the temporary file.
         * @return A string containing the file path.
         */
        const std::string& path() const
        {
            return path_;
        }

        /**
         * @brief Destructor that removes the temporary file.
         */
        ~TmpEmptyFile()
        {
            std::remove(path_.c_str());
        }

    private:
        std::string path_;
        std::ofstream outFile_;
    };
}

#endif //FILESYSTEM_H
