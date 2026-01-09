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
    explicit TranslationUnitWidget(const QString& target, const QString& source, QWidget *parent = nullptr);
    ~TranslationUnitWidget();

    QString getTargetText() const;
    QString getSourceText() const;

    void setTargetText(const QString& target);
    void setSourceText(const QString& source);
signals:
    void positionChanged(QPoint newPosition);
    void attemptConnection(TranslationUnitWidget* self);
    void attemptEdit(TranslationUnitWidget* self);

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
