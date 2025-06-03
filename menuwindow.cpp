#include "menuwindow.h"
#include "appwindowbuilder.h"
#include "ui_menuwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QGraphicsDropShadowEffect>
#include <QScreen>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    // Set application style
    applyStylesheet();
    setupUi();

    // Center the window on the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // Set window title and fixed size
    this->setWindowTitle("Insight-SNE");
    this->setFixedSize(400, 640);

    connect(ui->uploadCsvButton, &QPushButton::clicked, this, &MenuWindow::selectCsvFile);

    // Perplexity slider
    ui->perplexityLabel->setText("Perplexity: " + QString::number(15));

    ui->perplexitySlider->setRange(1, 50);
    ui->perplexitySlider->setTickInterval(1);
    ui->perplexitySlider->setSingleStep(1);
    ui->perplexitySlider->setPageStep(1);
    ui->perplexitySlider->setValue(15);
    connect(ui->perplexitySlider, &QSlider::valueChanged, this, &MenuWindow::updatePerplexityLabel);

    // Iteration slider
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

    // Parameters combobox
    ui->visualizationTypeComboBox->addItem("Class");
    ui->visualizationTypeComboBox->addItem("Unique");
    ui->visualizationTypeComboBox->setCurrentIndex(0); // Default to "Class"
    ui->visualizationTypeComboBox->setToolTip("Select visualization type: Class (group by labels) or Unique (individual points)");
    ui->visualizationTypeComboBox->setEnabled(ui->checkBox->isChecked());
    connect(ui->checkBox, &QCheckBox::toggled, ui->visualizationTypeComboBox, &QComboBox::setEnabled);

    // Run t-SNE and explanations
    ui->runAppButton->setEnabled(false);
    connect(ui->runAppButton, &QPushButton::clicked, this, &MenuWindow::onRunAppButtonClicked);

    // Hide the message label
    ui->message->setVisible(false);

    // Instanciate python executor
    m_pythonExecutor = new class PythonExecutor(this);
    connect(m_pythonExecutor, &PythonExecutor::outputReceived, this, &MenuWindow::onOutputReceived);
    connect(m_pythonExecutor, &PythonExecutor::finished, this, &MenuWindow::onScriptFinished);
}

void MenuWindow::setupUi()
{
    // Apply drop shadow to the main title
    QGraphicsDropShadowEffect *titleShadow = new QGraphicsDropShadowEffect;
    titleShadow->setBlurRadius(10);
    titleShadow->setColor(QColor(0, 0, 0, 100));
    titleShadow->setOffset(0, 1);
    ui->menuTitle->setGraphicsEffect(titleShadow);

    // Create spacers
    ui->verticalLayout->addSpacing(10);

    // Style the upload button with an icon
    ui->uploadCsvButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->uploadCsvButton->setIconSize(QSize(16, 16));

    // Style the file label
    ui->fileLabel->setWordWrap(true);
    ui->fileLabel->setMinimumHeight(40);

    // Connect checkbox state change to enable/disable combobox
    connect(ui->checkBox, &QCheckBox::checkStateChanged, this, [this](int state) {
        ui->visualizationTypeComboBox->setEnabled(state == Qt::Checked);
    });

    // Set initial state of combobox based on checkbox
    ui->visualizationTypeComboBox->setEnabled(ui->checkBox->isChecked());

    // Add tooltips for better UX
    ui->perplexitySlider->setToolTip("Adjust the perplexity parameter for t-SNE (affects clustering)");
    ui->iterationsSlider->setToolTip("Adjust the number of iterations for t-SNE training");
    ui->runAppButton->setToolTip("Process the data and open visualization");
    ui->standardizationCheckBox->setToolTip("Standardize data before t-SNE (recommended for features with different scales)");

    // Make the run button stand out
    ui->runAppButton->setMinimumHeight(40);
}

void MenuWindow::applyStylesheet()
{
    // Set application style to Fusion
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // Create a dark palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(29, 29, 29));       // #1d1d1d
    darkPalette.setColor(QPalette::WindowText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Base, QColor(36, 36, 36));         // #242424
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::ToolTipText, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::Text, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Button, QColor(54, 54, 54));       // #363636
    darkPalette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(76, 175, 80));        // #4CAF50
    darkPalette.setColor(QPalette::Highlight, QColor(76, 175, 80));   // #4CAF50
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Apply the palette
    qApp->setPalette(darkPalette);

    // Set style sheet
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1d1d1d;
        }

        QLabel#menuTitle {
            color: #4CAF50;
            font-size: 24px;
            font-weight: bold;
            padding: 10px;
            background-color: #333333;
            border-radius: 4px;
        }

        QPushButton {
            background-color: #363636;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px;
            color: white;
            min-height: 28px;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #4CAF50;
            border: 1px solid #388E3C;
        }

        QPushButton#runAppButton {
            background-color: #4CAF50;
            border: none;
            font-weight: bold;
        }

        QPushButton#runAppButton:hover {
            background-color: #66BB6A;
        }

        QPushButton#runAppButton:disabled {
            background-color: #424242;
            color: #9E9E9E;
        }

        QSlider::groove:horizontal {
            height: 10px;
            background: #424242;
            border-radius: 5px;
        }

        QSlider::handle:horizontal {
            background: #4CAF50;
            width: 20px;
            height: 20px;
            margin: -5px 0;
            border-radius: 10px;
        }

        QSlider::handle:horizontal:hover {
            background: #66BB6A;
        }

        QSlider::sub-page:horizontal {
            background: #2E7D32;
            border-radius: 5px;
        }

        QSlider::add-page:horizontal {
            background: #424242;
            border-radius: 5px;
        }

        QCheckBox {
            spacing: 8px;
            color: #e6e6e6;
            font-size: 14px;
        }

        QCheckBox::indicator {
            width: 18px;
            height: 18px;
        }

        QCheckBox::indicator:unchecked {
            border: 2px solid #555555;
            background-color: #424242;
            border-radius: 3px;
        }

        QCheckBox::indicator:unchecked:hover {
            border: 2px solid #4CAF50;
        }

        QCheckBox::indicator:checked {
            border: 2px solid #4CAF50;
            background-color: #4CAF50;
            border-radius: 3px;
        }

        QComboBox {
            border: 1px solid #555555;
            border-radius: 5px;
            padding: 6px 20px 6px 12px;
            background-color: #424242;
            color: #e6e6e6;
            font-size: 14px;
            min-height: 28px;
        }

        QComboBox:hover {
            border: 1px solid #4CAF50;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 22px;
            border-left-width: 1px;
            border-left-color: #555555;
            border-left-style: solid;
        }

        QComboBox::down-arrow {
            image: url('data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2"><path d="M6 9l6 6 6-6"/></svg>');
            width: 12px;
            height: 12px;
        }

        QComboBox QAbstractItemView {
            background-color: #424242;
            border: 1px solid #555555;
            selection-background-color: #4CAF50;
            selection-color: white;
        }

        QComboBox:disabled {
            color: #777777;
            background-color: #2d2d2d;
            border: 1px solid #444444;
        }

        QLabel#fileLabel {
            background-color: #363636;
            border-radius: 5px;
            padding: 8px;
            border: 1px solid #555555;
            font-size: 14px;
        }

        QLabel#message {
            color: #e6e6e6;
            font-size: 14px;
            padding: 8px;
            background-color: #363636;
            border-radius: 5px;
            border: 1px solid #555555;
        }
    )";

    this->setStyleSheet(styleSheet);

    // Add shadow effect
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(15);
    windowShadow->setColor(QColor(0, 0, 0, 120));
    windowShadow->setOffset(0, 2);
    this->setGraphicsEffect(windowShadow);
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
    ui->message->setVisible(true);
    ui->message->setText("Processing data...");
    ui->runAppButton->setEnabled(false);

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
    m_pythonExecutor->executeScript(QDir(projectRoot).filePath("main.py"), args);
}

void MenuWindow::onOutputReceived(const QString &output)
{
    m_partialOutput += output.trimmed();
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

            // Add perplexity and iterations values to JSON
            jsonObject["perplexity"] = ui->perplexitySlider->value();
            jsonObject["iterations"] = m_iterationsMap[ui->iterationsSlider->value()];

            // Add checkbox and combobox values
            jsonObject["showLabels"] = ui->checkBox->isChecked();
            jsonObject["isClass"] = true ? ui->visualizationTypeComboBox->currentText() == "Class" : false;

            // Add filename without .csv extension
            QFileInfo fileInfo(m_csvFile);
            QString filenameWithoutExtension = fileInfo.baseName();
            jsonObject["filename"] = filenameWithoutExtension;

            // Add data to JSON
            QJsonArray dataArray;
            for (const QList<QString> &row : std::as_const(m_data)) {
                QJsonArray rowArray;
                for (const QString &value : row) {
                    // Convert QString to qreal
                    bool conversionOk;
                    qreal numericValue = value.toDouble(&conversionOk);
                    if (conversionOk) {
                        rowArray.append(numericValue);
                    } else {
                        // Handle conversion error by appending a default value
                        rowArray.append(0.0);
                    }
                }
                dataArray.append(rowArray);
            }
            jsonObject["data"] = dataArray;

            jsonObject["standardize"] = ui->standardizationCheckBox->isChecked();

            AppWindowBuilder builder;
            builder.setJsonData(jsonObject)  // This also parses the JSON data
                .initializeTsnePlot()
                .initializeOverviewPlot()
                .initializeFeaturesPlot()
                .initializeExplanationPlots()
                .initializeMenuForm()
                .initializeFeaturesForm()
                .initializeDistributionPlot()
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
        ui->message->setText("Error: " + error);
        ui->message->setStyleSheet("color: #FF5252;");
        ui->message->setVisible(true);
        ui->runAppButton->setEnabled(true);
    }
}

void MenuWindow::selectCsvFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (!fileName.isEmpty()) {
        // Extract just the filename for display
        QFileInfo fileInfo(fileName);
        ui->fileLabel->setText(fileInfo.fileName());
        ui->fileLabel->setToolTip(fileName); // Show full path on hover

        m_csvFile = fileName;
        readCsvFile(m_csvFile);
        if (!m_data.isEmpty()) {
            ui->runAppButton->setEnabled(true);
            ui->message->setText("File loaded successfully!");
            ui->message->setStyleSheet("color: #4CAF50;");
            ui->message->setVisible(true);
        }
    } else {
        ui->fileLabel->setText("No file selected");
        ui->runAppButton->setEnabled(false);
    }
}

void MenuWindow::readCsvFile(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open the file for reading.";
        ui->message->setText("Error: Could not open the file.");
        ui->message->setStyleSheet("color: #FF5252;");
        ui->message->setVisible(true);
        return;
    }

    // Clear previous data
    m_data.clear();
    m_labels.clear();
    m_featureNames.clear();

    QTextStream in(&file);

    // Read first line to detect separator
    QString firstLine = in.readLine();
    QChar separator = detectSeparator(firstLine);

    if (separator == QChar()) {
        qDebug() << "Error: Could not detect CSV separator.";
        ui->message->setText("Error: Invalid CSV format.");
        ui->message->setStyleSheet("color: #FF5252;");
        ui->message->setVisible(true);
        return;
    }

    // Read feature names
    m_featureNames = firstLine.split(separator);
    if (m_featureNames.isEmpty()) {
        qDebug() << "Error: No columns found in CSV file.";
        ui->message->setText("Error: Empty CSV file.");
        ui->message->setStyleSheet("color: #FF5252;");
        ui->message->setVisible(true);
        return;
    }
    m_featureNames.removeLast(); // Remove label column

    // Read the rest of the lines
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue; // Skip empty lines

        QStringList fields = line.split(separator);
        if (fields.size() < 2) continue; // Skip lines with insufficient data

        QList<QString> features;
        QString label;

        // All columns except the last one are features
        for (int i = 0; i < fields.size() - 1; ++i) {
            features.append(fields[i].trimmed()); // Trim whitespace
        }

        // The last column is the label
        label = fields.last().trimmed();

        // Store the features and the label
        m_data.append(features);
        m_labels.append(label);
    }

    file.close();
}

QChar MenuWindow::detectSeparator(const QString &sampleLine) {
    // Count occurrences of potential separators
    int commaCount = sampleLine.count(',');
    int semicolonCount = sampleLine.count(';');
    int tabCount = sampleLine.count('\t');

    // Determine which separator is most likely used
    if (commaCount > 0 && commaCount >= semicolonCount && commaCount >= tabCount) {
        return ',';
    } else if (semicolonCount > 0 && semicolonCount >= commaCount && semicolonCount >= tabCount) {
        return ';';
    } else if (tabCount > 0 && tabCount >= commaCount && tabCount >= semicolonCount) {
        return '\t';
    }

    return QChar(); // No separator detected
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
    args.append(ui->standardizationCheckBox->isChecked() ? "1" : "0"); // Add standardization flag
    return args;
}

QString MenuWindow::list2String(const QList<QString> &list)
{
    return QStringLiteral("{%1}").arg(list.join(','));
}
