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

#include <QFileDialog>
#include <QMessageBox>

#define MCSERVER_URL "http://s3.amazonaws.com/MinecraftDownload/launcher/minecraft_server.jar"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);

    m_saveLocation = "";
    m_downloadPath = "";

    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::initialize()
{
    ui->downloadButton->setToolTip(tr("Click here to download the official Minecraft Server\nA connection will be made to %1")
                                   .arg(MCSERVER_URL));
}

void DownloadDialog::on_downloadButton_clicked()
{
    QString openDirectory = "";

#ifdef Q_OS_UNIX
    openDirectory = "/home";
#endif

    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Download Folder"),
                                                    openDirectory,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        ui->downloadLogTextEdit->clear();
        ui->saveLineEdit->clear();

        m_downloadPath = dir;
        startDownload();
    }
    else
    {
        m_downloadPath = "";
    }
}

void DownloadDialog::on_buttonBox_accepted()
{
    m_saveLocation = ui->saveLineEdit->text();
}

void DownloadDialog::startDownload()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui->downloadLogTextEdit->append(tr("Connecting to %1").arg(MCSERVER_URL));
    ui->downloadLogTextEdit->append(tr("Downloading Minecraft Server... Please wait..."));

    doDownload( QUrl(MCSERVER_URL) );
}

void DownloadDialog::doDownload(const QUrl& url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    currentDownloads.append(reply);
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

bool DownloadDialog::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        ui->downloadLogTextEdit->append(tr("<font color=\"red\">Could not open %1 for writing: %2</font>")
                                        .arg(QDir::toNativeSeparators(filename))
                                        .arg(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    ui->saveLineEdit->setText(QDir::toNativeSeparators(filename));

    return true;
}

void DownloadDialog::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();

    if (reply->error())
    {
        ui->downloadLogTextEdit->append(tr("<font color=\"red\">Download of %1 failed: %2</font>")
                                        .arg(url.toEncoded().constData())
                                        .arg(reply->errorString()));
    }
    else
    {
        QString filename = saveFileName(url, m_downloadPath);
        if (saveToDisk(filename, reply))
        {
            ui->downloadLogTextEdit->append(tr("Download of %1 succeeded (saved to %2)")
                                            .arg(url.toEncoded().constData())
                                            .arg(QDir::toNativeSeparators(filename)));
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    QApplication::restoreOverrideCursor();
}
