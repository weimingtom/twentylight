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
    //~TwCanvas(){ } //FIXME:��һ������ģ�
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QImage *image; //FIXME:��Ҫ��QPixmap�����ڴ����
};

#endif // TWCANVAS_H
