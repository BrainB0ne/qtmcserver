/*
 * Qt Minecraft Server
 * Copyleft 2013
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bTrayWarningShowed = false;

    minimizeAction = 0;
    maximizeAction = 0;
    restoreAction = 0;
    quitAction = 0;
    startServerAction = 0;
    stopServerAction = 0;
    trayIcon = 0;
    trayIconMenu = 0;

    m_pServerProcess = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{    
    createActions();
    createTrayIcon();
    setIcon();

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);

    if(trayIcon)
    {
        trayIcon->show();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon && trayIcon->isVisible())
    {
        if(!m_bTrayWarningShowed)
        {
            QMessageBox::information(this, tr("Qt Minecraft Server"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Exit</b> in the context menu "
                                    "of the system tray entry."));
            m_bTrayWarningShowed = true;
        }

        hide();
        event->ignore();
    }
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("E&xit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    startServerAction = new QAction(tr("&Start Server"), this);
    connect(startServerAction, SIGNAL(triggered()), this, SLOT(on_actionStart_triggered()));

    stopServerAction = new QAction(tr("Sto&p Server"), this);
    connect(stopServerAction, SIGNAL(triggered()), this, SLOT(on_actionStop_triggered()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);

    if(trayIconMenu)
    {
        trayIconMenu->addAction(minimizeAction);
        trayIconMenu->addAction(maximizeAction);
        trayIconMenu->addAction(restoreAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(startServerAction);
        trayIconMenu->addAction(stopServerAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        trayIcon = new QSystemTrayIcon(this);

        if(trayIcon)
        {
            trayIcon->setContextMenu(trayIconMenu);
        }
    }
}

void MainWindow::setIcon()
{
    QIcon icon = QIcon("://images/qtmcserver-16x16.png");
    trayIcon->setIcon(icon);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            showNormal();
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            ;
    }
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog* aboutDlg = new AboutDialog(this);

    if(aboutDlg)
    {
        aboutDlg->initialize();
        aboutDlg->exec();

        delete aboutDlg;
        aboutDlg = 0;
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog* settingsDlg = new SettingsDialog(this);

    if(settingsDlg)
    {
        settingsDlg->exec();

        delete settingsDlg;
        settingsDlg = 0;
    }
}

void MainWindow::on_actionStart_triggered()
{
    if(!m_pServerProcess)
    {
        m_pServerProcess = new QProcess(this);

        connect( m_pServerProcess, SIGNAL(started()), SLOT(onStart()) );
        connect( m_pServerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onFinish(int,QProcess::ExitStatus)) );
        connect( m_pServerProcess, SIGNAL(readyReadStandardOutput()), SLOT(onStandardOutput()) );
        connect( m_pServerProcess, SIGNAL(readyReadStandardError()), SLOT(onStandardError()) );

        if(m_pServerProcess)
        {
            QStringList arguments;
            arguments << "-Xms1024M" << "-Xmx1024M" << "-jar" << "minecraft_server.jar" << "nogui";

            m_pServerProcess->start("java", arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);
            m_pServerProcess->waitForStarted();
        }
    }
}

void MainWindow::onStart()
{
    ui->serverLogTextEdit->append(tr("Starting Minecraft Server..."));

    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
}

void MainWindow::onFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        ui->serverLogTextEdit->append(tr("Minecraft Server closed normally with exit code: %1").arg(exitCode));
    }
    else if(exitStatus == QProcess::CrashExit)
    {
        ui->serverLogTextEdit->append(tr("Minecraft Server exited abnormally"));
    }

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);

    m_pServerProcess->deleteLater();
    m_pServerProcess = 0;
}

void MainWindow::onStandardOutput()
{
    QByteArray ba;
    QString str;

    while (m_pServerProcess->canReadLine())
    {
        str = m_pServerProcess->readLine();
        if(!str.trimmed().isEmpty()) ui->serverLogTextEdit->append(str.trimmed());
    }

    ba = m_pServerProcess->readAllStandardOutput();

    if (!ba.isEmpty())
    {
        str = QString( ba );
        if(!str.trimmed().isEmpty()) ui->serverLogTextEdit->append(str.trimmed());
    }
}

void MainWindow::onStandardError()
{
    QByteArray ba;
    QString str;

    while (m_pServerProcess->canReadLine())
    {
        str = m_pServerProcess->readLine();
        if(!str.trimmed().isEmpty()) ui->serverLogTextEdit->append(str.trimmed());
    }

    ba = m_pServerProcess->readAllStandardError();

    if (!ba.isEmpty())
    {
        str = QString( ba );
        if(!str.trimmed().isEmpty()) ui->serverLogTextEdit->append(str.trimmed());
    }
}

void MainWindow::on_actionStop_triggered()
{
    if(m_pServerProcess)
    {
        if(m_pServerProcess->state() == QProcess::Running)
        {
            if(m_pServerProcess->isWritable())
            {
                m_pServerProcess->write("stop\n");
                m_pServerProcess->waitForBytesWritten();
            }
        }
    }
}
