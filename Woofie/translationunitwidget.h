#ifndef TRANSLATIONUNITWIDGET_H
#define TRANSLATIONUNITWIDGET_H

#include <QWidget>

namespace Ui {
class TranslationUnitWidget;
}

class TranslationUnitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationUnitWidget(QWidget *parent = nullptr);
    ~TranslationUnitWidget();
signals:
    void positionChanged(QPoint newPosition);
    void attemptConnection(TranslationUnitWidget* self);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::TranslationUnitWidget *ui;
    bool isBeingDragged;
    QPoint draggingPosition;
};

#endif // TRANSLATIONUNITWIDGET_H
