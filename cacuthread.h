#ifndef CACUTHREAD_H
#define CACUTHREAD_H

#include <QThread>
#include <QObject>

#include <QList>
#include <QDateTime>
#include <QCoreApplication>

#include "mapwidget.h"

class CacuThread : public QThread
{
    Q_OBJECT
public:
    explicit CacuThread();

    void setTask(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,int sec,int id,QList<int> tasks);

    static QList<QPair<int,int>> getPoints(QList<MapWidget::Line> lines);

private:

    struct pGroup{
        QPair<int,int> point;
        QList<QPair<int,int>> points;
        QList<MapWidget::Line> lines;
        QList<MapWidget::Line> blackLines;
    };

    QList<MapWidget::Line> lines;
    QList<QPair<int,int>> points;
    int sec=0;
    int id=-1;
    QList<int> tasks;

    void posLineGet(QList<MapWidget::Line> lines,QPair<int,int> current,QPair<double,double> pos,QList<QPair<int,int>>* results);
    QString getResults(QList<QPair<int,int>> pointPath);
    bool checkFinished(QList<MapWidget::Line> lines,QList<MapWidget::Line> blacklines);
    bool checkPointHover(MapWidget::Line currentLine,QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QList<MapWidget::Line> blacklines);
    QList<QPair<int,int>> getPassPoints(MapWidget::Line line,QList<QPair<int,int>> points);
    void solvePassedPoints(MapWidget::Line currentLine,QList<MapWidget::Line>* lines,QList<QPair<int,int>>* points);
    QList<QPair<int,int>> nextPoint(QList<MapWidget::Line> lines,QPair<int,int> current,QPair<double,double> pos);

    void startSearch(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QPair<int,int> startPoint,QTime startTime);
    void DFS(QList<MapWidget::Line> lines,QList<QPair<int,int>> points,QList<QPair<int,int>> pointPath,QPair<int,int> currentPoint,QList<MapWidget::Line> blackLines,QTime startTime);
protected:
    void run() override;

signals:
    void finished();
    void result(QString text);
};

#endif // CACUTHREAD_H
