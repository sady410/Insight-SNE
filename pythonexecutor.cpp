#include "pythonexecutor.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

PythonExecutor::PythonExecutor(QObject *parent)
    : QObject(parent),
    process(new QProcess(this))
{
    // Connect signals from QProcess to our slots
    connect(process, &QProcess::readyReadStandardOutput, this, &PythonExecutor::onReadyReadStandardOutput);
    connect(process, &QProcess::finished, this, &PythonExecutor::onProcessFinished);
}

PythonExecutor::~PythonExecutor()
{
    // Ensure the process is cleaned up
    if (process->state() == QProcess::Running) {
        process->terminate();
    }
}

void PythonExecutor::executeScript(const QString &scriptPath, const QStringList &arguments)
{
    QString projectRoot = QDir(QCoreApplication::applicationDirPath()).absolutePath();
    if (projectRoot.contains("build")) {
        if (QSysInfo::productType() == "windows") {
            projectRoot = QDir(projectRoot).filePath("../../..");
        } else {
            projectRoot = QDir(projectRoot).filePath("../../");
        }
    }

    QString pythonExecutable;
    if (QSysInfo::productType() == "windows") {
        pythonExecutable = QDir(projectRoot).filePath("venv/Scripts/python.exe");
    } else { // Linux & Mac
        pythonExecutable = QDir(projectRoot).filePath("venv/bin/python");
    }

    QStringList params;
    params << scriptPath << arguments;
    // qDebug() << pythonExecutable << params;
    process->start(pythonExecutable, params);

    // Check if the process started successfully
    if (!process->waitForStarted()) {
        emit finished(false, "Failed to start the Python process.");
    }
}

void PythonExecutor::onReadyReadStandardOutput()
{
    // Read the output from the Python process
    QString output = process->readAllStandardOutput();
    emit outputReceived(output);  // Emit the output via the signal
}

void PythonExecutor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        emit finished(false, "Python process crashed.");
    } else if (exitCode != 0) {
        emit finished(false, QString("Python process finished with error code %1").arg(exitCode));
    } else {
        emit finished(true, "Python script completed successfully.");
    }
}
