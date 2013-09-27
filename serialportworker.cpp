#include "serialportworker.h"

SerialPortWorker::SerialPortWorker()
{
}

SerialPortWorker::SerialPortWorker(QSerialPortInfo _pinfo)
{
    this->pinfo = new QSerialPortInfo(_pinfo);
    port = new QSerialPort(*this->pinfo);
}

SerialPortWorker::~SerialPortWorker() {
    delete port;
}

void SerialPortWorker::updatePortInfo()
{

}

void SerialPortWorker::doWork()
{
    port->open(QIODevice::ReadWrite);
    port->setPortName(pinfo->portName());
    port->setBaudRate(QSerialPort::Baud115200);
    port->setStopBits(QSerialPort::OneStop);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    char* c = new char[128];
    QString res = "";
    while(1) {
        if(port->isOpen()) {
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
        }
    }
}

void SerialPortWorker::closePort()
{
    if(port->isOpen())
        port->close();
}

void SerialPortWorker::updateAvailablePorts()
{
    while(1) {
        QList<QSerialPortInfo> p = QSerialPortInfo::availablePorts();

        QThread::sleep(500);
        emit availablePorts(p);
    }
}
