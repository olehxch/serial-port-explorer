#include "updateserialportinfo.h"

UpdateSerialPortInfo::UpdateSerialPortInfo():stopWork(false)
{
    recent = new QList<QSerialPortInfo>;
    old = new QList<QSerialPortInfo>;
    *old = QSerialPortInfo::availablePorts();
}

UpdateSerialPortInfo::~UpdateSerialPortInfo()
{
    delete old;
    delete recent;
}

/*
 * Stop this thread and finish it
 */
void UpdateSerialPortInfo::stop()
{
    this->stopWork = true;
}

/*
 * Update list of available connected devices via COM port every 500 ms
 * and send signal with data to update GUI
 */
void UpdateSerialPortInfo::doWork()
{
    while(!stopWork) {
       *recent = QSerialPortInfo::availablePorts();
        if(compareQLists()) {
            emit updated(*recent);
            *old = *recent;
        }
        QThread::msleep(500);
    }
}

/*
 * Compares 2 lists of available devices
 * if they are different(if new devices are connected -> return true) - send signal to update GUI
 * else return false
 */
bool UpdateSerialPortInfo::compareQLists()
{
    // check if empty
    if(recent->empty() || old->empty())
        return true;

    // check length
    if(recent->length() != old->length())
        return true;

    if( recent->length() > 0 && old->length() > 0) {
        // check lengths
        if(recent->length() == old->length()) {
            // if lengths are equal, compare inner elements
            for(int i = 0;i<recent->length(); i++) {
                QString r_name = recent->at(i).portName();
                QString o_name = old->at(i).portName();
                if( r_name.compare(o_name) != 0)
                    return true;
            }
        }
    }
    return false;
}
