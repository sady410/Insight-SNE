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
    this->setFixedSize(380, 540);

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

    // Hide the message label initially
    ui->message->setVisible(false);

    // Python Executor Instance
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

    // Create spacers for better layout
    ui->verticalLayout->addSpacing(10);

    // Style the upload button with an icon
    ui->uploadCsvButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->uploadCsvButton->setIconSize(QSize(16, 16));

    // Style the file label
    ui->fileLabel->setWordWrap(true);
    ui->fileLabel->setMinimumHeight(40);

    // Since we may not have custom icons, let's use Qt's standard icons
    if (!QIcon::hasThemeIcon("check")) {
        // We're setting up a fallback for missing custom icons
        // This approach will work without needing extra resources

        // For dropdown arrow, we'll use a standard icon if our custom one is missing
        QStyle *style = QApplication::style();
        QIcon downArrowIcon = style->standardIcon(QStyle::SP_ArrowDown);

        // For checkbox, we'll still use the CSS styling without an explicit image
        // The background color will indicate the checked state
    }

    // Add tooltips for better UX
    ui->perplexitySlider->setToolTip("Adjust the perplexity parameter for t-SNE (affects clustering)");
    ui->iterationsSlider->setToolTip("Adjust the number of iterations for t-SNE training");
    ui->runAppButton->setToolTip("Process the data and open visualization");

    // Make the run button stand out
    ui->runAppButton->setMinimumHeight(40);
}

void MenuWindow::applyStylesheet()
{
    // Set application style to Fusion
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // Create a palette for the window
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Base, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::ToolTipText, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::Text, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::Button, QColor(60, 60, 60));
    darkPalette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Apply the palette
    qApp->setPalette(darkPalette);

    // Set style sheet for specific widgets
    QString styleSheet = R"(
        QMainWindow {
            background-color: #2d2d2d;
        }

        QLabel#menuTitle {
            color: #4CAF50;
            font-size: 24px;
            font-weight: bold;
            padding: 10px;
        }

        QPushButton#uploadCsvButton {
            background-color: #424242;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px;
            color: white;
        }

        QPushButton#uploadCsvButton:hover {
            background-color: #4CAF50;
            border: 1px solid #388E3C;
        }

        QPushButton#runAppButton {
            background-color: #4CAF50;
            border: none;
            border-radius: 4px;
            padding: 10px;
            font-weight: bold;
            color: white;
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

        QSlider::sub-page:horizontal:disabled {
            background: #555555;
        }

        QSlider::add-page:horizontal:disabled {
            background: #333333;
        }

        QSlider::handle:horizontal:disabled {
            background: #555555;
        }

        QCheckBox {
            spacing: 8px;
            min-height: 25px;
            font-size: 10pt;
        }

        QCheckBox::indicator {
            width: 20px;
            height: 20px;
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

        QCheckBox::indicator:checked:hover {
            background-color: #66BB6A;
            border: 2px solid #66BB6A;
        }

        QComboBox {
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 5px 18px 5px 10px;
            min-width: 6em;
            background-color: #424242;
            min-height: 25px;
            font-size: 10pt;
        }

        QComboBox:hover {
            border: 1px solid #4CAF50;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left-width: 1px;
            border-left-color: #555555;
            border-left-style: solid;
        }

        QComboBox::down-arrow {
            width: 12px;
            height: 12px;
        }

        QComboBox QAbstractItemView {
            background-color: #424242;
            border: 1px solid #555555;
            selection-background-color: #4CAF50;
            selection-color: white;
        }

        QLabel#fileLabel {
            background-color: #424242;
            border-radius: 3px;
            padding: 5px;
        }
    )";

    this->setStyleSheet(styleSheet);
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

// TODO Modify to handle CSV with labels or id etc...
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

    // Read the first line for feature names
    QString firstLine = in.readLine();
    m_featureNames = firstLine.split(";");
    m_featureNames.removeLast(); // TODO MAYBE DO IT SMARTER

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
