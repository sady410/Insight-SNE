#ifndef SCOPECOMPUTATION_H
#define SCOPECOMPUTATION_H

#include <QList>

QList<qreal> computeInstanceAngles(
    int id,
    const QList<QList<QList<qreal>>>& gradients
    );

#endif // SCOPECOMPUTATION_H
