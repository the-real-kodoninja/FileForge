// FileForgeWindow.hpp
#ifndef FILEFORGEWINDOW_HPP
#define FILEFORGEWINDOW_HPP

#include <QtWidgets>

class ConversionThread;

class FileForgeWindow : public QWidget {
    Q_OBJECT
public:
    FileForgeWindow(QWidget *parent = nullptr);
    ~FileForgeWindow();

private:
    QLabel *inputFileLabel, *fileInfoLabel, *statusLabel;
    QComboBox *outputFormatCombo, *compressionLevelCombo;
    QPushButton *inputFileButton, *outputDirButton, *convertButton;
    QString inputFilePath, outputDirPath;
    ConversionThread *conversionThread;

    void setupUI();

private slots:
    void selectInputFile();
    void selectOutputDir();
    void startConversion();
    void onConversionFinished(bool success, uint64_t newSize);
};

#endif