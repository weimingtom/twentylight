#include "twcanvas.h"

TwCanvas::TwCanvas(QWidget *parent) :
    QLabel(parent),
    image(0)
{
    image = new QImage(tr("../qt/Windows_XP_Bliss.jpg"));
    this->setBackgroundRole(QPalette::Base);
    //this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //this->setScaledContents(true);
    //QImage image(tr("nanami.jpg"));
    //this->setPixmap(QPixmap::fromImage(image));
    //this->adjustSize();
}

//FIXME:参数可以忽略不写
void TwCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    this->setFixedSize(image->width(), image->height());
    painter.drawPixmap(0, 0, image->width(), image->height(),
                       QPixmap::fromImage(*image));
}
