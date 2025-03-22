// ConversionThread.cpp
#include "ConversionThread.hpp"

ConversionThread::ConversionThread(const std::string& input, const std::string& output, 
                                   const std::string& format, int compressionLevel, QObject *parent)
    : QThread(parent), inputFile(input), outputFile(output), outputFormat(format), compressionLevel(compressionLevel) {}

void ConversionThread::run() {
    bool success = converter.convertAndCompress(inputFile, outputFile, outputFormat, compressionLevel);
    uint64_t newSize = success ? std::filesystem::file_size(outputFile) : 0;
    emit conversionFinished(success, newSize);
}