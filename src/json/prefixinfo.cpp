#include "prefixinfo.h"

Ttyh::Json::PrefixInfo::PrefixInfo() : type(""), about("") {}

Ttyh::Json::PrefixInfo::PrefixInfo(const QJsonObject &jObject)
    : type(jObject[keyType].toString("")), about(jObject[keyAbout].toString(""))
{
}

QJsonObject Ttyh::Json::PrefixInfo::toJsonObject() const
{
    return QJsonObject { { keyType, type }, { keyAbout, about } };
}
