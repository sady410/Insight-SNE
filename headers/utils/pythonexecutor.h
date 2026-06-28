#ifndef PYTHONEXECUTOR_H
#define PYTHONEXECUTOR_H

#include <QObject>
#include <QProcess>

class PythonExecutor : public QObject
{
    Q_OBJECT

public:
    explicit PythonExecutor(QObject *parent = nullptr);
    ~PythonExecutor();

    // Starts the Python script with the given parameters
    void executeScript(const QString &scriptPath, const QStringList &arguments = QStringList());

signals:
    // Signal emitted when the Python script's output is available
    void outputReceived(const QString &output);

    // Signal emitted when the Python script has finished execution
    void finished(bool success, const QString &error);

private slots:
    // Slot to handle the readyReadStandardOutput signal from QProcess
    void onReadyReadStandardOutput();

    // Slot to handle the finished signal from QProcess
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_process; // The QProcess that runs the Python script
};

#endif // PYTHONEXECUTOR_H
