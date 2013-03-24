#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();
    
private slots:
    void on_downloadButton_clicked();

private:
    Ui::DownloadDialog *ui;
};

#endif // DOWNLOADDIALOG_H
