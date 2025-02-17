#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "pythonexecutor.h"

#include <QMainWindow>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class MenuWindow;
}
QT_END_NAMESPACE

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private:
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

private:
    Ui::MenuWindow *ui;

    QMap<int, int> iterationsMap;
    QString csvFile;

    QList<QList<QString>> data;
    QStringList featureNames;
    QStringList labels;

    PythonExecutor *PythonExecutor;
};
#endif // MENUWINDOW_H
