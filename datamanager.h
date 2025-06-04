#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QList>
#include <QSharedPointer>

class JsonParser;

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject* parent = nullptr);

    // Main method to load and parse JSON data
    bool loadJsonData(const QJsonObject& jsonData);

    // Getters for basic properties
    int perplexity() const { return m_perplexity; }
    int iterations() const { return m_iterations; }
    const QString& filename() const { return m_filename; }
    bool isLabel() const { return m_isLabel; }
    bool isClass() const { return m_isClass; }

    // Getters for lists (const references for efficiency)
    const QStringList& featureNames() const { return m_featureNamesList; }
    const QStringList& labels() const { return m_labelsList; }
    const QStringList& uniqueLabels() const { return m_uniqueLabels; }
    const QList<qreal>& xData() const { return m_xData; }
    const QList<qreal>& yData() const { return m_yData; }
    const QList<QList<int>>& indicesList() const { return m_indicesList; }

    // Lazy-loaded getters for complex data structures
    const QList<QList<QList<qreal>>>& gradients();
    const QList<QList<qreal>>& magnitudes();
    const QList<QList<qreal>>& attributes();

    // Utility methods
    bool isDataLoaded() const { return m_dataLoaded; }
    int dataSize() const { return m_xData.size(); }

private:
    void parseBasicData();
    void parseCoordinates();
    void parseLabelsAndIndices();
    void clearData();

    // JSON data storage
    QJsonObject m_jsonData;
    QJsonArray m_embeddingArray;
    QJsonArray m_labelsArray;
    QJsonArray m_featureNamesArray;
    QJsonArray m_gradientsArray;
    QJsonArray m_magnitudesArray;
    QJsonArray m_attributesArray;

    // Basic properties
    int m_perplexity{0};
    int m_iterations{0};
    QString m_filename;
    bool m_isLabel{false};
    bool m_isClass{false};
    bool m_dataLoaded{false};

    // Processed data
    QStringList m_featureNamesList;
    QStringList m_labelsList;
    QStringList m_uniqueLabels;
    QList<qreal> m_xData;
    QList<qreal> m_yData;
    QList<QList<int>> m_indicesList;

    // Lazy-loaded complex data structures
    mutable QList<QList<QList<qreal>>> m_gradientsList;
    mutable QList<QList<qreal>> m_magnitudesList;
    mutable QList<QList<qreal>> m_attributesList;
    mutable bool m_gradientsLoaded{false};
    mutable bool m_magnitudesLoaded{false};
    mutable bool m_attributesLoaded{false};
};

// Convenience typedef for shared pointer
using DataManagerPtr = QSharedPointer<DataManager>;

#endif // DATAMANAGER_H
