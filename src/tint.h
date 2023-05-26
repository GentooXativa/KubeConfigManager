#ifndef TINT_H
#define TINT_H

#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

static QImage tintImage(QImage src, QColor color, qreal strength = 1.0)
{
    if (src.isNull())
        return QImage();
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    QGraphicsColorizeEffect effect;
    effect.setColor(color);
    effect.setStrength(strength);
    item.setGraphicsEffect(&effect);
    scene.addItem(&item);
    QImage res(src);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), src.rect());
    return res;
}

#endif // TINT_H