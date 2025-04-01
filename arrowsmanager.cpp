#include "arrowsmanager.h"

#include <QDebug>

ArrowsManager::ArrowsManager(QObject* parent) : QObject(parent)
{

}

void ArrowsManager::createArrowsField(int selectionId, const QList<int> &indices, const QString &featureName) {
    int newId = generateUniqueId();
    // TODO modify later color
    QColor color = Qt::red;
    ArrowsField newArrowsField(newId, selectionId, indices, featureName, color);
    m_arrowsField.insert(newId, newArrowsField);

    qDebug() << m_arrowsField.keys();
    emit arrowsFieldCreated(newId, indices, featureName, color);
}

void ArrowsManager::removeArrowsField(int selectionId, const QString &featureName) {
    int id = findIdBySelectionIdAndFeatureName(selectionId, featureName);
    if (id == -1) {
        qDebug() << "Error: No arrows field found for selectionId" << selectionId;
        return; // Prevent invalid operations
    }

    m_arrowsField.remove(id);
    m_availableIds.push(id);
    qDebug() << m_arrowsField.keys();
    emit arrowsFieldRemoved(id);
}

int ArrowsManager::findIdBySelectionIdAndFeatureName(int selectionId, const QString &featureName) const
{
    for (auto it = m_arrowsField.constBegin(); it != m_arrowsField.constEnd(); ++it) {
        if (it.value().selectionId == selectionId && it.value().featureName == featureName) {
            return it.key(); // Return the matching id
        }
    }
    return -1; // Return -1 if no match is found
}

int ArrowsManager::generateUniqueId() {
    if (!m_availableIds.isEmpty()) {
        return m_availableIds.pop(); // Reuse an available ID
    }
    return m_nextId++; // Otherwise, generate a new ID
}
