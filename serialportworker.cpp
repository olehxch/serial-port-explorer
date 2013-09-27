#include "serialportworker.h"

SerialPortWorker::SerialPortWorker() {}

SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo):stop(false)
{
    port = new QSerialPort(pinfo);
}

SerialPortWorker::~SerialPortWorker() {
    delete port;
}

void SerialPortWorker::updatePortInfo()
{
}

/*
 * Closes active port
 */
void SerialPortWorker::closePort()
{
    this->stop = true;
}

/*
 * Opens specific port
 */
void SerialPortWorker::openPort(QSerialPortInfo info)
{
}

void SerialPortWorker::doWork()
{
    emit started();
    port->open(QIODevice::ReadOnly);
    port->setBaudRate(QSerialPort::Baud115200);
    port->setStopBits(QSerialPort::OneStop);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    char* c = new char[128];
    QString res = "";
    while(1) {
        if(port->isOpen() && !stop) {
            if ((port->bytesAvailable() > 0) || port->waitForReadyRead(500)) {
                if(res.indexOf( tr("\n") )>=0 ) {
                    if(res.count() != 0) {
                        emit dataReceived( res );
                        res = "";
                    }
                } else {
                    port->readLine(c, 128);
                    res += QString(c);
                }
            }
        } else {
            if(port->isOpen())
                port->close();
            emit dataReceived( "--------------------------" );
            emit stopped();
            delete c;
            return;
        }
    }
}

void SerialPortWorker::updateAvailablePorts()
{
    while(1) {
        QList<QSerialPortInfo> p = QSerialPortInfo::availablePorts();

        QThread::sleep(500);
        emit availablePorts(p);
    }
}
