#include "platform.h"

bool Ttyh::Utils::Platform::isLibraryAllowed(const Json::LibraryInfo &libInfo)
{
    if (libInfo.rules.isEmpty())
        return true;

    auto allowed = false;

    foreach (auto rule, libInfo.rules) {
        auto sameOs = rule.os == getOsName();

        if (rule.action == "allow" && (sameOs || rule.os.isEmpty())) {
            allowed = true;
        } else if (rule.action == "disallow" && sameOs) {
            allowed = false;
        }
    }

    return allowed;
}

// From: "package.dot.separated:name:version"
// To:   "package/slash/separated/name/version/name-version[-classifier].jar"
QString Ttyh::Utils::Platform::getLibraryPath(const Json::LibraryInfo &libInfo)
{
    auto tokens = libInfo.name.split(':');

    auto package = tokens[0].replace('.', '/');
    auto name = tokens[1];
    auto version = tokens[2];

    auto os = getOsName();
    if (libInfo.natives.contains(os)) {
        auto classifier = libInfo.natives[os];
        classifier.replace("${arch}", getWordSize()); // QString have only a mutable `replace`
        return QString("%1/%2/%3/%2-%3-%4.jar").arg(package, name, version, classifier);
    }

    return QString("%1/%2/%3/%2-%3.jar").arg(package, name, version);
}

QString Ttyh::Utils::Platform::getOsName()
{
#ifdef Q_OS_LINUX
    return "linux";
#endif
#ifdef Q_OS_OSX
    return "osx";
#endif
#ifdef Q_OS_WIN
    return "windows";
#endif
}

QString Ttyh::Utils::Platform::getOsVersion()
{
    return QSysInfo::prettyProductName();
}

QString Ttyh::Utils::Platform::getWordSize()
{
    return QString::number(QSysInfo::WordSize);
}

QChar Ttyh::Utils::Platform::getClassPathSeparator()
{
#ifdef Q_OS_WIN
    return ';';
#else
    return ':';
#endif
}
