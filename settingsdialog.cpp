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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "downloaddialog.h"

#include <QFileDialog>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    m_useCustomJavaPath = false;
    m_mcServerPath = "";
    m_customJavaPath = "";
    m_xms = 512;
    m_xmx = 512;
    m_additionalParameters = "";
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::initialize()
{
    ui->customRadioButton->setChecked(m_useCustomJavaPath);
    ui->customJavaLineEdit->setText(m_customJavaPath);
    ui->mcServerFileLineEdit->setText(m_mcServerPath);
    ui->xmsSpinBox->setValue(m_xms);
    ui->xmxSpinBox->setValue(m_xmx);
    ui->additionalParametersLineEdit->setText(m_additionalParameters);
}

void SettingsDialog::on_downloadButton_clicked()
{
    DownloadDialog* downloadDlg = new DownloadDialog(this);

    if(downloadDlg)
    {
        if(downloadDlg->exec() == QDialog::Accepted)
        {
            QString saveLocation = downloadDlg->getSaveLocation();

            if(!saveLocation.isEmpty())
            {
                ui->mcServerFileLineEdit->setText(saveLocation);
            }
        }

        delete downloadDlg;
        downloadDlg = 0;
    }
}

void SettingsDialog::on_javaBrowseButton_clicked()
{
    QString openDirectory = ui->customJavaLineEdit->text();

#ifdef Q_OS_WIN
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Java Executable"),
                                                    openDirectory,
                                                    tr("Java Executable (java.exe)"));
#elif Q_OS_UNIX
    if(openDirectory.isEmpty())
    {
        openDirectory = "/home";
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Java Executable"),
                                                    openDirectory,
                                                    tr("Java Executable (java)"));
#endif
    if(!fileName.isEmpty())
    {
        ui->customJavaLineEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void SettingsDialog::on_mcServerBrowseButton_clicked()
{
    QString openDirectory = ui->mcServerFileLineEdit->text();

#ifdef Q_OS_UNIX
    if(openDirectory.isEmpty())
    {
        openDirectory = "/home";
    }
#endif

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Minecraft Server"),
                                                    openDirectory,
                                                    tr("Jar Files (*.jar)"));
    if(!fileName.isEmpty())
    {
        ui->mcServerFileLineEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void SettingsDialog::accept()
{
    if(ui->mcServerFileLineEdit->text().isEmpty())
    {
        QMessageBox::information(this, tr("Qt Minecraft Server"),
                                 tr("No Minecraft Server File selected!\nPlease select a Minecraft Server File before continuing."));

    }
    else
    {
        QDialog::accept();
    }
}

void SettingsDialog::on_buttonBox_accepted()
{
    m_mcServerPath = ui->mcServerFileLineEdit->text();
    m_customJavaPath = ui->customJavaLineEdit->text();
    m_useCustomJavaPath = ui->customRadioButton->isChecked();
    m_xms = ui->xmsSpinBox->value();
    m_xmx = ui->xmxSpinBox->value();
    m_additionalParameters = ui->additionalParametersLineEdit->text();
}
