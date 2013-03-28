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

#include <QFileDialog>
#include <QTextStream>

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
    trayIcon = 0;
    trayIconMenu = 0;

    m_pServerProcess = 0;

    m_pSettings = 0;
    m_useCustomJavaPath = false;
    m_mcServerPath = "";
    m_customJavaPath = "";
}

MainWindow::~MainWindow()
{
    if(m_pSettings)
    {
        delete m_pSettings;
        m_pSettings = 0;
    }

    delete ui;
}

QString MainWindow::htmlBold(const QString &msg)
{
    return QString("<b>%1</b>").arg(msg);
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

    m_pServerProcess = new QProcess(this);

    connect( m_pServerProcess, SIGNAL(started()), SLOT(onStart()) );
    connect( m_pServerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onFinish(int,QProcess::ExitStatus)) );
    connect( m_pServerProcess, SIGNAL(readyReadStandardOutput()), SLOT(onStandardOutput()) );
    connect( m_pServerProcess, SIGNAL(readyReadStandardError()), SLOT(onStandardError()) );

    m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Qt Minecraft Server", "qtmcserver", this);

    loadSettings();

    if(m_mcServerPath.isEmpty())
    {
        on_actionSettings_triggered();
    }
}

void MainWindow::closeApplication()
{
    saveSettings();
    qApp->quit();
}

void MainWindow::loadSettings()
{
    if(m_pSettings)
    {
        QString strUseCustomJavaPath = m_pSettings->value("Settings/UseCustomJavaPath", "no").toString();
        m_useCustomJavaPath = (strUseCustomJavaPath == "yes") ? true : false;

        m_customJavaPath = m_pSettings->value("Settings/CustomJavaPath", "").toString();
        m_mcServerPath = m_pSettings->value("Settings/MinecraftServerPath", "").toString();
    }
}

void MainWindow::saveSettings()
{
    if(m_pSettings)
    {
        m_pSettings->setValue("Settings/UseCustomJavaPath", m_useCustomJavaPath ? "yes" : "no");
        m_pSettings->setValue("Settings/CustomJavaPath", m_customJavaPath);
        m_pSettings->setValue("Settings/MinecraftServerPath", m_mcServerPath);
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
        trayIconMenu->addAction(ui->actionStart);
        trayIconMenu->addAction(ui->actionStop);
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
    if(m_pServerProcess)
    {
        if(m_pServerProcess->state() == QProcess::Running)
        {
            on_actionStop_triggered();
            m_pServerProcess->waitForFinished();
        }

        if(m_pServerProcess->state() == QProcess::NotRunning)
        {
            closeApplication();
        }
    }
    else
    {
        closeApplication();
    }
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog* settingsDlg = new SettingsDialog(this);

    if(settingsDlg)
    {
        settingsDlg->setUseCustomJavaPath(m_useCustomJavaPath);
        settingsDlg->setCustomJavaPath(m_customJavaPath);
        settingsDlg->setMinecraftServerPath(m_mcServerPath);

        settingsDlg->initialize();

        if(settingsDlg->exec() == QDialog::Accepted)
        {
            m_mcServerPath = settingsDlg->getMinecraftServerPath();
            m_customJavaPath = settingsDlg->getCustomJavaPath();
            m_useCustomJavaPath = settingsDlg->useCustomJavaPath();
        }

        delete settingsDlg;
        settingsDlg = 0;
    }
}

void MainWindow::on_actionStart_triggered()
{
    if(m_pServerProcess)
    {
        QStringList arguments;
        arguments << "-Xms1024M" << "-Xmx1024M" << "-jar" << "minecraft_server.jar" << "nogui";

        m_pServerProcess->start("java", arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);
        m_pServerProcess->waitForStarted();
    }
}

void MainWindow::onStart()
{
    ui->serverLogTextEdit->append(htmlBold(tr("Starting Minecraft Server...")));

    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
}

void MainWindow::onFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
    {
        ui->serverLogTextEdit->append(htmlBold(tr("Minecraft Server closed normally with exit code: %1").arg(exitCode)));
    }
    else if(exitStatus == QProcess::CrashExit)
    {
        ui->serverLogTextEdit->append(htmlBold(tr("Minecraft Server exited abnormally")));
    }

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
}

void MainWindow::onStandardOutput()
{
    QByteArray baOutput = m_pServerProcess->readAllStandardOutput();
    QString str;

    if (!baOutput.isEmpty())
    {
        str = QString(baOutput).trimmed();

        if(!str.isEmpty())
            ui->serverLogTextEdit->append(str);
    }
}

void MainWindow::onStandardError()
{
    QByteArray baError = m_pServerProcess->readAllStandardError();
    QString str;

    if (!baError.isEmpty())
    {
        str = QString(baError).trimmed();

        if(!str.isEmpty())
            ui->serverLogTextEdit->append(str);
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

void MainWindow::on_sendCommandButton_clicked()
{
    if(ui->serverCommandLineEdit->text().isEmpty())
        return;

    if(m_pServerProcess)
    {
        if(m_pServerProcess->state() == QProcess::Running)
        {
            if(m_pServerProcess->isWritable())
            {
                ui->serverLogTextEdit->append(htmlBold(QString(">> ") + ui->serverCommandLineEdit->text()));

                QByteArray command = (ui->serverCommandLineEdit->text() + QString("\n")).toAscii();

                m_pServerProcess->write(command);
                m_pServerProcess->waitForBytesWritten();

                ui->serverCommandLineEdit->clear();
            }
        }
    }
}

void MainWindow::on_serverCommandLineEdit_returnPressed()
{
    on_sendCommandButton_clicked();
}

void MainWindow::on_actionClear_triggered()
{
    ui->serverLogTextEdit->clear();
}

void MainWindow::on_actionExport_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Console Log"),
                               "mcserverlog.txt",
                               tr("Text Files (*.txt)"));
    if(!fileName.isEmpty())
    {
        QFile outfile;
        outfile.setFileName(fileName);

        if(outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&outfile);
            out << ui->serverLogTextEdit->toPlainText() << endl;
            outfile.close();
        }
    }
}
