/*
 * This class manages threads for reading/sending data from com port
 *
 */

#ifndef SERIALPORTWORKER_H
#define SERIALPORTWORKER_H

#include <QString>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

class SerialPortWorker : public QObject
{
    Q_OBJECT
public:
    SerialPortWorker(QSerialPortInfo pinfo);
    SerialPortWorker(QSerialPortInfo pinfo, QSerialPort::BaudRate br, QSerialPort::DataBits db, QSerialPort::StopBits sb, QSerialPort::Parity p, QSerialPort::FlowControl fc);
    SerialPortWorker(QSerialPortInfo pinfo, int baudRate, int dataBits, int stopBits, int parity, int flowControl);
    ~SerialPortWorker();
    void closePort();
    void sendData(QString data);

signals:
    void dataReceived(QString data);
    void started();
    void stopped();

    void incPacketsReceived();
    void incPacketsSent();
public slots:
    void doWork();

private:
    QSerialPort *port;
    bool stop;
    bool needToSendData;
    QString data;

    void sendData();
};

#endif // SERIALPORTDATAMANAGER_H
