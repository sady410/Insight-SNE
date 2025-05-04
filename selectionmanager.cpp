#include "selectionmanager.h"
#include <QDebug>
#include <QStack>

// Custom hash function for QColor
inline size_t qHash(const QColor &color, size_t seed = 0)
{
    return qHash(color.rgba(), seed); // Use the rgba value for hashing
}

SelectionManager::SelectionManager(QObject* parent) : QObject(parent), m_nextId(0) {
    m_colorList = {Qt::magenta, Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::gray, Qt::darkRed};
}

void SelectionManager::changeSelectionStateAndEmitSignal(int id) {
    if (m_selections.contains(id)) {
        Selection &sel = m_selections[id];
        sel.state = !sel.state;
        if (sel.state == true) {
            emit selectionEnabled(id, sel.color, sel.indices, true, sel.state);
        } else {
            emit selectionDisabled(id);
        }
    }
}

void SelectionManager::createSelection(const QList<int>& indices) {
    createSelection(indices, true, true, "");  // Call the main function with default value
}


void SelectionManager::createSelection(const QList<int>& indices, const bool enableDelete, const bool state, const QString name) {
    if (m_colorList.size() == m_usedColors.size()) {
        qWarning() << "No available colors for new selection!";
        return;
    }
    int newId = generateUniqueId();
    QColor color = getAvailableColor();
    // Add the color to usedColors
    m_usedColors.insert(color);

    Selection newSelection(newId, color, name, indices, state);
    m_selections.insert(newId, newSelection);

    emit selectionCreated(newId, color, indices, enableDelete, state, name);
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

void SelectionManager::selectionClickedInBarPlot(const QString &label, int id) {
    qDebug() << label << id; // TODO
}

int SelectionManager::generateUniqueId() {
    if (!m_availableIds.isEmpty()) {
        return m_availableIds.pop(); // Reuse an available ID
    }
    return m_nextId++; // Otherwise, generate a new ID
}

void SelectionManager::selectionFreatureArrowCheckboxChanged(int id, const QString &featureName, const bool checked) {
    if (m_selections.contains(id)) {
        Selection &sel = m_selections[id];
        QList<int> indices = sel.indices;
        if (checked) {
            emit addArrows(id, indices, featureName);
        } else {
            qDebug() << "test";
            emit removeArrows(id, featureName);
        }
    }
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
