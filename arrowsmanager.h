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
        QColor color;
        QList<int> indices;
        QString featureName;
        ArrowsField(int id = -1, const QList<int>& indices = {}, const QString featureName = "")
            : id(id), indices(indices), featureName(featureName) {}
    };

    void setGradients(QList<QList<QList<qreal>>> gradients) { m_gradients = gradients; }

signals:
    void arrowsFieldCreated(int id, QList<int> arrows); // TODO not int
    void arrowsFieldRemoved(int id);

public slots:
    void createArrowsField(const QList<int>& indices, const QString featureName);
    void removeArrowsField(int id);

private:
    QList<QList<QList<qreal>>> m_gradients;

    QHash<int, ArrowsField> m_arrowsField; // Use QHash for efficient lookups by ID
    QStack<int> m_availableIds; // Stack to store reusable IDs
    int m_nextId; // Counter for generating new IDs

    int generateUniqueId();
};

#endif // ARROWSMANAGER_H
