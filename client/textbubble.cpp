#include "textbubble.h"
#include <QDebug>
#include <QFont>
#include <QFile>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>

TextBubble::TextBubble(MSG_OWNER owner, QWidget *parent):Bubble(owner,parent)
{
    text_ = new QTextEdit;
    text_->setReadOnly(true);
    text_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    text_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    QFont f;
    f.setPointSize(10);
    f.setFamily("Microsoft YaHei");
    text_->setFont(f);
    setWidget(text_);
    text_->setContentsMargins(0,0,0,0);
    text_->installEventFilter(this);
}

bool TextBubble::eventFilter(QObject *o, QEvent *e)
{
   if(o==text_ && e->type() == QEvent::Paint)
   {
       preferredHeigh();
   }
   return Bubble::eventFilter(o,e);
}

void TextBubble::setText(const QString &text)
{


    text_->setPlainText(text);
//    找到段落中最大宽度
    qreal doc_margin =text_->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(text_->font());
    QTextDocument *doc =text_->document();
    int max_width = 0;
    //遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.horizontalAdvance(it.text()));
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
    }
//    设置这个气泡的最大宽度 只需要设置一次
    setMaximumWidth(max_width + margin_left + margin_right + doc_margin*2 + 5);


}

void TextBubble::preferredHeigh()
{
     qreal doc_margin = text_->document()->documentMargin();    //字体到边框的距离默认为4
    QTextDocument *doc = text_->document();
    qreal text_height = 0;
    //把每一段的高度相加=文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();                             //这段的rect
        text_height += text_rect.height();
    }
    int vMargin = this->layout()->contentsMargins().top();
    //设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin *2 + vMargin*2 );


    // setFixedHeight(text_->size().height() + layout()->contentsMargins().top() + layout()->contentsMargins().bottom());
}

