#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialportworker.h"


/*
 * Initialize application, find available com ports and fill box info
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentPortInfo = new QSerialPortInfo;
    ports = new QList<QSerialPortInfo>();
    *ports = QSerialPortInfo::availablePorts();

    if(ports->isEmpty()) {
        ui->comPortEnum->clear();
        ui->comPortEnum->addItem("None");
    } else {
        foreach(*currentPortInfo, *ports) {
            ui->comPortEnum->addItem(currentPortInfo->description());
        }
    }

    updatePort();
    current = new QSerialPort(*currentPortInfo);

    // fill baud rate
    ui->comPortSpeed->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->comPortSpeed->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->comPortSpeed->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->comPortSpeed->addItem(QLatin1String("115200"), QSerialPort::Baud115200);

    // fill data bits
    ui->comPortDataBits->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->comPortDataBits->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->comPortDataBits->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->comPortDataBits->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->comPortDataBits->setCurrentIndex(3);

    // fill parity
    ui->comPortParityBit->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->comPortParityBit->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->comPortParityBit->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->comPortParityBit->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->comPortParityBit->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);

    // fill stop bits
    ui->comPortStopBits->addItem(QLatin1String("1"), QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->comPortStopBits->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
    #endif
    ui->comPortStopBits->addItem(QLatin1String("2"), QSerialPort::TwoStop);
}

MainWindow::~MainWindow()
{
    if(current->isOpen())
        current->close();

    delete current;
    delete ports;
    delete ui;
}

/*
 * Updates ports in a new thread
 */
void MainWindow::updatePort()
{
    //qRegisterMetaType< QList<QSerialPortInfo> >( "QList<QSerialPortInfo>" );

    QThread *wthread = new QThread;
    SerialPortWorker *pw = new SerialPortWorker();
    connect(this, &MainWindow::startWorker, pw, &SerialPortWorker::updateAvailablePorts);
    connect(pw, &SerialPortWorker::availablePorts, this, &MainWindow::availablePorts);
    connect(wthread, &QThread::finished, pw, &QObject::deleteLater);
    pw->moveToThread(wthread);
    wthread->start();
    emit startWorker();
}

void MainWindow::openCurrentPort(int index)
{
    if(!ports->isEmpty()) {
        QSerialPortInfo info = ports->at(index);

        // update current active port
        current = new QSerialPort(info);

        /*if(current->isOpen()) {
            statusBar()->showMessage(tr("Port opened: ") + current->portName());
        } else {
            statusBar()->showMessage(tr("Port open error: ") + current->portName());
        }*/

        QThread *wthread = new QThread;
        SerialPortWorker *pw = new SerialPortWorker(info);

        connect(pw, &SerialPortWorker::dataReceived, this, &MainWindow::updateDataBox );
        connect(this, &MainWindow::startWorker, pw, &SerialPortWorker::doWork);
        connect(this, &MainWindow::closeCurrentPort, pw, &SerialPortWorker::closePort);
        connect(wthread, &QThread::finished, pw, &QObject::deleteLater);

        pw->moveToThread(wthread);
        wthread->start();

        emit startWorker();
        statusBar()->showMessage(tr("Port opened: ") + current->portName());
    }
}

void MainWindow::updateCurrentPortInfo()
{
    if(!ports->isEmpty()) {

        QSerialPortInfo info = ports->at(ui->comPortEnum->currentIndex());

        // update info
        ui->comPortName->setText( QString("Port: ") + info.portName() );
        ui->comPortDescription->setText( QString("Description: ") + info.description() );
        ui->comPortIsBusy->setText( QString("Busy: ") + (info.isBusy() ? QString("Yes") : QString("No")) );
        ui->comPortProductId->setText( QString("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) );
        ui->comPortVendorId->setText( QString("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) );
        ui->comPortSystemLocation->setText( QString("Location: ") + info.systemLocation() );
        ui->comPortManufacturer->setText( QString("Manufacturer: ") + info.manufacturer() );
    } else {
        ui->comPortName->setText( QString("Port: ") );
        ui->comPortDescription->setText( QString("Description: ") );
        ui->comPortIsBusy->setText( QString("Busy: ") );
        ui->comPortProductId->setText( QString("Product Identifier: ") );
        ui->comPortVendorId->setText( QString("Vendor Identifier: ") );
        ui->comPortSystemLocation->setText( QString("Location: ") );
        ui->comPortManufacturer->setText( QString("Manufacturer: ") );
    }

}

/*
 * Update Info box when chosen com port is changed
 */
void MainWindow::on_comPortEnum_currentIndexChanged(int index)
{
    QSerialPortInfo info = ports->at(index);

    // update info
    ui->comPortName->setText( QString("Port: ") + info.portName() );
    ui->comPortDescription->setText( QString("Description: ") + info.description() );
    ui->comPortIsBusy->setText( QString("Busy: ") + (info.isBusy() ? QString("Yes") : QString("No")) );
    ui->comPortProductId->setText( QString("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) );
    ui->comPortVendorId->setText( QString("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) );
    ui->comPortSystemLocation->setText( QString("Location: ") + info.systemLocation() );
    ui->comPortManufacturer->setText( QString("Manufacturer: ") + info.manufacturer() );

}

/*
 * Updates data box and shows new received data from com port
 */
void MainWindow::updateDataBox(QString data)
{
    if(data != "") {
        data = data.trimmed();
        ui->receivedDataBox->append(QTime::currentTime().toString() + ": " + data);
    }
}

void MainWindow::availablePorts(QList<QSerialPortInfo> p)
{
    *ports = p;
    if(ports->isEmpty()) {
        ui->comPortEnum->clear();
        ui->comPortEnum->addItem("None");
    } else {
        ui->comPortEnum->clear();
        foreach(*currentPortInfo, *ports) {
            ui->comPortEnum->addItem(currentPortInfo->description());
        }
    }
}

void MainWindow::on_openPortButton_clicked()
{
    openCurrentPort(ui->comPortEnum->currentIndex());
}

void MainWindow::on_ClosePortButton_clicked()
{
    if(current->isOpen()) {
        emit closeCurrentPort();
        statusBar()->showMessage(tr("Port closed: ") + current->portName());
    }
}
