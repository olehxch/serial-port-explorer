#include "serialportworker.h"

/*
 * Standart constructor. All vsettings are default
 */
SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo):stop(false)
{
    port = new QSerialPort(pinfo);
}

/*
 * Constructor with custom options for com port
 */
SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo, QSerialPort::BaudRate br, QSerialPort::DataBits db, QSerialPort::StopBits sb, QSerialPort::Parity p, QSerialPort::FlowControl fc = QSerialPort::NoFlowControl):stop(false)
{
    port = new QSerialPort(pinfo);
    port->setBaudRate(br);
    port->setStopBits(sb);
    port->setDataBits(db);
    port->setParity(p);
    port->setFlowControl(fc);
}

SerialPortWorker::~SerialPortWorker() {
    delete port;
}

/*
 * Closes active port
 */
void SerialPortWorker::closePort()
{
    this->stop = true;
}

/*
 * This thread is only reading data from com port and sends signal when a line is read
 */
void SerialPortWorker::doWork()
{
    emit started();
    port->open(QIODevice::ReadOnly);
    /**/

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
            emit dataReceived( "-----------------------------------" );
            emit stopped();
            delete c;
            return;
        }
    }
}
