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
        QColor color;
        QList<int> indices;

        Selection(int id = -1, const QColor& color = Qt::black, const QList<int>& indices = {})
            : id(id), color(color), indices(indices) {}
    };

    explicit SelectionManager(QObject* parent = nullptr);

public slots:
    void createSelection(const QList<int>& indices);
    void removeSelection(int id);

signals:
    void selectionCreated(int id, const QColor &color, const QList<int>& indices);
    void selectionRemoved(int id);

private:
    QList<QColor> colorList;
    QSet<QColor> usedColors;

    QHash<int, Selection> selections; // Use QHash for efficient lookups by ID
    QStack<int> availableIds; // Stack to store reusable IDs
    int nextId; // Counter for generating new IDs

    int generateUniqueId();
    QColor getAvailableColor();
    void releaseColor(const QColor& color);
};

#endif // SELECTIONMANAGER_H
