#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include <bubble.h>
#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QEvent>
#include "global.h"

class TextBubble : public Bubble
{
    Q_OBJECT
public:
    explicit TextBubble(MSG_OWNER owner,QWidget *parent = nullptr);
    void setText(const QString &text);
protected:
    virtual bool eventFilter(QObject *o,QEvent *e) override;

private:
    void preferredHeigh();
    QTextEdit *text_;

};

#endif // TEXTBUBBLE_H
