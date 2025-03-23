#include "selectionmanager.h"
#include <QDebug>
#include <QStack>

// Custom hash function for QColor
inline size_t qHash(const QColor &color, size_t seed = 0)
{
    return qHash(color.rgba(), seed); // Use the rgba value for hashing
}

SelectionManager::SelectionManager(QObject* parent) : QObject(parent), m_nextId(0) {
    m_colorList = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray, Qt::darkRed};
}

void SelectionManager::changeSelectionStateAndEmitSignal(int id) {
    if (m_selections.contains(id)) {
        Selection &sel = m_selections[id];
        sel.state = !sel.state;
        if (sel.state == true) {
            emit selectionEnabled(id, sel.color, sel.indices);
        } else {
            emit selectionDisabled(id);
        }
    }
}

void SelectionManager::createMultipleSelections(const QList<QList<int>> indicesList, const bool enableDelete) {
    for (const QList<int>& indices : indicesList) {
        createSelection(indices, enableDelete);
    }
}

void SelectionManager::createSelection(const QList<int>& indices) {
    createSelection(indices, true);  // Call the main function with default value
}


void SelectionManager::createSelection(const QList<int>& indices, const bool enableDelete) {
    if (m_colorList.size() == m_usedColors.size()) {
        qWarning() << "No available colors for new selection!";
        return;
    }
    int newId = generateUniqueId();
    QColor color = getAvailableColor();

    // Add the color to usedColors
    m_usedColors.insert(color);

    Selection newSelection(newId, color, indices);
    m_selections.insert(newId, newSelection);

    emit selectionCreated(newId, color, indices, enableDelete);
}

void SelectionManager::removeSelection(int id) {
    if (m_selections.contains(id)) {
        // Release the color associated with the selection
        releaseColor(m_selections.value(id).color);

        m_selections.remove(id);
        m_availableIds.push(id);
        emit selectionRemoved(id);
        emit stopHighlightSelection();
    } else {
        qWarning() << "Selection with ID" << id << "not found.";
    }
}

void SelectionManager::findHoveredSelectionAndEmitSignal(int id) {
    if (m_selections.contains(id)) {
        Selection selection = m_selections.value(id);
        emit startHighlightSelection(selection.color, selection.indices);
    } else {
        qWarning() << "Selection with ID" << id << "not found.";
    }
}

int SelectionManager::generateUniqueId() {
    if (!m_availableIds.isEmpty()) {
        return m_availableIds.pop(); // Reuse an available ID
    }
    return m_nextId++; // Otherwise, generate a new ID
}

QColor SelectionManager::getAvailableColor() {
    for (const QColor& color : std::as_const(m_colorList)) {
        if (!m_usedColors.contains(color)) {
            return color;
        }
    }
    return Qt::black; // Fallback (should not happen)
}

void SelectionManager::releaseColor(const QColor& color) {
    m_usedColors.remove(color);
}
