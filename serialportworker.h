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
    SerialPortWorker(QSerialPortInfo pinfo);
    ~SerialPortWorker();

    void updatePortInfo();
    void closePort();
    void openPort(QSerialPortInfo info);
signals:
    void dataReceived(QString data);
    void availablePorts(QList<QSerialPortInfo> p);

    void started();
    void stopped();

public slots:
    void doWork();
    void updateAvailablePorts();

private:
    QSerialPort *port;

    bool stop;
};

#endif // SERIALPORTDATAMANAGER_H
