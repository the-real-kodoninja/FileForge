// FileConverter.hpp
#ifndef FILECONVERTER_HPP
#define FILECONVERTER_HPP

#include <string>

class FileConverter {
public:
    bool convertAndCompress(const std::string& input, const std::string& output, 
                            const std::string& format, int compressionLevel);
};

#endif