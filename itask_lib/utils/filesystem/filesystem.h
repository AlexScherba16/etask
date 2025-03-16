#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <nlohmann/json.hpp>
#include <fstream>

namespace etask::util::filesystem
{
    class TmpJsonFile
    {
    public:
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
        }

        const std::string& path() const
        {
            return path_;
        }

        ~TmpJsonFile()
        {
            std::remove(path_.c_str());
        }

    private:
        std::string path_;
        std::ofstream outFile_;
    };
}

#endif //FILESYSTEM_H
