#include "cacuthread.h"

CacuThread::CacuThread()
{

}

void CacuThread::setTask(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,int sec,int id,QList<int> tasks)
{
    this->lines=lines;
    this->points=points;
    this->sec=sec;
    this->id=id;
    this->tasks=tasks;
}

void CacuThread::run()
{
    QTime startTime=QTime::currentTime();

    emit result(QString::asprintf("[%s]线程%d启动",qPrintable(startTime.toString("hh:mm:ss")),id));

    if(sec>0){
        if(startTime.secsTo(QTime::currentTime())>sec){
            emit finished();
            quit();
            return;
        }
    }

    for(int i=0;i<tasks.size();i++){
        if(tasks.at(i)<points.size()){
            QPair<int,int> startPoint=points.at(tasks.at(i));

            startSearch(lines,points,startPoint,startTime);

            if(sec>0){
                if(startTime.secsTo(QTime::currentTime())>sec){
                    emit finished();
                    quit();
                    return;
                }
            }
        }
    }

    emit result(QString::asprintf("[%s]线程%d结束，耗时%d秒",qPrintable(startTime.toString("hh:mm:ss")),id,startTime.secsTo(QTime::currentTime())));
    emit finished();
    quit();
    return;
}

QList<QPair<int,int>> CacuThread::getPoints(QList<MapWidget::Line> lines)
{
    QList<QPair<int,int>> out;
    for(int i=0;i<lines.size();i++){
        if(!out.contains(lines.at(i).startp)){
            out.append(lines.at(i).startp);
        }
        if(!out.contains(lines.at(i).endp)){
            out.append(lines.at(i).endp);
        }
    }
    return out;
}

void CacuThread::startSearch(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QPair<int,int> startPoint,QTime startTime)
{
    QList<QPair<int,int>> pointPath;
    QList<MapWidget::Line> blackLines;
    DFS(lines,points,pointPath,startPoint,blackLines,startTime);
}

QList<QPair<int,int>> CacuThread::nextPoint(QList<MapWidget::Line> lines,QPair<int,int> current,QPair<double,double> pos)
{
    QList<QPair<int,int>> out;
    posLineGet(lines,current,pos,&out);
    return out;
}

void CacuThread::posLineGet(QList<MapWidget::Line> lines,QPair<int,int> current,QPair<double,double> pos,QList<QPair<int,int>>* results)
{
    auto cosF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltax/c;
    };

    auto sinF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltay/c;
    };

    auto posF=[cosF,sinF](QPair<int,int> O,QPair<int,int> A)->QPair<double,double>
    {
        return qMakePair(cosF(O,A),sinF(O,A));
    };//方向函数

    for(int i=0;i<lines.size();i++){
        MapWidget::Line line=lines.at(i);

        if(line.startp==current){
            QPair<double,double> post=posF(current,line.endp);
            if(post==pos){
                results->append(line.endp);
                posLineGet(lines,line.endp,pos,results);
            }
        }
        if(line.endp==current){
            QPair<double,double> post=posF(current,line.startp);
            if(post==pos){
                results->append(line.startp);
                posLineGet(lines,line.startp,pos,results);
            }
        }
    }
}

void CacuThread::DFS(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QList<QPair<int,int>> pointPath,QPair<int,int> currentPoint,QList<MapWidget::Line> blackLines,QTime startTime)
{
    QCoreApplication::processEvents();
    if(sec>0){
        if(startTime.secsTo(QTime::currentTime())>sec){
            return;
        }
    }//时间控制

    QList<QPair<int,int>> path=pointPath;
    path.append(currentPoint);//路径至此点

    QVector<QPair<double,double>> posList;//连接方向(cos,sin)

    auto cosF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltax/c;
    };

    auto sinF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltay/c;
    };

    auto posF=[cosF,sinF](QPair<int,int> O,QPair<int,int> A)->QPair<double,double>
    {
        return qMakePair(cosF(O,A),sinF(O,A));
    };//方向函数

    for(int i=0;i<lines.size();i++){
        MapWidget::Line line=lines.at(i);

        if(line.startp==currentPoint){
            QPair<double,double> pos=posF(currentPoint,line.endp);
            if(!posList.contains(pos)){
                posList.append(pos);
                //qDebug("(%d,%d)cos:%.2f sin:%.2f",currentPoint.first,currentPoint.second,pos.first,pos.second);
            }
        }
        if(line.endp==currentPoint){
            QPair<double,double> pos=posF(currentPoint,line.startp);
            if(!posList.contains(pos)){
                posList.append(pos);
                //qDebug("(%d,%d)cos:%.2f sin:%.2f",currentPoint.first,currentPoint.second,pos.first,pos.second);
            }
        }
    }//方向统计


    QList<pGroup> nextList;//下一层参数列表

    for(int i=0;i<posList.size();i++){
        QList<QPair<int,int>> nextPoints=nextPoint(lines,currentPoint,posList.at(i));//获取该方向可连接的点列表

        for(int j=0;j<nextPoints.size();j++){
            //qDebug("cos:%.2f sin:%.2f next:(%d,%d)",posList.at(i).first,posList.at(i).second,nextPoints.at(j).first,nextPoints.at(j).second);

            MapWidget::Line linet;
            linet.startp=currentPoint;
            linet.endp=nextPoints.at(j);
            //拟定连接直线

            if(!blackLines.contains(linet)){//排除黑名单连接（重复链接）
                pGroup pgTask;
                QList<MapWidget::Line> bltemp=blackLines;
                bltemp.append(linet);
                pgTask.blackLines=bltemp;//新连接入黑名单

                QList<MapWidget::Line> linetemp=lines;
                QList<QPair<int,int>> pointtemp=points;
                pgTask.point=nextPoints.at(j);
                //缓存准备

                if(checkPointHover(linet,lines,points,bltemp)){//判断覆盖的点不作为末端
                    solvePassedPoints(linet,&linetemp,&pointtemp);//处理经过点与通过经过点的直线

                    pgTask.points=pointtemp;
                    pgTask.lines=linetemp;
                    nextList.append(pgTask);//新搜索任务
                }
            }
        }
    }

    for(int i=0;i<nextList.size();i++){
        //qDebug("next:(%d,%d)",nextList.at(i).point.first,nextList.at(i).point.second);
        DFS(nextList.at(i).lines,nextList.at(i).points,path,nextList.at(i).point,nextList.at(i).blackLines,startTime);
    }

    if(nextList.size()==0){
        if(checkFinished(lines,blackLines)){
            emit result(getResults(path));
        }
    }

}

QString CacuThread::getResults(QList<QPair<int,int>> pointPath)
{
    QString out="起点->";
    for(int i=0;i<pointPath.size();i++){
        out+=QString(QString::asprintf("(%d,%d)->",pointPath.at(i).first,pointPath.at(i).second));
    }
    out+="终点";
    return out;
}

bool CacuThread::checkFinished(QList<MapWidget::Line> lines,QList<MapWidget::Line> blacklines)
{
    //return true;
    for(int i=0;i<lines.size();i++){
        if(!blacklines.contains(lines.at(i))){
            return false;
        }
    }
    return true;
}

void CacuThread::solvePassedPoints(MapWidget::Line currentLine,QList<MapWidget::Line>* lines,QList<QPair<int,int>>* points)
{
    auto cosF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltax/c;
    };

    auto sinF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltay/c;
    };

    auto posF=[cosF,sinF](QPair<int,int> O,QPair<int,int> A)->QPair<double,double>
    {
        return qMakePair(cosF(O,A),sinF(O,A));
    };//方向函数

    QList<QPair<int,int>> passPoints=getPassPoints(currentLine,*points);//获得覆盖点

    for(int i=0;i<passPoints.size();i++){

        QList<MapWidget::Line> newLines;

        int pindex=points->indexOf(passPoints.at(i));
        points->removeAt(pindex);//从点序列中移除

        QList<MapWidget::Line> lineConn;
        for(int j=0;j<lines->size();j++){
            if(lines->at(j).startp==passPoints.at(i)){
                if(!lineConn.contains(lines->at(j))){
                    lineConn.append(lines->at(j));
                }
            }else if(lines->at(j).endp==passPoints.at(i)){
                MapWidget::Line linetemp=lines->at(j);
                linetemp.startp=lines->at(j).endp;
                linetemp.endp=lines->at(j).startp;
                if(!lineConn.contains(linetemp)){
                    lineConn.append(linetemp);
                }
            }else{
                newLines.append(lines->at(j));
            }
        }

        QList<QPair<QPair<double,double>,MapWidget::Line>> posesl;
        for(int j=0;j<lineConn.size();j++){
            QPair<QPair<double,double>,MapWidget::Line> pairTemp=qMakePair(posF(lineConn.at(j).startp,lineConn.at(j).endp),lineConn.at(j));
            if(!posesl.contains(pairTemp)){
                posesl.append(pairTemp);
            }
        }

        while (posesl.size()>0) {
            QPair<double,double> antipos=qMakePair(-(posesl.at(0).first.first),-(posesl.at(0).first.second));
            bool containFlag=false;
            for(int j=0;j<posesl.size();j++){
                if(posesl.at(j).first==antipos){
                    containFlag=true;
                    MapWidget::Line newline;
                    newline.startp=posesl.at(0).second.endp;
                    newline.endp=posesl.at(j).second.endp;
                    newLines.append(newline);
                    posesl.removeAt(j);
                    posesl.removeAt(0);
                    break;
                }
            }
            if(!containFlag){
                posesl.removeAt(0);
            }
        }

        *lines=newLines;

    }

}

bool CacuThread::checkPointHover(MapWidget::Line currentLine,QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QList<MapWidget::Line> blacklines)
{
    auto cosF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltax/c;
    };

    auto sinF=[](QPair<int,int> O,QPair<int,int> A)->double
    {
        double deltax=A.first-O.first;
        double deltay=A.second-O.second;
        double c=sqrt(deltax*deltax+deltay*deltay);
        return deltay/c;
    };

    auto posF=[cosF,sinF](QPair<int,int> O,QPair<int,int> A)->QPair<double,double>
    {
        return qMakePair(cosF(O,A),sinF(O,A));
    };//方向函数

    QList<QPair<int,int>> passPoints=getPassPoints(currentLine,points);//获得覆盖点
    //qDebug("pass points size:%lld",passPoints.size());
    for(int i=0;i<passPoints.size();i++){
        //qDebug("%d",i);
        //qDebug("line:(%d,%d),(%d,%d) pass:(%d,%d)",currentLine.startp.first,currentLine.startp.second,currentLine.endp.first,currentLine.endp.second,passPoints.at(i).first,passPoints.at(i).second);
        QList<MapWidget::Line> lineConn;
        for(int j=0;j<lines.size();j++){
            if(lines.at(j).startp==passPoints.at(i)){
                if(!lineConn.contains(lines.at(j))){
                    lineConn.append(lines.at(j));
                }
            }
            if(lines.at(j).endp==passPoints.at(i)){
                MapWidget::Line linetemp=lines.at(j);
                linetemp.startp=lines.at(j).endp;
                linetemp.endp=lines.at(j).startp;
                if(!lineConn.contains(linetemp)){
                    lineConn.append(linetemp);
                }
            }
        }

        QList<QPair<QPair<double,double>,MapWidget::Line>> posesl;
        for(int j=0;j<lineConn.size();j++){
            QPair<QPair<double,double>,MapWidget::Line> pairTemp=qMakePair(posF(lineConn.at(j).startp,lineConn.at(j).endp),lineConn.at(j));
            if(!posesl.contains(pairTemp)){
                posesl.append(pairTemp);
            }
        }

        while (posesl.size()>0) {
            QPair<double,double> antipos=qMakePair(-(posesl.at(0).first.first),-(posesl.at(0).first.second));
            bool containFlag=false;
            for(int j=0;j<posesl.size();j++){
                if(posesl.at(j).first==antipos){
                    containFlag=true;
                    posesl.removeAt(j);
                    posesl.removeAt(0);
                    break;
                }
            }
            if(!containFlag){
                MapWidget::Line linetemp=posesl.at(0).second;
                if(!blacklines.contains(linetemp)){
                    return false;
                }
                posesl.removeAt(0);
            }
        }
    }
    return true;
}

QList<QPair<int,int>> CacuThread::getPassPoints(MapWidget::Line line,QList<QPair<int,int>> points)
{
    QList<QPair<int,int>> out;

    int x1=line.startp.first,y1=line.startp.second;
    int x2=line.endp.first,y2=line.endp.second;

    auto aF=[](int x1,int y1,int x2,int y2)->double
    {
        return (double)(y2-y1)/(double)(x2-x1);
    };

    for(int i=0;i<points.size();i++){

        int x=points.at(i).first,y=points.at(i).second;
        //qDebug("pass point checking:(%d,%d)",x,y);

        if(x1==x2){
            if(x==x1&&y>qMin(y1,y2)&&y<qMax(y1,y2)){
                out.append(points.at(i));
                //qDebug("pass point add:(%d,%d)",x,y);
            }
        }else if(y1==y2){
            if(y==y1&&x>qMin(x1,x2)&&x<qMax(x1,x2)){
                out.append(points.at(i));
                //qDebug("pass point add:(%d,%d)",x,y);
            }
        }else{
            if((aF(x1,y1,x2,y2)==aF(x1,y1,x,y))&&x>qMin(x1,x2)&&x<qMax(x1,x2)){
                out.append(points.at(i));
                //qDebug("pass point add:(%d,%d)",x,y);
            }
        }
    }
    return out;
}
