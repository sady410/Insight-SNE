#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QHash>
#include <QStack>
#include <QSet>

class SelectionManager : public QObject {
    Q_OBJECT

public:
    struct Selection {
        int id;
        bool state;
        QColor color;
        QList<int> indices;

        Selection(int id = -1, const QColor& color = Qt::black, const QList<int>& indices = {}, const bool state = true)
            : id(id), state(state), color(color), indices(indices) {}
    };

    explicit SelectionManager(QObject* parent = nullptr);

public slots:
    void createMultipleSelections(const QList<QList<int>> indicesList, const bool enableDelete);

    void createSelection(const QList<int>& indices, const bool enableDelete);
    void createSelection(const QList<int>& indices);
    void removeSelection(int id);

    void findHoveredSelectionAndEmitSignal(int id);

    void changeSelectionStateAndEmitSignal(int id);

signals:
    void selectionCreated(int id, const QColor &color, const QList<int>& indices, const bool enableDelete);
    void selectionRemoved(int id);

    // Signals used when hovering in MenuForm
    void startHighlightSelection(const QColor &color, const QList<int>& indices);
    void stopHighlightSelection();

    // Signals used when changing a selection's state
    void selectionEnabled(int id, const QColor &color, const QList<int>& indices);
    void selectionDisabled(int id);

private:
    QList<QColor> m_colorList;
    QSet<QColor> m_usedColors;

    QHash<int, Selection> m_selections; // Use QHash for efficient lookups by ID
    QStack<int> m_availableIds; // Stack to store reusable IDs
    int m_nextId; // Counter for generating new IDs

    int generateUniqueId();
    QColor getAvailableColor();
    void releaseColor(const QColor& color);
};

#endif // SELECTIONMANAGER_H
