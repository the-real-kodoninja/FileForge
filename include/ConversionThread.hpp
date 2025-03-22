// ConversionThread.hpp
#ifndef CONVERSIONTHREAD_HPP
#define CONVERSIONTHREAD_HPP

#include <QThread>
#include "FileConverter.hpp"

class ConversionThread : public QThread {
    Q_OBJECT
public:
    ConversionThread(const std::string& input, const std::string& output, 
                     const std::string& format, int compressionLevel, QObject *parent = nullptr);
    void run() override;

signals:
    void conversionFinished(bool success, uint64_t newSize);

private:
    std::string inputFile, outputFile, outputFormat;
    int compressionLevel;
    FileConverter converter;
};

#endif