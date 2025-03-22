// FileForgeWindow.cpp
#include "FileForgeWindow.hpp"
#include "ConversionThread.hpp"
#include <filesystem>

namespace fs = std::filesystem;

FileForgeWindow::FileForgeWindow(QWidget *parent) : QWidget(parent), conversionThread(nullptr) {
    setupUI();
}

FileForgeWindow::~FileForgeWindow() {
    if (conversionThread) delete conversionThread;
}

void FileForgeWindow::setupUI() {
    setWindowTitle("FileForge");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Input file
    inputFileButton = new QPushButton("Select Input File");
    inputFileLabel = new QLabel("No file selected");
    fileInfoLabel = new QLabel("Type: N/A | Size: N/A");
    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->addWidget(inputFileButton);
    fileLayout->addWidget(inputFileLabel);
    fileLayout->addWidget(fileInfoLabel);

    // Output format and location
    outputFormatCombo = new QComboBox();
    outputFormatCombo->setEditable(true);
    outputFormatCombo->addItems({" .txt", ".zip", ".pdf", ".mp3", ".mp4"});
    outputDirButton = new QPushButton("Select Output Directory");
    compressionLevelCombo = new QComboBox();
    compressionLevelCombo->addItems({"Low (1)", "Medium (2)", "High (3)"});
    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(new QLabel("Output Format:"));
    outputLayout->addWidget(outputFormatCombo);
    outputLayout->addWidget(outputDirButton);
    outputLayout->addWidget(new QLabel("Compression:"));
    outputLayout->addWidget(compressionLevelCombo);

    // Convert and status
    convertButton = new QPushButton("Convert");
    statusLabel = new QLabel("Status: Idle");
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(convertButton);
    buttonLayout->addWidget(statusLabel);

    mainLayout->addLayout(fileLayout);
    mainLayout->addLayout(outputLayout);
    mainLayout->addLayout(buttonLayout);

    connect(inputFileButton, &QPushButton::clicked, this, &FileForgeWindow::selectInputFile);
    connect(outputDirButton, &QPushButton::clicked, this, &FileForgeWindow::selectOutputDir);
    connect(convertButton, &QPushButton::clicked, this, &FileForgeWindow::startConversion);
}

void FileForgeWindow::selectInputFile() {
    inputFilePath = QFileDialog::getOpenFileName(this, "Select Input File");
    if (!inputFilePath.isEmpty()) {
        inputFileLabel->setText(QFileInfo(inputFilePath).fileName());
        uint64_t size = fs::file_size(inputFilePath.toStdString());
        QString type = QFileInfo(inputFilePath).suffix().isEmpty() ? "Unknown" : "." + QFileInfo(inputFilePath).suffix();
        fileInfoLabel->setText(QString("Type: %1 | Size: %2 bytes").arg(type).arg(size));
    }
}

void FileForgeWindow::selectOutputDir() {
    outputDirPath = QFileDialog::getExistingDirectory(this, "Select Output Directory");
    if (!outputDirPath.isEmpty()) {
        outputDirButton->setText("Output: " + QDir(outputDirPath).dirName());
    }
}

void FileForgeWindow::startConversion() {
    if (inputFilePath.isEmpty() || outputDirPath.isEmpty() || outputFormatCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    QString outputFile = outputDirPath + "/output" + outputFormatCombo->currentText().trimmed();
    int compressionLevel = compressionLevelCombo->currentIndex() + 1;

    statusLabel->setText("Status: Processing...");
    convertButton->setEnabled(false);

    if (conversionThread) delete conversionThread;
    conversionThread = new ConversionThread(inputFilePath.toStdString(), outputFile.toStdString(),
                                            outputFormatCombo->currentText().trimmed().toStdString(), compressionLevel, this);
    connect(conversionThread, &ConversionThread::conversionFinished, this, &FileForgeWindow::onConversionFinished);
    conversionThread->start();
}

void FileForgeWindow::onConversionFinished(bool success, uint64_t newSize) {
    if (success) {
        statusLabel->setText(QString("Status: Done! New Size: %1 bytes").arg(newSize));
    } else {
        statusLabel->setText("Status: Conversion failed.");
    }
    convertButton->setEnabled(true);
}