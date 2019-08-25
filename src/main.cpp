#include <QApplication>
#include <QtCore/QSharedPointer>
#include <QtCore/QTranslator>

#include "config.h"
#include "launcher.h"
#include "logs/logger.h"
#include "settings/settingsmanager.h"
#include "versions/versionsmanager.h"
#include "profiles/profilesmanager.h"
#include "profiles/profilerunner.h"
#include "master/ttyhclient.h"
#include "storage/filechecker.h"
#include "storage/downloader.h"
#include "news/newsfeed.h"
#include "utils/migrations.h"

using namespace Ttyh;
using namespace Ttyh::Logs;
using namespace Ttyh::Settings;
using namespace Ttyh::Versions;
using namespace Ttyh::Profiles;
using namespace Ttyh::Master;
using namespace Ttyh::News;
using namespace Ttyh::Storage;

template<typename T>
using QSP = QSharedPointer<T>;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator t;
    QDate today = QDate::currentDate();
    if (today.month() == 4 && today.day() == 1) {
        t.load(":/translations/koi7.qm");
    } else {
        t.load(":/translations/ru.qm");
    }
    QApplication::installTranslator(&t);

    QApplication::setApplicationVersion(PROJECT_VERSION);
    QApplication::setApplicationName("TtyhLauncher");

    const QString storeUrl = "https://ttyh.ru/files/newstore";
    const QString masterUrl = "https://master.ttyh.ru";
    const QString newsUrl = "https://ttyh.ru/misc.php?page=feed";
    const QString dirName = "ttyhlauncher2";

    const int logCount = 3;

    auto qDel = &QObject::deleteLater;
    auto nam = QSP<QNetworkAccessManager>(new QNetworkAccessManager(), qDel);
    auto logger = QSP<Logger>(new Logger(dirName, logCount), qDel);
    QObject::connect(logger.data(), &Logger::onLog,
                     [](const QString &line) { QTextStream(stdout) << line << endl; });

    auto settings = QSP<SettingsManager>(new SettingsManager(dirName, logger));
    if (settings->isFreshRun()) {
        Utils::Migrations::restoreLoginSettings(settings, logger);
    }

    auto profiles = QSP<ProfilesManager>(new ProfilesManager(dirName, logger));
    auto versions = QSP<VersionsManager>(new VersionsManager(dirName, storeUrl, nam, logger), qDel);

    auto ticket = settings->data.ticket;
    auto client = QSP<TtyhClient>(new TtyhClient(masterUrl, ticket, nam, logger), qDel);

    auto checker = QSP<FileChecker>(new FileChecker(dirName, logger), qDel);
    auto downloader = QSP<Downloader>(new Downloader(storeUrl, dirName, nam, logger), qDel);
    auto runner = QSP<ProfileRunner>(new ProfileRunner(dirName, logger), qDel);
    auto feed = QSP<NewsFeed>(new NewsFeed(newsUrl, nam, logger), qDel);

    Launcher l(settings, profiles, versions, client, checker, downloader, runner, feed, logger);
    l.start();

    return QApplication::exec();
}
