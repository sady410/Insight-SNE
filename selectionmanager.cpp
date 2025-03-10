#include "selectionmanager.h"
#include <QDebug>
#include <QStack>

SelectionManager::SelectionManager(QObject* parent) : QObject(parent), nextId(0) {}

void SelectionManager::createSelection(const QColor& color, const QList<int>& indices) {
    int newId = generateUniqueId(); // Generate a unique ID for the new selection
    Selection newSelection(newId, color, indices);

    selections.insert(newId, newSelection); // Add the new selection to the map

    emit selectionCreated(newId, color, indices); // Emit signal with the new selection
}

void SelectionManager::removeSelection(int id) {
    if (selections.contains(id)) {
        selections.remove(id); // Remove the selection from the map
        availableIds.push(id); // Add the ID back to the available pool
        emit selectionRemoved(id); // Emit signal with the ID of the removed selection
    } else {
        qWarning() << "Selection with ID" << id << "not found.";
    }
}

int SelectionManager::generateUniqueId() {
    if (!availableIds.isEmpty()) {
        return availableIds.pop(); // Reuse an available ID
    }
    return nextId++; // Otherwise, generate a new ID
}
