#include "datamanager.h"
#include "jsonparser.h"
#include <QDebug>
#include <QSet>

DataManager::DataManager(QObject* parent)
    : QObject(parent)
{
}

bool DataManager::loadJsonData(const QJsonObject& jsonData)
{
    try {
        clearData();
        m_jsonData = jsonData;

        if (m_jsonData.isEmpty()) {
            qWarning() << "JSON data is empty";
            return false;
        }

        // Parse JSON arrays
        m_embeddingArray = m_jsonData.value("Y").toArray();
        m_labelsArray = m_jsonData.value("labels").toArray();
        m_featureNamesArray = m_jsonData.value("featureNames").toArray();
        m_gradientsArray = m_jsonData.value("gradients").toArray();
        m_magnitudesArray = m_jsonData.value("magnitudes").toArray();
        m_attributesArray = m_jsonData.value("data").toArray();

        parseBasicData();
        parseCoordinates();
        parseLabelsAndIndices();

        m_dataLoaded = true;
        return true;

    } catch (const std::exception& e) {
        clearData();
        qWarning() << "Failed to load data:" << e.what();
        return false;
    }
}

void DataManager::parseBasicData()
{
    m_perplexity = m_jsonData.value("perplexity").toInt();
    m_iterations = m_jsonData.value("iterations").toInt();
    m_filename = m_jsonData.value("filename").toString();
    m_isLabel = m_jsonData.value("showLabels").toBool();
    m_isClass = m_jsonData.value("isClass").toBool();

    // Parse feature names
    m_featureNamesList.clear();
    m_featureNamesList.reserve(m_featureNamesArray.size());
    for (const QJsonValue &value : std::as_const(m_featureNamesArray)) {
        m_featureNamesList.append(value.toString());
    }

    // Parse labels
    m_labelsList.clear();
    m_labelsList.reserve(m_labelsArray.size());
    for (const QJsonValue &value : std::as_const(m_labelsArray)) {
        m_labelsList.append(value.toString());
    }
}

void DataManager::parseCoordinates()
{
    m_xData.clear();
    m_yData.clear();
    m_xData.reserve(m_embeddingArray.size());
    m_yData.reserve(m_embeddingArray.size());

    for (const QJsonValue &val : std::as_const(m_embeddingArray)) {
        QJsonArray point = val.toArray();
        if (point.size() == 2) {
            m_xData.append(point[0].toDouble());
            m_yData.append(point[1].toDouble());
        }
    }
}

void DataManager::parseLabelsAndIndices()
{
    m_indicesList.clear();
    m_uniqueLabels.clear();

    if (m_labelsList.isEmpty()) {
        return;
    }

    // Create unique labels
    QSet<QString> seenLabels;
    for (const QString& label : std::as_const(m_labelsList)) {
        if (!seenLabels.contains(label)) {
            seenLabels.insert(label);
            m_uniqueLabels.append(label);
        }
    }

    // Create indices map
    QMap<QString, QList<int>> classIndicesMap;
    QList<int> allIndices;
    allIndices.reserve(m_labelsList.size());

    for (int i = 0; i < m_labelsList.size(); ++i) {
        allIndices.append(i);
        classIndicesMap[m_labelsList[i]].append(i);
    }

    m_indicesList.append(allIndices);
    for (const auto& label : std::as_const(m_uniqueLabels)) {
        m_indicesList.append(classIndicesMap[label]);
    }
}

const QList<QList<QList<qreal>>>& DataManager::gradients()
{
    if (!m_gradientsLoaded && !m_gradientsArray.isEmpty()) {
        m_gradientsList = JsonParser::jsonArrayToQList3D(m_gradientsArray);
        m_gradientsLoaded = true;
    }
    return m_gradientsList;
}

const QList<QList<qreal>>& DataManager::magnitudes()
{
    if (!m_magnitudesLoaded && !m_magnitudesArray.isEmpty()) {
        m_magnitudesList = JsonParser::jsonArrayToQList2D(m_magnitudesArray);
        m_magnitudesLoaded = true;
    }
    return m_magnitudesList;
}

const QList<QList<qreal>>& DataManager::attributes()
{
    if (!m_attributesLoaded && !m_attributesArray.isEmpty()) {
        m_attributesList = JsonParser::jsonArrayToQList2D(m_attributesArray);
        m_attributesLoaded = true;
    }
    return m_attributesList;
}

const QList<QList<qreal>>& DataManager::xAxisGradients()
{
    if (!m_xAxisGradientsLoaded && !m_gradientsArray.isEmpty()) {
        // Ensure the main gradients are loaded first
        const auto& gradsList = gradients();
        m_xAxisGradientsList = getAxisGradient(gradsList, 0); // X-axis is index 0
        m_xAxisGradientsLoaded = true;
    }
    return m_xAxisGradientsList;
}

const QList<QList<qreal>>& DataManager::yAxisGradients()
{
    if (!m_yAxisGradientsLoaded && !m_gradientsArray.isEmpty()) {
        // Ensure the main gradients are loaded first
        const auto& gradsList = gradients();
        m_yAxisGradientsList = getAxisGradient(gradsList, 1); // Y-axis is index 1
        m_yAxisGradientsLoaded = true;
    }
    return m_yAxisGradientsList;
}

QList<QList<qreal>> DataManager::getAxisGradient(const QList<QList<QList<qreal>>>& gradientsList, int axis) const
{
    QList<QList<qreal>> result;
    result.reserve(gradientsList.size());

    for (const QList<QList<qreal>>& gradient : gradientsList) {
        if (axis >= 0 && axis < gradient.size()) {
            result.append(gradient[axis]);
        } else {
            qWarning() << "Axis" << axis << "out of bounds! Available axes: 0 to" << (gradient.size() - 1);
            return QList<QList<qreal>>(); // Return empty list on error
        }
    }
    return result;
}

void DataManager::clearData()
{
    m_jsonData = QJsonObject();
    m_embeddingArray = QJsonArray();
    m_labelsArray = QJsonArray();
    m_featureNamesArray = QJsonArray();
    m_gradientsArray = QJsonArray();
    m_magnitudesArray = QJsonArray();
    m_attributesArray = QJsonArray();

    m_perplexity = 0;
    m_iterations = 0;
    m_filename.clear();
    m_isLabel = false;
    m_isClass = false;
    m_dataLoaded = false;

    m_featureNamesList.clear();
    m_labelsList.clear();
    m_uniqueLabels.clear();
    m_xData.clear();
    m_yData.clear();
    m_indicesList.clear();

    m_gradientsList.clear();
    m_magnitudesList.clear();
    m_attributesList.clear();
    m_gradientsLoaded = false;
    m_magnitudesLoaded = false;
    m_attributesLoaded = false;

    // Clear the new axis-specific gradient data
    m_xAxisGradientsList.clear();
    m_yAxisGradientsList.clear();
    m_xAxisGradientsLoaded = false;
    m_yAxisGradientsLoaded = false;
}
