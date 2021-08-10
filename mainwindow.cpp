#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    for(int i=0;i<ctl.size();i++){
        CacuThread* ct=ctl.at(i);
        if(ct->isRunning()){
            ct->terminate();
            ct->wait();
        }
        delete ct;
    }
    delete ui;
}


void MainWindow::on_wid_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ui->map->setPoints(ui->wid->value(),ui->hei->value());
}


void MainWindow::on_hei_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ui->map->setPoints(ui->wid->value(),ui->hei->value());
}

void MainWindow::onct_finished()
{
    threadEndCount++;
    if(threadEndCount==ctl.size()){
        threadEndCount=0;
        ui->results->appendPlainText("全部线程已完成执行");
    }
    bool runningFlag=false;
    for(int i=0;i<ctl.size();i++){
        if(ctl.at(i)->isRunning()){
            runningFlag=true;
            break;
        }
    }
    if(!runningFlag){
        for(int i=0;i<ctl.size();i++){
            ctl.at(i)->deleteLater();
        }
        ctl.clear();
        ui->wid->setEnabled(true);
        ui->hei->setEnabled(true);
        ui->map->setEnabled(true);
        ui->time->setEnabled(true);
        ui->threads->setEnabled(true);
        ui->search->setText("搜索");
    }

}

void MainWindow::onct_result(QString text)
{
    ui->results->appendPlainText(text);
}

void MainWindow::on_search_clicked()
{
    bool runningFlag=false;
    for(int i=0;i<ctl.size();i++){
        if(ctl.at(i)->isRunning()){
            runningFlag=true;
            break;
        }
    }
    if(!runningFlag){
        threadEndCount=0;
        ui->results->clear();
        ui->wid->setEnabled(false);
        ui->hei->setEnabled(false);
        ui->map->setEnabled(false);
        ui->time->setEnabled(false);
        ui->threads->setEnabled(false);
        ui->search->setText("停止");

        QList<QList<int>> tasks;

        QList<MapWidget::Line> lines=ui->map->getLines();
        QList<QPair<int,int>> points=CacuThread::getPoints(lines);

        for(int i=0;i<ui->threads->value();i++){
            QList<int> task;
            tasks.append(task);

            CacuThread* ct=new CacuThread;
            connect(ct,&CacuThread::finished,this,&MainWindow::onct_finished,Qt::AutoConnection);
            connect(ct,&CacuThread::result,this,&MainWindow::onct_result,Qt::AutoConnection);
            ctl.append(ct);
        }

        for(int i=0;i<points.size();i++){
            int id=i%ui->threads->value();
            QList<int> task=tasks.at(id);
            task.append(i);
            tasks.replace(id,task);
        }

        for(int i=0;i<ctl.size();i++){
            ctl.at(i)->setTask(lines,points,ui->time->value(),i,tasks.at(i));
            ctl.at(i)->start();
        }

        ui->results->appendPlainText("搜索开始");
    }else{
        threadEndCount=0;
        for(int i=0;i<ctl.size();i++){
            if(ctl.at(i)->isRunning()){
                ctl.at(i)->terminate();
                ctl.at(i)->wait();
            }
            ctl.at(i)->deleteLater();
        }
        ctl.clear();
        ui->results->appendPlainText("搜索线程中止...");
        ui->wid->setEnabled(true);
        ui->hei->setEnabled(true);
        ui->map->setEnabled(true);
        ui->time->setEnabled(true);
        ui->threads->setEnabled(true);
        ui->search->setText("搜索");
    }

}

