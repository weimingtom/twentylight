#ifndef TWCANVAS_H
#define TWCANVAS_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QLabel>
#include <QImage>

class TwCanvas : public QLabel
{
    Q_OBJECT
public:
    explicit TwCanvas(QWidget *parent = 0);
    //~TwCanvas(){ } //FIXME:不一定是虚的？
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QImage *image; //FIXME:不要用QPixmap，有内存错误
};

#endif // TWCANVAS_H
