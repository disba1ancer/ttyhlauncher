#ifndef FILEFETCHER_H
#define FILEFETCHER_H

#include <QtCore>
#include <QtNetwork>

#include "logger.h"

class FileFetcher : public QObject
{
    Q_OBJECT

public:
    explicit FileFetcher(QObject *parent = 0);
    ~FileFetcher();

    void add(QUrl url, QString filename);
    void add(QUrl url, QString filename, quint64 size);
    void fetchSizes();
    void fetchFiles();

    quint64 getFetchSize();
    int getCount();

public slots:
    void reset();
    void cancel();

private:
    quint64 fetched;
    quint64 fetchSize;

    QList< QPair<QUrl, QString> > fetchData;
    int current;

    QNetworkAccessManager *nam;
    QNetworkReply *fetchReply;

    bool hasFetchErrors;

    Logger *logger;
    void log(const QString &text);

signals:
    void sizesFetchProgress(int progress);
    void sizesFetchError(QString errorString);
    void sizesFetchFinished();

    void filesFetchNewTarget(QString url, QString fname);
    void filesFetchError(QString errorString);
    void filesFetchProgress(int progress);
    void filesFetchFinished();
    void filesFetchResult(bool result);

private slots:
    void fetchCurrentSize();
    void updateFetchSize();

    void fetchCurrentFile();
    void fetchError(QNetworkReply::NetworkError error);
    void fetchSslError(QList<QSslError> err);
    void saveCurrentFile();
    void fileFetchProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // FILEFETCHER_H
