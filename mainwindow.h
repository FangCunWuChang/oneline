#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cacuthread.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_wid_valueChanged(int arg1);

    void on_hei_valueChanged(int arg1);

    void on_search_clicked();

    void onct_finished();

    void onct_result(QString text);

private:
    Ui::MainWindow *ui;
    QList<CacuThread*> ctl;
    int threadEndCount=0;
};
#endif // MAINWINDOW_H
