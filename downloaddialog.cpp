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

#include "downloaddialog.h"
#include "ui_downloaddialog.h"

#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>

#define MCSERVER_URL "http://s3.amazonaws.com/MinecraftDownload/launcher/minecraft_server.jar"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);

    m_saveLocation = "";
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::on_downloadButton_clicked()
{
    startDownload();
}

void DownloadDialog::on_buttonBox_accepted()
{
    m_saveLocation = ui->saveLineEdit->text();

    accept();
}

void DownloadDialog::on_buttonBox_rejected()
{
    reject();
}

void DownloadDialog::doDownload(const QUrl& url)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Download Folder"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        ui->downloadLogTextEdit->clear();
        ui->saveLineEdit->clear();

        QString saveLocation = saveFileName(url, dir);
        QString program = "./wget";
        QStringList arguments;
        arguments << "-O" << saveLocation << MCSERVER_URL;

        ui->downloadLogTextEdit->append(tr("Connecting to %1\n").arg(MCSERVER_URL));
        ui->downloadLogTextEdit->append(tr("Downloading Minecraft Server... Please wait...\n"));

        ui->downloadLogTextEdit->update();

        QProcess p;
        p.start(program, arguments);
        p.waitForFinished(-1);

        QString p_stdout = p.readAllStandardOutput();
        QString p_stderr = p.readAllStandardError();

        if(!p_stdout.isEmpty())
        {
            ui->downloadLogTextEdit->append(p_stdout);
        }

        if(!p_stderr.isEmpty())
        {
            ui->downloadLogTextEdit->append("\n");
            ui->downloadLogTextEdit->append(p_stderr);
        }

        QApplication::restoreOverrideCursor();

        if(p.exitStatus() == QProcess::NormalExit)
        {
            ui->saveLineEdit->setText(QDir::toNativeSeparators(saveLocation));
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("Failed to download Minecraft Server!"));
        }
    }
}

QString DownloadDialog::saveFileName(const QUrl& url, const QString& path)
{
    QString urlpath = url.path();
    QString basename = QFileInfo(urlpath).fileName();

    if (basename.isEmpty())
    {
        basename = "minecraft_server.jar";
    }

    if (QFile::exists(path + QString("/") + basename))
    {
        // already exists, don't overwrite
        int i = 0;
        basename = basename.remove(".jar");
        basename += '.';
        while (QFile::exists(path + QString("/") + basename + QString::number(i) + QString(".jar")))
        {
            ++i;
        }

        basename += QString::number(i);
        basename += QString(".jar");
    }

    return path + QString("/") + basename;
}

void DownloadDialog::startDownload()
{
    QUrl url = QUrl(MCSERVER_URL);
    doDownload(url);
}
