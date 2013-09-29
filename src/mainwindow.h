#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QDebug>
#include <QTime>
#include <QComboBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QFileDialog>
#include "serialportworker.h"
#include "updateserialportinfo.h"
#include "aboutdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateCurrentPortInfo();
    void updateDataStatistics();

signals:
    void startWorker();
    void portUpdated();
    void closeCurrentPort();

private slots:
    void updateDataBox(QString data);
    void availablePorts(QList<QSerialPortInfo> p);

    void on_comPortEnum_currentIndexChanged(int index);
    void updateStatusBar(QString s);
    void on_openPortButton_clicked();
    void on_ClosePortButton_clicked();

    void workerStarted();
    void workerStopped();

    void on_sendData_clicked();
    void on_actionAbout_triggered();
    void on_saveDataToFileButton_clicked();

    void incPacketsSent();
    void incPacketReceived();
    void clearPacketStatistics();
private:
    Ui::MainWindow *ui;
    QComboBox *comports;

    QSerialPortInfo *currentPortInfo;
    QList<QSerialPortInfo> *ports;
    QThread *wthread;

    SerialPortWorker *pw;
    UpdateSerialPortInfo *sinfo;

    unsigned int packetsSent;
    unsigned int packetsReceived;
};

#endif // MAINWINDOW_H
