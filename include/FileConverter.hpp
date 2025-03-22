#ifndef FILECONVERTER_HPP
#define FILECONVERTER_HPP

#include <string>
#include <functional>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

class FileConverter {
public:
    FileConverter();
    ~FileConverter();
    bool convertAndCompress(const std::string& input, const std::string& output, 
                            const std::string& format, int compressionLevel, 
                            std::string& errorMsg, std::function<void(int)> progressCallback);

private:
    bool compressToArchive(const std::string& input, const std::string& output, 
                           const std::string& format, int compressionLevel, 
                           std::string& errorMsg, std::function<void(int)> progressCallback);
    bool transcodeWithFFmpeg(const std::string& input, const std::string& output, 
                             const std::string& format, std::string& errorMsg, 
                             std::function<void(int)> progressCallback);
};

#endif