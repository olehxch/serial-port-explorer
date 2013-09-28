/*
 * This class updates serial port info and shows all connected port changes in realtime
 */

#ifndef UPDATESERIALPORTINFO_H
#define UPDATESERIALPORTINFO_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPortInfo>

class UpdateSerialPortInfo: public QObject
{
    Q_OBJECT
public:
    UpdateSerialPortInfo();
    ~UpdateSerialPortInfo();
    void stop();

signals:
    void updated(QList<QSerialPortInfo> p);

public slots:
    void doWork();

private:
    bool stopWork;
    QList<QSerialPortInfo> *old;
    QList<QSerialPortInfo> *recent;

    bool compareQLists();
};

#endif // UPDATESERIALPORTINFO_H
