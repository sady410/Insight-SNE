#include "menuwindow.h"
#include "appwindow.h"
#include "ui_menuwindow.h"

#include <QDebug>
#include <QFileDialog>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    connect(ui->uploadCsvButton, &QPushButton::clicked, this, &MenuWindow::selectCsvFile);

    ui->perplexitySlider->setRange(1, 50);
    ui->perplexitySlider->setTickInterval(1);
    ui->perplexitySlider->setSingleStep(1);
    ui->perplexitySlider->setPageStep(1);
    ui->perplexitySlider->setValue(15);
    connect(ui->perplexitySlider, &QSlider::valueChanged, this, &MenuWindow::updatePerplexityLabel);

    iterationsMap = {
        {0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500},
        {5, 600}, {6, 700}, {7, 800}, {8, 900}, {9, 1000}
    };
    ui->iterationsSlider->setRange(0, 9);
    ui->iterationsSlider->setTickInterval(1);
    ui->iterationsSlider->setSingleStep(1);
    ui->iterationsSlider->setPageStep(1);
    ui->iterationsSlider->setValue(3);
    connect(ui->iterationsSlider, &QSlider::valueChanged, this, &MenuWindow::updateIterationsLabel);

    connect(ui->runAppButton, &QPushButton::clicked, this, &MenuWindow::onRunAppButtonClicked);

    // Python Executor Instance
    PythonExecutor = new class PythonExecutor(this);

    connect(PythonExecutor, &PythonExecutor::outputReceived, this, &MenuWindow::onOutputReceived);
    connect(PythonExecutor, &PythonExecutor::finished, this, &MenuWindow::onScriptFinished);
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::updatePerplexityLabel(int value) {
    ui->perplexityLabel->setText("Perplexity: " + QString::number(value));
    qDebug() << "Perplexity: " << value;
}

void MenuWindow::updateIterationsLabel(int value) {
    int iterations = iterationsMap[value];
    ui->iterationsLabel->setText("Iterations: " + QString::number(iterations));
    qDebug() << "Iterations: " << iterations;
}

void MenuWindow::onRunAppButtonClicked()
{
    // Get settings values
    int perplexity = ui->perplexitySlider->value();

    int iterationsIndex = ui->iterationsSlider->value();
    int iterations = iterationsMap[iterationsIndex];

    if (csvFile != nullptr) {
        readCsvFile(csvFile);
    } else {
        qDebug() << "No CSV file selected!";
    }


    qInfo() << "Perplexity:" << perplexity;
    qInfo() << "Iterations:" << iterations;

    // Execute python script
    qDebug() << "Executing Python script...";
    QString projectRoot = QDir(QCoreApplication::applicationDirPath()).absolutePath();
    if (projectRoot.contains("build")) {
        projectRoot = QDir(projectRoot).filePath("../../..");
    }

    QStringList args = constructArgsString(); // TODO
    PythonExecutor->executeScript(QDir(projectRoot).filePath("test.py"), args);
}

void MenuWindow::onOutputReceived(const QString &output)
{
    qDebug() << "Python Output: " + output;
}

void MenuWindow::onScriptFinished(bool success, const QString &error)
{
    if (success) {
        qDebug() << "Python script completed successfully.";
    } else {
        qDebug() << "Error: " + error;
    }
}

void MenuWindow::openAppWindow() {
    AppWindow *appWindow = new AppWindow();
    appWindow->show();
    this->close();
}

void MenuWindow::selectCsvFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (!fileName.isEmpty()) {
        ui->fileLabel->setText("Selected File: " + fileName);
        csvFile = fileName;
        qDebug() << "Selected CSV file: " << fileName;
    } else {
        ui->fileLabel->setText("No file selected");
    }
}

void MenuWindow::readCsvFile(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open the file for reading.";
        return;
    }

    QTextStream in(&file);

    // Read the first line for feature names
    QString firstLine = in.readLine();
    QStringList featureNames = firstLine.split(";");
    qDebug() << "Feature Names: " << featureNames;

    QList<QList<QString>> data;
    QStringList labels;

    // Read the rest of the lines
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(";");

        QList<QString> features;
        QString label;

        // All columns except the last one are features
        for (int i = 0; i < fields.size() - 1; ++i) {
            features.append(fields[i]);
        }

        // The last column is the label
        label = fields.last();

        // Store the features and the label
        data.append(features);
        labels.append(label);
    }

    file.close();

    // TODO: store featureNames and data globally for further use
    qDebug() << "Labels : " << labels;
    qDebug() << "Data (features without labels): " << data;
}

QStringList MenuWindow::constructArgsString() {
    return QStringList("TODO");
}


