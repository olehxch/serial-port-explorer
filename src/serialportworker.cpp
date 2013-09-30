#include "serialportworker.h"

/*
 * Standart constructor. All settings are default
 */
SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo):stop(false)
{
    port = new QSerialPort(pinfo);
}

/*
 * Constructor with custom options for com port
 * Not used now
 */
SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo, QSerialPort::BaudRate br, QSerialPort::DataBits db, QSerialPort::StopBits sb, QSerialPort::Parity p, QSerialPort::FlowControl fc = QSerialPort::NoFlowControl):stop(false), needToSendData(false)
{
    port = new QSerialPort(pinfo);
    port->setBaudRate(br);
    port->setStopBits(sb);
    port->setDataBits(db);
    port->setParity(p);
    port->setFlowControl(fc);
}

SerialPortWorker::SerialPortWorker(QSerialPortInfo pinfo, int baudRate, int dataBits, int stopBits, int parity, int flowControl):stop(false)
{
    port = new QSerialPort(pinfo);
    port->setBaudRate(baudRate);
    port->setStopBits( static_cast<QSerialPort::StopBits>(stopBits) );
    port->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    port->setParity( static_cast<QSerialPort::Parity>(parity)) ;
    port->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));
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
 * Send data to device via opened COM port
 */
void SerialPortWorker::sendData(QString data)
{
    if(data != "") {
        needToSendData = true;
        this->data = data;
    }
}

/*
 * This thread is only reading data from com port and sends signal when a line is read
 */
void SerialPortWorker::doWork()
{
    emit started();
    port->open(QIODevice::ReadWrite);

    char* c = new char[128];
    QString res = "";
    while(1) {
        if(port->isOpen() && !stop) {
            if ((port->bytesAvailable() > 0) || port->waitForReadyRead(500)) {
                if(res.indexOf( tr("\n") )>=0 ) {
                    if(res.count() != 0) {
                        emit dataReceived( res );
                        emit incPacketsReceived();
                        res = "";
                    }
                } else {
                    port->readLine(c, 128);
                    res += QString(c);
                }
            }

            // if data need to be send
            if(needToSendData) {
                sendData();
                needToSendData = false;
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

void SerialPortWorker::sendData()
{
    if( port->write(data.toLatin1()) > -1) {
        emit dataReceived(data);
        emit incPacketsSent();
    }else {
        emit dataReceived("Error with sending data");
    }
}
