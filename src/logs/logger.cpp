#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include "logger.h"

Ttyh::Logs::Logger::Logger(const QString &workDir, int logsCount)
{
    auto logsPath = QString("%1/%2").arg(workDir, "logs");
    QDir().mkpath(logsPath);

    auto logFilePath = QString("%1/ttyh-launcher.%2.log");
    for (auto i = logsCount - 1; i >= 0; i--) {
        auto currLog = logFilePath.arg(logsPath, QString::number(i));

        if (QFile::exists(currLog))
            QFile::remove(currLog);

        if (i == 0)
            break;

        auto prevLog = logFilePath.arg(logsPath, QString::number(i - 1));

        if (QFile::exists(prevLog))
            QFile::rename(prevLog, currLog);
    }

    logFile.setFileName(logFilePath.arg(logsPath, "0"));
    if (!logFile.open(QIODevice::Text | QIODevice::Append | QIODevice::WriteOnly)) {
        QTextStream(stderr) << "Can't open a logfile!" << endl;
    }
}

void Ttyh::Logs::Logger::info(const QString &who, const QString &msg)
{
    log("INFO", who, msg);
}

void Ttyh::Logs::Logger::warning(const QString &who, const QString &msg)
{
    log("WARNING", who, msg);
}

void Ttyh::Logs::Logger::error(const QString &who, const QString &msg)
{
    log("ERROR", who, msg);
}

void Ttyh::Logs::Logger::log(const QString &lvl, const QString &who, const QString &msg)
{
    auto dateTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    raw(QString("%1 [%2] %3: %4").arg(dateTime, lvl, who, msg));
}

void Ttyh::Logs::Logger::raw(const QString &line)
{
    if (logFile.isOpen()) {
        QTextStream stream(&logFile);
        stream.setCodec("UTF-8");
        stream << line << '\n';
    }

    emit onLog(line);
}
