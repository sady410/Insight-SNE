#include "scopecomputation.h"
#include <QtMath>

namespace {
// These are file-local now (anonymous namespace)
qreal vectorNorm(const QList<qreal>& v) {
    qreal sum = 0.0;
    for (qreal val : v) {
        sum += val * val;
    }
    return std::sqrt(sum);
}

qreal dotProduct(const QList<qreal>& v1, const QList<qreal>& v2) {
    qreal result = 0.0;
    int size = qMin(v1.size(), v2.size());
    for (int i = 0; i < size; ++i) {
        result += v1[i] * v2[i];
    }
    return result;
}
}

qreal getAngle(const QList<qreal>& v1, const QList<qreal>& v2) {
    qreal norm1 = vectorNorm(v1);
    qreal norm2 = vectorNorm(v2);
    if (norm1 == 0 || norm2 == 0)
        return 0.0;
    qreal dot = dotProduct(v1, v2);
    return std::abs(qBound(-1.0, dot / (norm1 * norm2), 1.0)); // clamp the cosine value between -1 and 1
}

qreal harmonicMean(qreal a, qreal b) {
    if (a == 0 || b == 0)
        return 0.0;
    return 2.0 * a * b / (a + b);
}

QList<qreal> computeInstanceAngles(
    int id,
    const QList<QList<QList<qreal>>>& gradients
    ) {
    int size = gradients.size();
    QList<qreal> angles(size, 0.0);

    if (id < 0 || id >= size || gradients[id].size() < 2)
        return angles;

    const QList<qreal>& refVec0 = gradients[id][0];
    const QList<qreal>& refVec1 = gradients[id][1];

    for (int i = 0; i < size; ++i) {
        if (gradients[i].size() < 2)
            continue;

        qreal angle1 = getAngle(refVec0, gradients[i][0]);
        qreal angle2 = getAngle(refVec1, gradients[i][1]);

        angles[i] = harmonicMean(angle1, angle2);
    }

    return angles;
}
