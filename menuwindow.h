#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "pythonexecutor.h"
#include <QMainWindow>
#include <QFile>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MenuWindow;
}
QT_END_NAMESPACE

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private:
    Ui::MenuWindow *ui;
    QString partialOutput; // to handle terminal output
    PythonExecutor *pythonExecutor;

    // File & Data Handling
    QString csvFile;
    QList<QList<QString>> data;
    QStringList featureNames;
    QStringList labels;

    QMap<int, int> iterationsMap; // Maps slider values to iterations

    // Utility Functions
    void readCsvFile(const QString &filePath);
    QStringList constructArgsString();
    QString list2String(const QList<QString> &list);

private slots:
    void updatePerplexityLabel(int value);
    void updateIterationsLabel(int value);

    void selectCsvFile();

    void onOutputReceived(const QString &output);
    void onScriptFinished(bool success, const QString &error);

    void onRunAppButtonClicked();
};

#endif // MENUWINDOW_H
