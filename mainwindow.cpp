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
    ports = new QList<QSerialPortInfo>(QSerialPortInfo::availablePorts());

    // fill all available ports
    if(ports->isEmpty()) {
        ui->comPortEnum->addItem("None");
        ui->openPortButton->setEnabled(false);
        ui->ClosePortButton->setEnabled(false);
    } else {
        foreach(*currentPortInfo, *ports) {
            ui->comPortEnum->addItem(currentPortInfo->description());
        }
        ui->openPortButton->setEnabled(true);
        ui->ClosePortButton->setEnabled(false);
    }

    // fill baud rate
    ui->comPortBaudRate->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->comPortBaudRate->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->comPortBaudRate->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->comPortBaudRate->addItem(QLatin1String("115200"), QSerialPort::Baud115200);

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
    delete wthread;
    delete pw;
    delete currentPortInfo;
    delete ports;
    delete ui;
}

/*
 * Updates ports in a new thread
 */
void MainWindow::updatePort()
{
    /*QThread *wthread = new QThread;
    SerialPortWorker *pw = new SerialPortWorker();
    connect(this, &MainWindow::startWorker, pw, &SerialPortWorker::updateAvailablePorts);
    connect(wthread, &QThread::finished, pw, &QObject::deleteLater);
    pw->moveToThread(wthread);
    wthread->start();
    emit startWorker();*/
}

// TODO delete, unused
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
    if(!ports->isEmpty()) {
        QSerialPortInfo info = ports->at(index);

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
    // open new port
    if(!ports->isEmpty()) {
        *currentPortInfo = ports->at(ui->comPortEnum->currentIndex());

        // update current active port
        wthread = new QThread;
       /* QSerialPort::DataBits dataBits = QVariant::convert(QSerialPort::DataBits, ui->comPortDataBits->itemData(ui->comPortDataBits->currentIndex()) );
        QSerialPort::Parity parityBit = ui->comPortParityBit->itemData(ui->comPortParityBit->currentIndex());
        QSerialPort::BaudRate baudRate = ui->comPortBaudRate->itemData(ui->comPortBaudRate->currentIndex());
        QSerialPort::StopBits stopBits = ui->comPortStopBits->itemData(ui->comPortStopBits->currentIndex());

        QVariant dataBits = ui->comPortDataBits->itemData(ui->comPortDataBits->currentIndex(), QSerialPort::DataBits);
        QVariant parityBit = ui->comPortParityBit->itemData(ui->comPortParityBit->currentIndex());
        QVariant baudRate = ui->comPortBaudRate->itemData(ui->comPortBaudRate->currentIndex());
        QVariant stopBits = ui->comPortStopBits->itemData(ui->comPortStopBits->currentIndex());
        */
        //pw = new SerialPortWorker(*currentPortInfo, baudRate.convert(), dataBits, stopBits, parityBit);
        pw = new SerialPortWorker(*currentPortInfo);

        connect(pw, &SerialPortWorker::dataReceived, this, &MainWindow::updateDataBox );
        connect(wthread, &QThread::started, pw, &SerialPortWorker::doWork);
        connect(wthread, &QThread::finished, pw, &QObject::deleteLater);

        // connect to gui
        connect(pw, &SerialPortWorker::started, this, &MainWindow::workerStarted);
        connect(pw, &SerialPortWorker::stopped, this, &MainWindow::workerStopped);

        pw->moveToThread(wthread);
        wthread->start();
    }
}

void MainWindow::on_ClosePortButton_clicked()
{
    pw->closePort();
}

/*
 * Deactivate "Open port" button
 */
void MainWindow::workerStarted()
{
    // disable comboboxes with settings
    ui->comPortDataBits->setEnabled(false);
    ui->comPortParityBit->setEnabled(false);
    ui->comPortStopBits->setEnabled(false);
    ui->comPortBaudRate->setEnabled(false);

    ui->openPortButton->setEnabled(false);
    ui->ClosePortButton->setEnabled(true);
    statusBar()->showMessage(tr("Port opened: ") + currentPortInfo->portName());
}

/*
 * Enable "Open port" button
 */
void MainWindow::workerStopped()
{
    // disable comboboxes with settings
    ui->comPortDataBits->setEnabled(true);
    ui->comPortParityBit->setEnabled(true);
    ui->comPortStopBits->setEnabled(true);
    ui->comPortBaudRate->setEnabled(true);

    ui->openPortButton->setEnabled(true);
    ui->ClosePortButton->setEnabled(false);
    statusBar()->showMessage(tr("Port closed: ") + currentPortInfo->portName());
}
