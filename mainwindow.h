#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QList>
#include <QDebug>
#include <QTime>
#include "serialportworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updatePort();
    void updateCurrentPortInfo();

signals:
    void startWorker();
    void portUpdated();
    void closeCurrentPort();

private slots:
    void on_comPortEnum_currentIndexChanged(int index);
    void updateDataBox(QString data);
    void availablePorts(QList<QSerialPortInfo> p);

    void on_openPortButton_clicked();
    void on_ClosePortButton_clicked();

    void workerStarted();
    void workerStopped();

private:
    Ui::MainWindow *ui;
    QSerialPortInfo *currentPortInfo;
    QList<QSerialPortInfo> *ports;
    QThread *wthread;
    SerialPortWorker *pw;
};

#endif // MAINWINDOW_H
