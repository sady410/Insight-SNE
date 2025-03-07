// JsonParser.h
#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

class JsonParser {
public:
    static QList<qreal> jsonArrayToQList(const QJsonArray &jsonArray);
    static QList<QList<qreal>> jsonArrayToQList2D(const QJsonArray &jsonArray);
    static QList<QList<QList<qreal>>> jsonArrayToQList3D(const QJsonArray &jsonArray);
};

#endif // JSONPARSER_H
