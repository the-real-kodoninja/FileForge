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
    setFixedSize(400, 150);
    setStyleSheet(
        "QWidget { background-color: #2E2E2E; color: #D4A017; }" // Dark gray bg, earthy yellow text
        "QPushButton { background-color: #4A4A4A; border: none; padding: 5px; color: #E8DAB2; }" // Medium gray buttons
        "QPushButton:hover { background-color: #5A5A5A; }" // Lighter gray on hover
        "QComboBox { background-color: #4A4A4A; border: none; padding: 5px; color: #E8DAB2; }" // Earthy beige text
        "QProgressBar { background-color: #4A4A4A; border: none; color: #D4A017; }"
        "QProgressBar::chunk { background-color: #8B5A2B; }" // Earthy brown progress
        "QLabel { color: #E8DAB2; }" // Beige text
    );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    // Input and format row
    QHBoxLayout *topLayout = new QHBoxLayout();
    inputButton = new QPushButton("Select File");
    formatCombo = new QComboBox();
    formatCombo->setEditable(true);
    formatCombo->addItems({" .txt", ".zip", ".tar.gz", ".mp3", ".mp4"});
    topLayout->addWidget(inputButton);
    topLayout->addWidget(formatCombo);

    // Progress and status
    progressBar = new QProgressBar();
    progressBar->setTextVisible(false);
    statusLabel = new QLabel("Idle");

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    convertButton = new QPushButton("Convert");
    cancelButton = new QPushButton("Cancel");
    cancelButton->setEnabled(false);
    buttonLayout->addStretch();
    buttonLayout->addWidget(convertButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    layout->addLayout(topLayout);
    layout->addWidget(progressBar);
    layout->addWidget(statusLabel, 0, Qt::AlignCenter);
    layout->addLayout(buttonLayout);

    connect(inputButton, &QPushButton::clicked, this, &FileForgeWindow::selectInputFile);
    connect(convertButton, &QPushButton::clicked, this, &FileForgeWindow::startConversion);
    connect(cancelButton, &QPushButton::clicked, this, &FileForgeWindow::cancelConversion);
}

void FileForgeWindow::selectInputFile() {
    inputFilePath = QFileDialog::getOpenFileName(this, "Select File");
    if (!inputFilePath.isEmpty()) {
        outputDirPath = QFileInfo(inputFilePath).absolutePath();
        statusLabel->setText(QFileInfo(inputFilePath).fileName());
    }
}

void FileForgeWindow::startConversion() {
    if (inputFilePath.isEmpty() || formatCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Select a file and format.");
        return;
    }

    QString outputFile = outputDirPath + "/output" + formatCombo->currentText().trimmed();
    statusLabel->setText("Processing...");
    progressBar->setValue(0);
    convertButton->setEnabled(false);
    cancelButton->setEnabled(true);

    if (conversionThread) delete conversionThread;
    conversionThread = new ConversionThread(inputFilePath.toStdString(), outputFile.toStdString(),
                                            formatCombo->currentText().trimmed().toStdString(), 2, this); // Medium compression
    connect(conversionThread, &ConversionThread::conversionFinished, this, &FileForgeWindow::onConversionFinished);
    connect(conversionThread, &ConversionThread::progressUpdated, this, &FileForgeWindow::onProgressUpdated);
    conversionThread->start();
}

void FileForgeWindow::cancelConversion() {
    if (conversionThread) {
        conversionThread->cancel();
        statusLabel->setText("Cancelled");
        progressBar->setValue(0);
        convertButton->setEnabled(true);
        cancelButton->setEnabled(false);
    }
}

void FileForgeWindow::onConversionFinished(bool success, uint64_t newSize, const QString& errorMsg) {
    if (success) {
        statusLabel->setText(QString("Done: %1 bytes").arg(newSize));
    } else {
        statusLabel->setText("Failed");
        QMessageBox::critical(this, "Error", errorMsg);
    }
    convertButton->setEnabled(true);
    cancelButton->setEnabled(false);
}

void FileForgeWindow::onProgressUpdated(int value) {
    progressBar->setValue(value);
}