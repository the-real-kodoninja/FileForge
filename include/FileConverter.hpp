#ifndef FILECONVERTER_HPP
#define FILECONVERTER_HPP

#include <string>
#include <functional>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <Magick++.h>

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
    bool convertPDF(const std::string& input, const std::string& output, 
                    const std::string& format, std::string& errorMsg, 
                    std::function<void(int)> progressCallback);
    bool convertImage(const std::string& input, const std::string& output, 
                      const std::string& format, std::string& errorMsg, 
                      std::function<void(int)> progressCallback);
};

#endif