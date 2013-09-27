/*
 * This class manages threads for reading/sending data from com port
 *
 */

#ifndef SERIALPORTWORKER_H
#define SERIALPORTWORKER_H

#include <QString>
#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QDebug>

class SerialPortWorker : public QObject
{
    Q_OBJECT
public:
    SerialPortWorker(QSerialPortInfo pinfo);
    SerialPortWorker(QSerialPortInfo pinfo, QSerialPort::BaudRate br, QSerialPort::DataBits db, QSerialPort::StopBits sb, QSerialPort::Parity p, QSerialPort::FlowControl fc);
    ~SerialPortWorker();
    void closePort();

signals:
    void dataReceived(QString data);
    void started();
    void stopped();

public slots:
    void doWork();

private:
    QSerialPort *port;
    bool stop;
};

#endif // SERIALPORTDATAMANAGER_H
