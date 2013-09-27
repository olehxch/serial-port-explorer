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
    SerialPortWorker();
    SerialPortWorker(QSerialPortInfo _pinfo);
    ~SerialPortWorker();

    void updatePortInfo();
signals:
    void dataReceived(QString data);
    void availablePorts(QList<QSerialPortInfo> p);

public slots:
    void doWork();
    void closePort();
    void updateAvailablePorts();

private:
    QSerialPort *port;
    QSerialPortInfo *pinfo;
};

#endif // SERIALPORTDATAMANAGER_H
