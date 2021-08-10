#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QSet>
#include <QPair>

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    void setPoints(int wid,int hei);

    class Line{
    public:
        Line()
        {

        }

        Line(const Line & arg)
        {
            this->startp=arg.startp;
            this->endp=arg.endp;
            this->state=arg.state;
            this->flag1=arg.flag1;
        }

        QPair<int,int> startp,endp;
        enum lineStates{
            defaultState=0,
            pressedState=1
        };
        lineStates state=defaultState;
        bool flag1=false;

        Line & operator=(const Line & arg)
        {
            if(&arg!=this){
                this->startp=arg.startp;
                this->endp=arg.endp;
                this->state=arg.state;
                this->flag1=arg.flag1;
            }
            return *this;
        }

        bool operator==(const Line & arg) const
        {
            return ((this->startp==arg.startp)&&(this->endp==arg.endp))||((this->startp==arg.endp)&&(this->endp==arg.startp));
        }

        bool operator!=(const Line & arg) const
        {
            return !(((this->startp==arg.startp)&&(this->endp==arg.endp))||((this->startp==arg.endp)&&(this->endp==arg.startp)));
        }
    };

    QList<Line> getLines();
private:
    int wid=5;
    int hei=5;

    QPair<int,int> pressedPoint=qMakePair(-1,-1);
    QSet<QPair<int,int>> pointon;
    QList<Line> lines;

    int mouseFlag=0;
    //0:default
    //1:pressed
    //2:pressed and move

    bool isPoint(QPoint pos);
    bool isLine(QPoint pos);
    QPair<int,int> getPoint(QPoint pos);
    Line getLine(QPoint pos);
    QLine dline=QLine(QPoint(-100,-100),QPoint(-100,-100));

    void addLine(QPair<int,int> point1,QPair<int,int> point2);

    bool inRect(QRect rect,QPoint point);
    bool inLine(QPoint p1,QPoint p2,QPoint point);

    void parseLineDel();
    void parseLineAdd();
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

signals:

};

#endif // MAPWIDGET_H
