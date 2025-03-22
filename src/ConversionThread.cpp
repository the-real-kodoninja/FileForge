#include "ConversionThread.hpp"

ConversionThread::ConversionThread(const std::string& input, const std::string& output, 
                                   const std::string& format, int compressionLevel, QObject *parent)
    : QThread(parent), inputFile(input), outputFile(output), outputFormat(format), 
      compressionLevel(compressionLevel), cancelled(false) {}

void ConversionThread::run() {
    std::string errorMsg;
    bool success = converter.convertAndCompress(inputFile, outputFile, outputFormat, compressionLevel, 
                                                errorMsg, [this](int value) {
                                                    if (!cancelled) emit progressUpdated(value);
                                                });
    uint64_t newSize = success ? fs::file_size(outputFile) : 0;
    emit conversionFinished(success && !cancelled, newSize, QString::fromStdString(errorMsg));
}

void ConversionThread::cancel() {
    cancelled = true;
}