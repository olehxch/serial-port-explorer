#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_comPortEnum_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    // port vars
    QSerialPort *current;
    QSerialPortInfo *currentPortInfo;
    QList<QSerialPortInfo> *ports;
};

#endif // MAINWINDOW_H
