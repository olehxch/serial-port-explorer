#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QList>
#include <QDebug>
#include <QTime>

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
    void openCurrentPort(int index);
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

private:
    Ui::MainWindow *ui;

    // port vars
    QSerialPort *current;
    QSerialPortInfo *currentPortInfo;
    QList<QSerialPortInfo> *ports;
};

#endif // MAINWINDOW_H
