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
    m_pFileSystemWatcher = 0;

    m_pSettings = 0;
    m_useCustomJavaPath = false;
    m_mcServerPath = "";
    m_customJavaPath = "";
    m_xms = 512;
    m_xmx = 512;
    m_additionalParameters = "";
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

QString MainWindow::htmlColor(const QString &msg, const QString &color)
{
    return QString("<font color=\"%1\">%2</font>").arg(color).arg(msg);
}

QString MainWindow::htmlBlue(const QString &msg)
{
    return htmlColor(msg, "blue");
}

QString MainWindow::htmlRed(const QString &msg)
{
    return htmlColor(msg, "red");
}

QString MainWindow::htmlGreen(const QString &msg)
{
    return htmlColor(msg, "green");
}

QString MainWindow::htmlPurple(const QString &msg)
{
    return htmlColor(msg, "purple");
}

void MainWindow::initialize()
{
    createActions();
    createTrayIcon();
    setIcon();

    statusLabel = new QLabel;
    if(statusLabel)
    {
        statusBar()->addWidget(statusLabel);
        statusLabel->setText(tr("Minecraft Server Status: Stopped"));
    }

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionSettings->setEnabled(true);
    ui->serverPropertiesTextEdit->setEnabled(true);
    ui->sendCommandButton->setEnabled(false);
    ui->actionSaveServerProperties->setEnabled(false);

    if(trayIcon)
    {
        trayIcon->show();
    }

    m_pServerProcess = new QProcess(this);

    connect( m_pServerProcess, SIGNAL(started()), SLOT(onStart()) );
    connect( m_pServerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onFinish(int,QProcess::ExitStatus)) );
    connect( m_pServerProcess, SIGNAL(readyReadStandardOutput()), SLOT(onStandardOutput()) );
    connect( m_pServerProcess, SIGNAL(readyReadStandardError()), SLOT(onStandardError()) );

    m_pFileSystemWatcher = new QFileSystemWatcher(this);
    connect( m_pFileSystemWatcher, SIGNAL(fileChanged(QString)), SLOT(onWatchedFileChanged(QString)) );

    m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Qt Minecraft Server", "qtmcserver", this);

    loadSettings();

    if(m_mcServerPath.isEmpty())
    {
        on_actionSettings_triggered();
    }

    loadServerProperties();

    if(!m_mcServerPath.isEmpty())
    {
        updateWatchedFileSystemPath("", getMinecraftServerPropertiesPath(m_mcServerPath));
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
        m_xms =  m_pSettings->value("Settings/Xms", "512").toInt();
        m_xmx =  m_pSettings->value("Settings/Xmx", "512").toInt();
        m_additionalParameters = m_pSettings->value("Settings/AdditionalParameters", "").toString();
    }
}

void MainWindow::saveSettings()
{
    if(m_pSettings)
    {
        m_pSettings->setValue("Settings/UseCustomJavaPath", m_useCustomJavaPath ? "yes" : "no");
        m_pSettings->setValue("Settings/CustomJavaPath", m_customJavaPath);
        m_pSettings->setValue("Settings/MinecraftServerPath", m_mcServerPath);
        m_pSettings->setValue("Settings/Xms", m_xms);
        m_pSettings->setValue("Settings/Xmx", m_xmx);
        m_pSettings->setValue("Settings/AdditionalParameters", m_additionalParameters);
    }
}

void MainWindow::loadServerProperties()
{
    if(!m_mcServerPath.isEmpty())
    {
        if(m_pServerProcess && (m_pServerProcess->state() == QProcess::NotRunning))
        {
            ui->actionSaveServerProperties->setEnabled(true);
        }

        ui->serverPropertiesTextEdit->clear();

        QFile file(getMinecraftServerPropertiesPath(m_mcServerPath));
        if(!file.open(QIODevice::ReadOnly))
        {
            return;
        }

        QTextStream in(&file);

        QString contents = in.readAll();
        ui->serverPropertiesTextEdit->setText(contents);

        file.close();
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
            trayIcon->setToolTip("Qt Minecraft Server");
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
        settingsDlg->setXms(m_xms);
        settingsDlg->setXmx(m_xmx);
        settingsDlg->setAdditionalParameters(m_additionalParameters);

        settingsDlg->initialize();

        if(settingsDlg->exec() == QDialog::Accepted)
        {
            updateWatchedFileSystemPath( getMinecraftServerPropertiesPath(m_mcServerPath),
                                         getMinecraftServerPropertiesPath(settingsDlg->getMinecraftServerPath()));

            m_mcServerPath = settingsDlg->getMinecraftServerPath();
            m_customJavaPath = settingsDlg->getCustomJavaPath();
            m_useCustomJavaPath = settingsDlg->useCustomJavaPath();
            m_xms = settingsDlg->getXms();
            m_xmx = settingsDlg->getXmx();
            m_additionalParameters = settingsDlg->getAdditionalParameters();

            loadServerProperties();
        }

        delete settingsDlg;
        settingsDlg = 0;
    }
}

QString MainWindow::getMinecraftServerPropertiesPath(const QString& mcServerPath)
{
    QString mcServerPropertiesPath = "";

    if(!mcServerPath.isEmpty())
    {
        QFileInfo mcServerFileInfo = QFileInfo(mcServerPath);
        QString workingDir = mcServerFileInfo.absolutePath();

        mcServerPropertiesPath = workingDir + QString("/server.properties");
    }

    return mcServerPropertiesPath;
}

void MainWindow::updateWatchedFileSystemPath(const QString& oldPath, const QString& newPath)
{
    if(m_pFileSystemWatcher)
    {
        if(!oldPath.isEmpty())
        {
            m_pFileSystemWatcher->removePath(oldPath);
        }

        if(!newPath.isEmpty())
        {
            if(QFile::exists(newPath))
            {
                m_pFileSystemWatcher->addPath(newPath);
            }
        }
    }
}

void MainWindow::on_actionStart_triggered()
{
    if(m_mcServerPath.isEmpty())
    {
        QMessageBox::information(this, tr("Qt Minecraft Server"),
                                 tr("No Minecraft Server File available!\nPlease select a Minecraft Server File at Qt Minecraft Server Settings."));

        on_actionSettings_triggered();

        return;
    }

    if(m_pServerProcess)
    {
        QFileInfo mcServerFileInfo = QFileInfo(m_mcServerPath);
        QString workingDir = mcServerFileInfo.absolutePath();
        QString mcServerFile = mcServerFileInfo.fileName();

        m_pServerProcess->setWorkingDirectory(workingDir);

        QStringList arguments;

        if(m_xms > 0)
        {
            arguments.append(QString("-Xms%1M").arg(QString::number(m_xms)));
        }

        if(m_xmx > 0)
        {
            arguments.append(QString("-Xmx%1M").arg(QString::number(m_xmx)));
        }

        arguments.append("-jar");
        arguments.append(mcServerFile);
        arguments.append("nogui");

        if(!m_additionalParameters.isEmpty())
        {
            arguments.append(m_additionalParameters);
        }

        on_actionSaveServerProperties_triggered();

        ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; Starting Java VM in Working Directory: %1...")
                                               .arg(QDir::toNativeSeparators(workingDir))));

        if(m_useCustomJavaPath)
        {
            ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; %1 %2").arg(QDir::toNativeSeparators(m_customJavaPath))
                                                   .arg(arguments.join(" "))));

            m_pServerProcess->start(m_customJavaPath, arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);
        }
        else
        {
            ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; java %1").arg(arguments.join(" "))));
            m_pServerProcess->start("java", arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);
        }

        if(!m_pServerProcess->waitForStarted())
        {
            ui->serverLogTextEdit->append(htmlRed(tr("&gt;&gt; Unable to start Java VM.")));
        }
    }
}

void MainWindow::onStart()
{
    ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; Starting Minecraft Server...")));

    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
    ui->actionSettings->setEnabled(false);
    ui->serverPropertiesTextEdit->setEnabled(false);
    ui->actionSaveServerProperties->setEnabled(false);

    statusLabel->setText(tr("Minecraft Server Status: Started"));
}

void MainWindow::onFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    if((exitStatus == QProcess::NormalExit) && (exitCode ==  0))
    {
        ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; Minecraft Server stopped normally with exit code: %1").arg(exitCode)));
    }
    else if((exitStatus == QProcess::NormalExit) && (exitCode ==  1))
    {
        ui->serverLogTextEdit->append(htmlRed(tr("&gt;&gt; Minecraft Server killed and exited with exit code: %1").arg(exitCode)));
    }
    else if(exitStatus == QProcess::CrashExit)
    {
        ui->serverLogTextEdit->append(htmlRed(tr("&gt;&gt; Minecraft Server crashed!")));
    }

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionSettings->setEnabled(true);
    ui->serverPropertiesTextEdit->setEnabled(true);
    ui->actionSaveServerProperties->setEnabled(true);

    statusLabel->setText(tr("Minecraft Server Status: Stopped"));
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
        {
            ui->serverLogTextEdit->append(str);
        }
    }
}

void MainWindow::onWatchedFileChanged(const QString &path)
{
    if(!m_mcServerPath.isEmpty())
    {
        if(path == getMinecraftServerPropertiesPath(m_mcServerPath))
        {
            loadServerProperties();
        }
    }
}

void MainWindow::on_actionStop_triggered()
{
    if(m_pServerProcess)
    {
        if(m_pServerProcess->state() == QProcess::Running)
        {
            ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; Stopping Minecraft Server...")));

            if(m_pServerProcess->isWritable())
            {
                QByteArray command = (QString("stop") + QString("\n")).toAscii();

                m_pServerProcess->write(command);
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
                ui->serverLogTextEdit->append(htmlGreen(QString("&lt;&lt; ") + ui->serverCommandLineEdit->text()));

                if(ui->serverCommandLineEdit->text().trimmed() == "stop")
                {
                    ui->serverLogTextEdit->append(htmlBlue(tr("&gt;&gt; Stopping Minecraft Server...")));
                }

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

void MainWindow::on_serverCommandLineEdit_textEdited(const QString &text)
{
    ui->sendCommandButton->setEnabled(!text.isEmpty());
}

void MainWindow::on_actionSaveServerProperties_triggered()
{
    if(!m_mcServerPath.isEmpty())
    {
        QFileInfo mcServerFileInfo = QFileInfo(m_mcServerPath);
        QString workingDir = mcServerFileInfo.absolutePath();

        QFile outfile;
        outfile.setFileName(workingDir + QString("/server.properties"));

        if(outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&outfile);
            out << ui->serverPropertiesTextEdit->toPlainText();

            outfile.close();
        }
    }
}

void MainWindow::on_actionRefreshServerProperties_triggered()
{
    QString mcServerPropertiesPath = getMinecraftServerPropertiesPath(m_mcServerPath);
    updateWatchedFileSystemPath(mcServerPropertiesPath, mcServerPropertiesPath);

    loadServerProperties();
}
