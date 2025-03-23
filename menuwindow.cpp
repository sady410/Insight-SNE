#include "menuwindow.h"
#include "appwindowbuilder.h"
#include "ui_menuwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    m_iterationsMap = {
        {0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500},
        {5, 600}, {6, 700}, {7, 800}, {8, 900}, {9, 1000}
    };
    ui->iterationsLabel->setText("Iterations: " + QString::number(m_iterationsMap[3]));

    ui->iterationsSlider->setRange(0, 9);
    ui->iterationsSlider->setTickInterval(1);
    ui->iterationsSlider->setSingleStep(1);
    ui->iterationsSlider->setPageStep(1);
    ui->iterationsSlider->setValue(3);
    connect(ui->iterationsSlider, &QSlider::valueChanged, this, &MenuWindow::updateIterationsLabel);

    ui->runAppButton->setEnabled(false);

    connect(ui->runAppButton, &QPushButton::clicked, this, &MenuWindow::onRunAppButtonClicked);

    // Python Executor Instance
    m_pythonExecutor = new class PythonExecutor(this);

    connect(m_pythonExecutor, &PythonExecutor::outputReceived, this, &MenuWindow::onOutputReceived);
    connect(m_pythonExecutor, &PythonExecutor::finished, this, &MenuWindow::onScriptFinished);
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::updatePerplexityLabel(int value) {
    ui->perplexityLabel->setText("Perplexity: " + QString::number(value));
}

void MenuWindow::updateIterationsLabel(int value) {
    int iterations = m_iterationsMap[value];
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
    m_pythonExecutor->executeScript(QDir(projectRoot).filePath("main.py"), args);
}

void MenuWindow::onOutputReceived(const QString &output)
{
    m_partialOutput += output.trimmed();
    // qDebug() << partialOutput;
    // Try to parse JSON
    if (m_partialOutput.endsWith("}") || m_partialOutput.endsWith("]")) {
        QJsonDocument json = QJsonDocument::fromJson(m_partialOutput.toUtf8());
        if (!json.isNull() && json.isObject()) {
            QJsonObject jsonObject = json.object();

            // Add feature names and labels
            QJsonArray featureNamesArray;
            for (const QString &feature : std::as_const(m_featureNames)) {
                featureNamesArray.append(feature);
            }
            jsonObject["featureNames"] = featureNamesArray;

            QJsonArray labelsArray;
            for (const QString &label : std::as_const(m_labels)) {
                labelsArray.append(label);
            }
            jsonObject["labels"] = labelsArray;

            // Add data to JSON
            QJsonArray dataArray;
            for (const QList<QString> &row : std::as_const(m_data)) {
                QJsonArray rowArray;
                for (const QString &value : row) {
                    // Convert QString to qreal (double)
                    bool conversionOk;
                    qreal numericValue = value.toDouble(&conversionOk);
                    if (conversionOk) {
                        rowArray.append(numericValue);
                    } else {
                        // Handle conversion error, perhaps by appending a default value or skipping
                        rowArray.append(0.0); // or handle differently
                    }
                }
                dataArray.append(rowArray);
            }
            jsonObject["data"] = dataArray;

            AppWindowBuilder builder(jsonObject);
            builder.initializeTsnePlot()
                .initializeOverviewPlot()
                .initializeFeaturesPlot()
                .initializeExplanationPlots()
                .build();

            this->close();
            m_partialOutput.clear();
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
        m_csvFile = fileName;
        readCsvFile(m_csvFile);
        if (!m_data.isEmpty()) {
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
    m_featureNames = firstLine.split(";");
    m_featureNames.removeLast(); // TODO MAYBE DO IT SMARTER

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
        m_data.append(features);
        m_labels.append(label);
    }

    file.close();

    // qDebug() << "Labels : " << labels;
    // qDebug() << "Data (features without labels): " << data;
}

QStringList MenuWindow::constructArgsString() {
    QStringList formattedRows;

    for (const QList<QString>& row : std::as_const(m_data)) {
        QString formattedRow = "[" + row.join(",") + "]";
        formattedRows.append(formattedRow);
    }

    int perplexity = ui->perplexitySlider->value();

    int iterationsIndex = ui->iterationsSlider->value();
    int iterations = m_iterationsMap[iterationsIndex];

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
