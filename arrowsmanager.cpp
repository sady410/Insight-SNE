#include "arrowsmanager.h"

#include <QDebug>

ArrowsManager::ArrowsManager(QObject* parent) : QObject(parent)
{

}

void ArrowsManager::createArrowsField(const QList<int>& indices, const QString featureName) {
    int newId = generateUniqueId();

    ArrowsField newArrowsField(newId, indices, featureName);
    m_arrowsField.insert(newId, newArrowsField);

    // TODO Create arrows drawable objects
    // emit arrowsFieldCreated(newId, ArrowsList); // TODO in h file change int
}

void ArrowsManager::removeArrowsField(int id) {
    if (m_arrowsField.contains(id)) {

        m_arrowsField.remove(id);
        m_availableIds.push(id);
        // emit arrowsFieldRemoved(id);
    } else {
        qWarning() << "ArrowsField with ID" << id << "not found.";
    }
}

int ArrowsManager::generateUniqueId() {
    if (!m_availableIds.isEmpty()) {
        return m_availableIds.pop(); // Reuse an available ID
    }
    return m_nextId++; // Otherwise, generate a new ID
}
