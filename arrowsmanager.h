#ifndef ARROWSMANAGER_H
#define ARROWSMANAGER_H

#include <QObject>
#include <QColor>
#include <QStack>
#include <QHash>

class ArrowsManager: public QObject {
    Q_OBJECT

public:
    explicit ArrowsManager(QObject* parent = nullptr);

    struct ArrowsField {
        int id;
        int selectionId;
        QList<int> indices;
        QString featureName;
        QColor color;
        ArrowsField(int id = -1, int selectionId = -1, const QList<int>& indices = {}, const QString featureName = "", const QColor color = Qt::red)
            : id(id), selectionId(selectionId), indices(indices), featureName(featureName), color(color) {}
    };

    void setGradients(QList<QList<QList<qreal>>> gradients) { m_gradients = gradients; }

signals:
    void arrowsFieldCreated(int id, const QList<int> &arrows, const QString &featureName, const QColor &color);
    void arrowsFieldRemoved(int id);
    void arrowsFieldColorChanged(int id, const QColor &newColor);

public slots:
    void createArrowsField(int selectionId, const QList<int> &indices, const QString &featureName);
    void removeArrowsField(int selectionId, const QString &featureName);
    void changeArrowsFieldColor(int selectionId, const QString &featureName, const QColor &newColor);

private:
    QList<QList<QList<qreal>>> m_gradients;

    QHash<int, ArrowsField> m_arrowsField; // Use QHash for efficient lookups by ID
    QStack<int> m_availableIds; // Stack to store reusable IDs
    int m_nextId; // Counter for generating new IDs

    int generateUniqueId();
    int findIdBySelectionIdAndFeatureName(int selectionId, const QString &featureName) const;
};

#endif // ARROWSMANAGER_H
