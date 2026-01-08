#ifndef TRANSLATIONUNITFRAME_H
#define TRANSLATIONUNITFRAME_H

#include <QFrame>

namespace Ui {
class TranslationUnitFrame;
}

class TranslationUnitFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TranslationUnitFrame(QWidget *parent = nullptr);
    ~TranslationUnitFrame();

private:
    Ui::TranslationUnitFrame *ui;
};

#endif // TRANSLATIONUNITFRAME_H
