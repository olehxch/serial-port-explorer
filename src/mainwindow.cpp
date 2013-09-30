#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialportworker.h"

/*
 * Initialize application, find available com ports and fill box info
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), packetsSent(0), packetsReceived(0)
{
    ui->setupUi(this);
    currentPortInfo = new QSerialPortInfo;
    ports = new QList<QSerialPortInfo>(QSerialPortInfo::availablePorts());

    // fill all available ports
    if(ports->isEmpty()) {
        ui->comPortEnum->addItem("None");
        ui->openPortButton->setEnabled(false);
        ui->ClosePortButton->setEnabled(false);
        ui->sendData->setEnabled(false);
    } else {
        foreach(*currentPortInfo, *ports) {
            ui->comPortEnum->addItem(currentPortInfo->description());
        }
        ui->openPortButton->setEnabled(true);
        ui->ClosePortButton->setEnabled(false);
        ui->sendData->setEnabled(false);
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

    // fill flow control
    ui->comPortFlowControl->addItem(QLatin1String("NoFlowControl"), QSerialPort::NoFlowControl);
    ui->comPortFlowControl->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comPortFlowControl->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);

    // run thread that updates connected COM port devices in background worker thread
    updateCurrentPortInfo();
}

MainWindow::~MainWindow()
{
    sinfo->stop();
    delete currentPortInfo;
    delete ports;
    delete ui;
}

/*
 * Run thread, that every 500ms updates info about available com ports
 * and sends dignal to update GUI
 */
void MainWindow::updateCurrentPortInfo()
{
    qRegisterMetaType< QList<QSerialPortInfo> >("QList<QSerialPortInfo>");
    QThread *updatePortInfoThread = new QThread;
    sinfo = new UpdateSerialPortInfo();

    connect(sinfo, &UpdateSerialPortInfo::updated, this, &MainWindow::availablePorts);
    connect(updatePortInfoThread, &QThread::started, sinfo, &UpdateSerialPortInfo::doWork);
    connect(updatePortInfoThread, &QThread::finished, sinfo, &QObject::deleteLater);

    sinfo->moveToThread(updatePortInfoThread);
    updatePortInfoThread->start();
}

/*
 * Update label 'Read/write statistics'
 */
void MainWindow::updateDataStatistics()
{
    ui->readWriteStat->setText("Read/Write: " + QString::number(packetsReceived) + "/" + QString::number(packetsSent) + " packets");
}

void MainWindow::incPacketsSent()
{
    ++packetsSent;
    updateDataStatistics();
}

void MainWindow::incPacketReceived()
{
    ++packetsReceived;
    updateDataStatistics();
}

void MainWindow::clearPacketStatistics()
{
    packetsReceived = 0;
    packetsSent = 0;
    updateDataStatistics();
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
 * Updates status bar with string
 */
void MainWindow::updateStatusBar(QString s)
{
    statusBar()->showMessage(s);
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

/*
 * Update all available connected devices to ports
 */
void MainWindow::availablePorts(QList<QSerialPortInfo> p)
{    
    *ports = p;
    if(ports->isEmpty()) {
        ui->comPortEnum->clear();
        ui->comPortEnum->addItem("None");
    } else {
        ui->comPortEnum->blockSignals(true);
        ui->comPortEnum->clear();
        ui->comPortEnum->blockSignals(false);
        foreach(*currentPortInfo, *ports) {
            ui->comPortEnum->addItem(currentPortInfo->description());
        }
    }
}

/*
 * Open specific port to read/write data
 */
void MainWindow::on_openPortButton_clicked()
{
    clearPacketStatistics();
    // open new port
    if(!ports->isEmpty()) {
        *currentPortInfo = ports->at(ui->comPortEnum->currentIndex());

        // update current active port
        wthread = new QThread;

        int dataBits = ui->comPortDataBits->itemData(ui->comPortDataBits->currentIndex()).toInt();
        int parityBit = ui->comPortParityBit->itemData(ui->comPortParityBit->currentIndex()).toInt();
        int baudRate = ui->comPortBaudRate->itemData(ui->comPortBaudRate->currentIndex()).toInt();
        int stopBits = ui->comPortStopBits->itemData(ui->comPortStopBits->currentIndex()).toInt();
        int flowControl = ui->comPortFlowControl->itemData(ui->comPortFlowControl->currentIndex()).toInt();
        pw = new SerialPortWorker(*currentPortInfo, baudRate, dataBits, stopBits, parityBit, flowControl);

        connect(pw, &SerialPortWorker::incPacketsReceived, this, &MainWindow::incPacketReceived);
        connect(pw, &SerialPortWorker::incPacketsSent, this, &MainWindow::incPacketsSent);
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
    ui->comPortFlowControl->setEnabled(false);

    ui->sendData->setEnabled(true);
    ui->openPortButton->setEnabled(false);
    ui->ClosePortButton->setEnabled(true);
    updateStatusBar(tr("Port opened: ") + currentPortInfo->portName());
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
    ui->comPortFlowControl->setEnabled(true);

    ui->sendData->setEnabled(false);
    ui->openPortButton->setEnabled(true);
    ui->ClosePortButton->setEnabled(false);
    updateStatusBar(tr("Port closed: ") + currentPortInfo->portName());
}

/*
 * Send data
 */
void MainWindow::on_sendData_clicked()
{
    pw->sendData(ui->dataLine->text());
}

/*
 * Show about window
 */
void MainWindow::on_actionAbout_triggered()
{
    QDialog *aboutWindow = new AboutDialog;
    aboutWindow->setModal(true);
    aboutWindow->show();
}

/*
 * Save sent/received data to log file
 */
void MainWindow::on_saveDataToFileButton_clicked()
{
    QString plainText = ui->receivedDataBox->toPlainText();
    if(plainText.compare("") == 0) {
        updateStatusBar("Data box is empty.");
        return;
    }

    QThread *saveLog = new QThread;
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save to file"), "/", tr("Text File (*.txt)"));

    // save data to file
    connect(saveLog, &QThread::started, [=] {
        QFile file(saveFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << plainText;
        file.close();
    });

    saveLog->start();
}
