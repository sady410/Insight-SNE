#include "menuwindow.h"
#include "appwindow.h"
#include "qjsonarray.h"
#include "ui_menuwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    connect(ui->uploadCsvButton, &QPushButton::clicked, this, &MenuWindow::selectCsvFile);

    ui->perplexityLabel->setText("Perplexity: " + QString::number(15));

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
    ui->iterationsLabel->setText("Iterations: " + QString::number(iterationsMap[3]));

    ui->iterationsSlider->setRange(0, 9);
    ui->iterationsSlider->setTickInterval(1);
    ui->iterationsSlider->setSingleStep(1);
    ui->iterationsSlider->setPageStep(1);
    ui->iterationsSlider->setValue(3);
    connect(ui->iterationsSlider, &QSlider::valueChanged, this, &MenuWindow::updateIterationsLabel);

    ui->runAppButton->setEnabled(false);

    connect(ui->runAppButton, &QPushButton::clicked, this, &MenuWindow::onRunAppButtonClicked);

    // Python Executor Instance
    pythonExecutor = new class PythonExecutor(this);

    connect(pythonExecutor, &PythonExecutor::outputReceived, this, &MenuWindow::onOutputReceived);
    connect(pythonExecutor, &PythonExecutor::finished, this, &MenuWindow::onScriptFinished);
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::updatePerplexityLabel(int value) {
    ui->perplexityLabel->setText("Perplexity: " + QString::number(value));
}

void MenuWindow::updateIterationsLabel(int value) {
    int iterations = iterationsMap[value];
    ui->iterationsLabel->setText("Iterations: " + QString::number(iterations));
}

void MenuWindow::onRunAppButtonClicked()
{
    qDebug() << "Executing Python script...";
    QString projectRoot = QDir(QCoreApplication::applicationDirPath()).absolutePath();
    if (projectRoot.contains("build")) {
        if (QSysInfo::productType() == "windows") {
            projectRoot = QDir(projectRoot).filePath("../../..");
        } else {
            projectRoot = QDir(projectRoot).filePath("../../");
        }
    }

    QStringList args = constructArgsString();
    // qDebug() << args;
    pythonExecutor->executeScript(QDir(projectRoot).filePath("main.py"), args);
}

void MenuWindow::onOutputReceived(const QString &output)
{
    partialOutput += output.trimmed();
    // qDebug() << partialOutput;
    // Try to parse JSON
    if (partialOutput.endsWith("}") || partialOutput.endsWith("]")) {
        QJsonDocument json = QJsonDocument::fromJson(partialOutput.toUtf8());
        if (!json.isNull() && json.isObject()) {
            QJsonObject jsonObject = json.object();

            // Add feature names and labels
            QJsonArray featureNamesArray;
            for (const QString &feature : std::as_const(featureNames)) {
                featureNamesArray.append(feature);
            }
            jsonObject["featureNames"] = featureNamesArray;

            QJsonArray labelsArray;
            for (const QString &label : std::as_const(labels)) {
                labelsArray.append(label);
            }
            jsonObject["labels"] = labelsArray;

            //AppWindow *appWindow = new AppWindow(jsonObject);
            //appWindow->show();
            new AppWindow(jsonObject);
            this->close();

            partialOutput.clear();
        }
    } else {
        qDebug() << "Waiting for full JSON...";
    }
}

void MenuWindow::onScriptFinished(bool success, const QString &error)
{
    if (success) {
        qDebug() << "Python script completed successfully.";
    } else {
        qDebug() << "Error: " + error;
    }
}

void MenuWindow::selectCsvFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (!fileName.isEmpty()) {
        ui->fileLabel->setText("Selected File: " + fileName);
        csvFile = fileName;
        readCsvFile(csvFile);
        if (!data.isEmpty()) {
            ui->runAppButton->setEnabled(true);
        }
    } else {
        ui->fileLabel->setText("No file selected");
        ui->runAppButton->setEnabled(false);
    }
}

// TODO Modify to handle CSV with labels or id etc...
void MenuWindow::readCsvFile(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open the file for reading.";
        return;
    }

    QTextStream in(&file);

    // Read the first line for feature names
    QString firstLine = in.readLine();
    featureNames = firstLine.split(";");
    featureNames.removeLast(); // TODO MAYBE DO IT SMARTER

    // qDebug() << "Feature Names: " << featureNames;

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

    // qDebug() << "Labels : " << labels;
    // qDebug() << "Data (features without labels): " << data;
}

QStringList MenuWindow::constructArgsString() {
    QStringList formattedRows;

    for (const QList<QString>& row : std::as_const(data)) {
        QString formattedRow = "[" + row.join(",") + "]";
        formattedRows.append(formattedRow);
    }

    int perplexity = ui->perplexitySlider->value();

    int iterationsIndex = ui->iterationsSlider->value();
    int iterations = iterationsMap[iterationsIndex];

    QList<QString> args;

    args.append("[" +  formattedRows.join(", ") + "]");
    args.append(QString::number(perplexity));
    args.append(QString::number(iterations));
    return args;
}

QString MenuWindow::list2String(const QList<QString> &list)
{
    return QStringLiteral("{%1}").arg(list.join(','));
}
