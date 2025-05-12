#ifndef APPWINDOWBUILDER_H
#define APPWINDOWBUILDER_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QVector>

class AppWindow;

class AppWindowBuilder
{
public:
    explicit AppWindowBuilder(QWidget* parent = nullptr);

    // Set and parse JSON data in one operation
    AppWindowBuilder& setJsonData(const QJsonObject& jsonData);

    // Parse all JSON data at once
    AppWindowBuilder& parseJsonData();

    // Initialize different components
    void extracted(QSet<QString> &seenLabels);
    AppWindowBuilder &initializeTsnePlot();
    AppWindowBuilder& initializeOverviewPlot();
    AppWindowBuilder& initializeFeaturesPlot();
    AppWindowBuilder& initializeExplanationPlots();
    AppWindowBuilder& initializeMenuForm();
    AppWindowBuilder& initializeFeaturesForm();
    AppWindowBuilder& initializeDistributionPlot();

    // Build and return the window
    AppWindow* build();

private:
    // Lazy accessor methods
    const QList<QList<QList<qreal>>>& getGradientsList();
    const QList<QList<qreal>>& getMagnitudesList();
    const QList<QList<qreal>>& getAttributesList();

    // Member variables
    QWidget* m_parent;
    AppWindow* m_appWindow;
    QJsonObject m_jsonData;

    // Cached JSON arrays
    QJsonArray m_embeddingArray;
    QJsonArray m_labelsArray;
    QJsonArray m_featureNamesArray;
    QJsonArray m_gradientsArray;
    QJsonArray m_magnitudesArray;
    QJsonArray m_attributesArray;
    int m_perplexity;
    int m_iterations;
    QString m_filename;
    bool m_isLabel;
    bool m_isClass;

    // Parsed data structures
    QStringList m_featureNamesList;
    QStringList m_labelsList;
    QStringList m_uniqueLabels;
    QVector<double> m_xData;
    QVector<double> m_yData;
    QList<QList<int>> m_indicesList;

    // Lazily initialized data
    QList<QList<QList<qreal>>> m_gradientsList;
    QList<QList<qreal>> m_magnitudesList;
    QList<QList<qreal>> m_attributesList;
};

#endif // APPWINDOWBUILDER_H
