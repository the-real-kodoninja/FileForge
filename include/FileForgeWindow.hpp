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
    QLabel *statusLabel;
    QPushButton *inputButton, *convertButton, *cancelButton;
    QComboBox *formatCombo;
    QProgressBar *progressBar;
    QString inputFilePath, outputDirPath;
    ConversionThread *conversionThread;

    void setupUI();

private slots:
    void selectInputFile();
    void startConversion();
    void cancelConversion();
    void onConversionFinished(bool success, uint64_t newSize, const QString& errorMsg);
    void onProgressUpdated(int value);
};

#endif