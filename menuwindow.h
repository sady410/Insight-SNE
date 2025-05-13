#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QList>
#include "pythonexecutor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MenuWindow; }
QT_END_NAMESPACE

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private slots:
    void updatePerplexityLabel(int value);
    void updateIterationsLabel(int value);
    void onRunAppButtonClicked();
    void onOutputReceived(const QString &output);
    void onScriptFinished(bool success, const QString &error);
    void selectCsvFile();

private:
    Ui::MenuWindow *ui;
    PythonExecutor *m_pythonExecutor;

    // CSV data handling
    QString m_csvFile;
    QList<QList<QString>> m_data;
    QList<QString> m_labels;
    QList<QString> m_featureNames;

    // Iteration mapping
    QMap<int, int> m_iterationsMap;

    // JSON data handling
    QString m_partialOutput;

    // UI setup
    void setupUi();
    void applyStylesheet();

    // Helper methods
    QChar detectSeparator(const QString &sampleLine);
    void readCsvFile(const QString &filePath);
    QStringList constructArgsString();
    QString list2String(const QList<QString> &list);
};
#endif // MENUWINDOW_H
