// JsonParser.cpp
#include "jsonparser.h"

QList<qreal> JsonParser::jsonArrayToQList(const QJsonArray &jsonArray) {
    QList<qreal> list;
    for (const auto &value : jsonArray) {
        if (value.isDouble()) {
            list.append(value.toDouble());
        }
    }
    return list;
}

QList<QList<qreal>> JsonParser::jsonArrayToQList2D(const QJsonArray &jsonArray) {
    QList<QList<qreal>> listOfLists2D;
    for (const QJsonValue &outerValue : jsonArray) {
        if (outerValue.isArray()) {
            QJsonArray outerArray = outerValue.toArray();
            QList<qreal> innerList;
            for (const QJsonValue &value : outerArray) {
                if (value.isDouble()) {
                    innerList.append(value.toDouble());
                }
            }
            listOfLists2D.append(innerList);
        }
    }
    return listOfLists2D;
}

QList<QList<QList<qreal>>> JsonParser::jsonArrayToQList3D(const QJsonArray &jsonArray) {
    QList<QList<QList<qreal>>> listOfLists3D;
    for (const QJsonValue &outerValue : jsonArray) {
        if (outerValue.isArray()) {
            QJsonArray outerArray = outerValue.toArray();
            QList<QList<qreal>> listOfLists2D;
            for (const QJsonValue &innerValue : outerArray) {
                if (innerValue.isArray()) {
                    QJsonArray innerArray = innerValue.toArray();
                    QList<qreal> innerList;
                    for (const QJsonValue &value : innerArray) {
                        if (value.isDouble()) {
                            innerList.append(value.toDouble());
                        }
                    }
                    listOfLists2D.append(innerList);
                }
            }
            listOfLists3D.append(listOfLists2D);
        }
    }
    return listOfLists3D;
}
