#include "arrowsmanager.h"

#include <QDebug>

ArrowsManager::ArrowsManager(QObject* parent) : QObject(parent)
{

}

void ArrowsManager::createArrowsField(int selectionId, const QList<int>& indices, const QString featureName) {
    int newId = generateUniqueId();

    ArrowsField newArrowsField(newId, selectionId, indices, featureName);
    m_arrowsField.insert(newId, newArrowsField);

    emit arrowsFieldCreated(newId, indices, featureName);
}

void ArrowsManager::removeArrowsField(int selectionId) {
    int id = findIdBySelectionId(selectionId);
    m_arrowsField.remove(id);
    m_availableIds.push(id);
    emit arrowsFieldRemoved(id);
}

int ArrowsManager::findIdBySelectionId(int selectionId) const
{
    for (auto it = m_arrowsField.constBegin(); it != m_arrowsField.constEnd(); ++it) {
        if (it.value().selectionId == selectionId) {
            return it.key(); // return the id (the key in the hash)
        }
    }
    return -1; // return -1 if not found
}

int ArrowsManager::generateUniqueId() {
    if (!m_availableIds.isEmpty()) {
        return m_availableIds.pop(); // Reuse an available ID
    }
    return m_nextId++; // Otherwise, generate a new ID
}
