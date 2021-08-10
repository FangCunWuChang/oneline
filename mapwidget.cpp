#include "mapwidget.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    repaint();
}

void MapWidget::setPoints(int wid,int hei)
{
    this->wid=wid;
    this->hei=hei;
    this->lines.clear();
    this->pointon.clear();
    this->pressedPoint=qMakePair(-1,-1);
    this->dline=QLine(QPoint(-100,-100),QPoint(-100,-100));
    this->repaint();
}

void MapWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    this->repaint();
}

void MapWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    QColor pointus(150,150,150);
    QColor points(255,0,128);
    QColor pointh(255,0,128,100);

    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    int pointsize=qMin(this->width()/25,this->height()/25);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    pen.setColor(pointus);
    pen.setWidth(qMin(this->width()/30,this->height()/30));
    painter.setPen(pen);

    painter.drawLine(dline);

    auto prf=[px,py,tabsize](int i,int j){return QPoint(px+i*tabsize,py+j*tabsize);};

    for(int i=0;i<lines.size();i++){
        Line line=lines.at(i);
        QPoint sp=prf(line.startp.first,line.startp.second),ep=prf(line.endp.first,line.endp.second);

        if(line.state==Line::pressedState){
            pen.setColor(pointus);
            painter.setPen(pen);
        }else{
            pen.setColor(pointh);
            painter.setPen(pen);
        }

        painter.drawLine(sp,ep);

    }

    pen.setWidth(1);
    pen.setColor(pointus);
    painter.setPen(pen);
    brush.setColor(pointus);
    painter.setBrush(brush);

    for(int i=0;i<wid;i++){
        for(int j=0;j<hei;j++){
            int xpos=px+i*tabsize;
            int ypos=py+j*tabsize;

            if(pressedPoint==qMakePair(i,j)){
                pen.setColor(pointh);
                painter.setPen(pen);
                brush.setColor(pointh);
                painter.setBrush(brush);
            }else{
                if(pointon.contains(qMakePair(i,j))){
                    pen.setColor(points);
                    painter.setPen(pen);
                    brush.setColor(points);
                    painter.setBrush(brush);
                }else{
                    pen.setColor(pointus);
                    painter.setPen(pen);
                    brush.setColor(pointus);
                    painter.setBrush(brush);
                }
            }


            QRect roundrect(xpos-pointsize/2,ypos-pointsize/2,pointsize,pointsize);
            painter.drawEllipse(roundrect);
        }
    }

}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton){
        QPoint pos=event->pos();
        if(this->mouseFlag==0){
            if(isPoint(pos)){
                this->pressedPoint=getPoint(pos);
                this->mouseFlag=1;
            }else if(isLine(pos)){
                Line line=getLine(pos);
                for(int i=0;i<lines.size();i++){
                    if(lines.at(i)==line){
                        line.state=Line::pressedState;
                        lines.replace(i,line);
                    }
                }
                this->mouseFlag=3;
            }else{

            }
        }else if(this->mouseFlag==1){

        }else if(mouseFlag==2){

        }else if(mouseFlag==3){

        }else{
            if(isPoint(pos)){
                this->pressedPoint=getPoint(pos);
                this->mouseFlag=1;
            }else if(isLine(pos)){
                Line line=getLine(pos);
                for(int i=0;i<lines.size();i++){
                    if(lines.at(i)==line){
                        line.state=Line::pressedState;
                        lines.replace(i,line);
                    }
                }
                this->mouseFlag=3;
            }else{

            }
        }
    }
    repaint();
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton){
        QPoint pos=event->pos();
        if(this->mouseFlag==0){

        }else if(this->mouseFlag==1){
            if(isPoint(pos)){
                if(pressedPoint==getPoint(pos)){
                    if(pointon.contains(this->pressedPoint)){
                        pointon.remove(this->pressedPoint);
                        parseLineDel();
                    }else{
                        pointon.insert(this->pressedPoint);
                        parseLineAdd();
                    }
                }
            }
        }else if(mouseFlag==2){
            if(isPoint(pos)){

                QPair<int,int> point=getPoint(pos);
                if(pointon.contains(pressedPoint)&&pointon.contains(point)){
                    addLine(pressedPoint,point);
                }
            }
        }else if(mouseFlag==3){
            if(isPoint(pos)){
                for(int i=0;i<lines.size();i++){
                    Line linet=lines.at(i);
                    linet.state=Line::defaultState;
                    lines.replace(i,linet);
                }
            }else if(isLine(pos)){
                Line line=getLine(pos);
                for(int i=0;i<lines.size();){
                    if(lines.at(i)==line){
                        if(lines.at(i).state==Line::pressedState){
                            lines.removeAt(i);
                            continue;
                        }
                    }
                    Line linet=lines.at(i);
                    linet.state=Line::defaultState;
                    lines.replace(i,linet);
                    i++;
                }
            }else{
                for(int i=0;i<lines.size();i++){
                    Line linet=lines.at(i);
                    linet.state=Line::defaultState;
                    lines.replace(i,linet);
                }
            }
        }else{

        }
        this->mouseFlag=0;
        this->dline=QLine(QPoint(-100,-100),QPoint(-100,-100));
        this->pressedPoint=qMakePair(-1,-1);
    }
    repaint();
}

void MapWidget::addLine(QPair<int,int> point1,QPair<int,int> point2)
{
    int x1=point1.first,y1=point1.second;
    int x2=point2.first,y2=point2.second;

    QList<QPair<int,int>> passPoints;

    auto passf=[x1,y1,x2,y2](QPair<int,int> point)->bool
    {
        int x=point.first,y=point.second;
        if(x1==x2){
            return (x==x1)&&(y>qMin(y1,y2)&&y<qMax(y1,y2));
        }else if(y1==y2){
            return (y==y1)&&(x>qMin(x1,x2)&&x<qMax(x1,x2));
        }else{
            double xp=(double)(x-x1)/(double)(x2-x1);
            double yp=(double)(y-y1)/(double)(y2-y1);
            return xp==yp&&(y>qMin(y1,y2)&&y<qMax(y1,y2));
        }

    };

    QList<QPair<int,int>> pov=pointon.values();
    for(int i=0;i<pov.size();i++){
        if(passf(pov.at(i))){
            passPoints.append(pov.at(i));
        }
    }

    QList<QPair<int,int>> pointlist;
    pointlist.append(point1);
    pointlist.append(point2);

    auto dtf=[](QPair<int,int> p1,QPair<int,int> p2)->double
    {
        return sqrt((double)(p1.first-p2.first)*(double)(p1.first-p2.first)+(double)(p1.second-p2.second)*(double)(p1.second-p2.second));
    };


    for(int i=0;i<passPoints.size();i++){
        QPair<int,int> point=passPoints.at(i);

        double pdt=dtf(point,point1);
        for(int j=0;j<pointlist.size()-1;j++){
            if(dtf(pointlist.at(j),point1)<pdt&&dtf(pointlist.at(j+1),point1)>pdt){
                pointlist.insert(j+1,point);
                break;
            }
        }
    }

    for(int i=0;i<pointlist.size()-1;i++){
        Line line1;
        line1.startp=pointlist.at(i);
        line1.endp=pointlist.at(i+1);
        if(!lines.contains(line1)){
            lines.append(line1);
        }
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    auto prf=[px,py,tabsize](int i,int j){return QPoint(px+i*tabsize,py+j*tabsize);};

    QPoint pos=event->pos();
    if(this->mouseFlag==0){
        if(isPoint(pos)){
            this->setCursor(Qt::PointingHandCursor);
        }else if(isLine(pos)){
            if(lines.contains(getLine(pos))){
                this->setCursor(Qt::PointingHandCursor);
            }else{
                this->setCursor(Qt::ArrowCursor);
            }
        }else{
            this->setCursor(Qt::ArrowCursor);
        }
    }else if(this->mouseFlag==1){
        if(isPoint(pos)){
            if(pressedPoint==getPoint(pos)){
                this->setCursor(Qt::PointingHandCursor);
            }else{
                this->setCursor(Qt::ArrowCursor);
            }

        }else if(isLine(pos)){
            this->setCursor(Qt::ArrowCursor);
            if(pointon.contains(pressedPoint)){
                QLine line=this->dline;
                line.setP1(prf(pressedPoint.first,pressedPoint.second));
                line.setP2(pos);
                this->dline=line;
                mouseFlag=2;
            }
        }else{
            this->setCursor(Qt::ArrowCursor);
            if(pointon.contains(pressedPoint)){
                QLine line=this->dline;
                line.setP1(prf(pressedPoint.first,pressedPoint.second));
                line.setP2(pos);
                this->dline=line;
                mouseFlag=2;
            }
        }
    }else if(this->mouseFlag==2){
        if(isPoint(pos)){
            if(pointon.contains(getPoint(pos))){
                this->setCursor(Qt::PointingHandCursor);
            }else{
                this->setCursor(Qt::ArrowCursor);
            }
        }else if(isLine(pos)){
            this->setCursor(Qt::ArrowCursor);
        }else{
            this->setCursor(Qt::ArrowCursor);
        }
        QLine line=this->dline;
        line.setP2(pos);
        this->dline=line;
    }else if(mouseFlag==3){
        if(isPoint(pos)){
            this->setCursor(Qt::ArrowCursor);
        }else if(isLine(pos)){
            Line line=getLine(pos);
            bool secf=false;
            for(int i=0;i<lines.size();i++){
                if(lines.at(i)==line){
                    if(lines.at(i).state==Line::pressedState){
                        secf=true;
                        break;
                    }
                }
            }
            if(secf){
                this->setCursor(Qt::PointingHandCursor);
            }else{
                this->setCursor(Qt::ArrowCursor);
            }
        }else{
            this->setCursor(Qt::ArrowCursor);
        }
    }else{
        if(isPoint(pos)){
            this->setCursor(Qt::PointingHandCursor);
        }else if(isLine(pos)){
            this->setCursor(Qt::PointingHandCursor);
        }else{
            this->setCursor(Qt::ArrowCursor);
        }
    }

    repaint();
}

void MapWidget::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    this->mouseFlag=0;
    this->dline=QLine(QPoint(-100,-100),QPoint(-100,-100));
    this->pressedPoint=qMakePair(-1,-1);
    repaint();
}

bool MapWidget::isPoint(QPoint pos)
{
    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    int pointsize=qMin(this->width()/25,this->height()/25);

    for(int i=0;i<wid;i++){
        for(int j=0;j<hei;j++){
            int xpos=px+i*tabsize;
            int ypos=py+j*tabsize;

            QRect roundrect(xpos-pointsize/2,ypos-pointsize/2,pointsize,pointsize);
            if(inRect(roundrect,pos)){
                return true;
            }
        }
    }
    return false;
}

bool MapWidget::isLine(QPoint pos)
{
    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    auto prf=[px,py,tabsize](int i,int j){return QPoint(px+i*tabsize,py+j*tabsize);};

    for(int i=0;i<lines.size();i++){
        Line line=lines.at(i);
        QPoint sp=prf(line.startp.first,line.startp.second),ep=prf(line.endp.first,line.endp.second);
        if(inLine(sp,ep,pos)){
            return true;
        }
    }
    return false;
}

QPair<int,int> MapWidget::getPoint(QPoint pos)
{
    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    int pointsize=qMin(this->width()/25,this->height()/25);

    for(int i=0;i<wid;i++){
        for(int j=0;j<hei;j++){
            int xpos=px+i*tabsize;
            int ypos=py+j*tabsize;

            QRect roundrect(xpos-pointsize/2,ypos-pointsize/2,pointsize,pointsize);
            if(inRect(roundrect,pos)){
                return qMakePair(i,j);
            }
        }
    }
    return qMakePair(-1,-1);
}

MapWidget::Line MapWidget::getLine(QPoint pos)
{
    int tabsize=qMin((double)((double)(this->width()*0.9)/(double)(this->wid-1)),(double)((double)(this->height()*0.9)/(double)(this->hei-1)));
    int halfw=(tabsize*(wid-1))/2;
    int halfh=(tabsize*(hei-1))/2;
    int px=this->width()/2-halfw;
    int py=this->height()/2-halfh;

    auto prf=[px,py,tabsize](int i,int j){return QPoint(px+i*tabsize,py+j*tabsize);};

    for(int i=0;i<lines.size();i++){
        Line line=lines.at(i);
        QPoint sp=prf(line.startp.first,line.startp.second),ep=prf(line.endp.first,line.endp.second);
        if(inLine(sp,ep,pos)){
            return line;
        }
    }
    return Line();
}

bool MapWidget::inRect(QRect rect,QPoint point)
{
    return (point.x()>=rect.x()&&point.x()<=rect.x()+rect.width()&&point.y()>=rect.y()&&point.y()<=rect.y()+rect.height());
}

bool MapWidget::inLine(QPoint p1,QPoint p2,QPoint point)
{
    int x1=p1.x(),y1=p1.y(),x2=p2.x(),y2=p2.y();
    if(x1!=x2){
        double a=(double)((double)(y2-y1)/(double)(x2-x1));
        double b=-1;
        double c=(double)y1-(double)(a*x1);

        double p=point.x();
        double q=point.y();

        double d=abs(a*p+b*q+c)/sqrt(a*a+b*b);

        auto inlf=[](QPoint po,QPoint p1,QPoint p2)->bool
        {
            int v1x=p1.x()-po.x(),v1y=p1.y()-po.y();
            int v2x=p2.x()-po.x(),v2y=p2.y()-po.y();
            double cosv=((double)(v1x*v2x+v1y*v2y)/(double)(sqrt(v1x*v1x+v1y*v1y)*sqrt(v2x*v2x+v2y*v2y)));
            return cosv>0;
        };

        return (d<=qMin(this->width()/30,this->height()/30))&&inlf(p1,p2,point)&&inlf(p2,p1,point);
    }else{
        return (abs(point.x()-x1)<=qMin(this->width()/30,this->height()/30))&&(point.y()>=qMin(y1,y2)&&point.y()<=qMax(y1,y2));
    }
}

void MapWidget::parseLineDel()
{
    for(int i=0;i<lines.size();){
        if(!(pointon.contains(lines.at(i).startp)&&pointon.contains(lines.at(i).endp))){
            lines.removeAt(i);
            continue;
        }
        i++;
    }
}

void MapWidget::parseLineAdd()
{
    QVector<Line> linet;
    QSet<QPair<int,int>> pointon=this->pointon;
    auto linef=[&linet,pointon](QPair<int,int> point1,QPair<int,int> point2)
    {
        int x1=point1.first,y1=point1.second;
        int x2=point2.first,y2=point2.second;

        QList<QPair<int,int>> passPoints;

        auto passf=[x1,y1,x2,y2](QPair<int,int> point)->bool
        {
            int x=point.first,y=point.second;
            if(x1==x2){
                return (x==x1)&&(y>qMin(y1,y2)&&y<qMax(y1,y2));
            }else if(y1==y2){
                return (y==y1)&&(x>qMin(x1,x2)&&x<qMax(x1,x2));
            }else{
                double xp=(double)(x-x1)/(double)(x2-x1);
                double yp=(double)(y-y1)/(double)(y2-y1);
                return xp==yp&&(y>qMin(y1,y2)&&y<qMax(y1,y2));
            }

        };

        QList<QPair<int,int>> pov=pointon.values();
        for(int i=0;i<pov.size();i++){
            if(passf(pov.at(i))){
                passPoints.append(pov.at(i));
            }
        }

        QList<QPair<int,int>> pointlist;
        pointlist.append(point1);
        pointlist.append(point2);

        auto dtf=[](QPair<int,int> p1,QPair<int,int> p2)->double
        {
            return sqrt((double)(p1.first-p2.first)*(double)(p1.first-p2.first)+(double)(p1.second-p2.second)*(double)(p1.second-p2.second));
        };


        for(int i=0;i<passPoints.size();i++){
            QPair<int,int> point=passPoints.at(i);

            double pdt=dtf(point,point1);
            for(int j=0;j<pointlist.size()-1;j++){
                if(dtf(pointlist.at(j),point1)<pdt&&dtf(pointlist.at(j+1),point1)>pdt){
                    pointlist.insert(j+1,point);
                    break;
                }
            }
        }

        for(int i=0;i<pointlist.size()-1;i++){
            Line line1;
            line1.startp=pointlist.at(i);
            line1.endp=pointlist.at(i+1);
            if(!linet.contains(line1)){
                linet.append(line1);
            }
        }
    };


    for(int i=0;i<lines.size();i++){
        linef(lines.at(i).startp,lines.at(i).endp);
    }

    this->lines=linet;
}

QList<MapWidget::Line> MapWidget::getLines()
{
    /*
    QList<Line> linelist;

    auto dtf=[](Line line)->double
    {
        int x1=line.startp.first,y1=line.startp.second;
        int x2=line.endp.first,y2=line.endp.second;
        return (double)x1*10+(double)y1+(double)x2*0.1+(double)y2*0.01;
    };

    auto pdtf=[](int x,int y)->double
    {
        return (double)x*10+(double)y;
    };

    for(int i=0;i<lines.size();i++){
        Line linet=lines.at(i);
        Line line=linet;
        line.startp=pdtf(linet.startp.first,linet.startp.second)<pdtf(linet.endp.first,linet.endp.second)?linet.startp:linet.endp;
        line.endp=pdtf(linet.startp.first,linet.startp.second)<pdtf(linet.endp.first,linet.endp.second)?linet.endp:linet.startp;
        if(linelist.size()==0){
            linelist.append(line);
        }else if(linelist.size()==1){
            if(dtf(line)>dtf(linelist.at(0))){
                linelist.append(line);
            }else{
                linelist.prepend(line);
            }
        }else{
            for(int j=0;j<linelist.size()-1;j++){
                if((dtf(line)>dtf(linelist.at(j)))&&(dtf(line)<dtf(linelist.at(j+1)))){
                    linelist.insert(j+1,line);
                    break;
                }
            }
        }
    }*/

    return lines;
}
