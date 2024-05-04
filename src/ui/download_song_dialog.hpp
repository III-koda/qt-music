#ifndef DOWNLOAD_SONG_DIALOG_HPP
#define DOWNLOAD_SONG_DIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>

namespace Ui {
class DownloadSongDialog;
}

class MainWindow;


class DownloadSongDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadSongDialog(QWidget *parent = nullptr);
    ~DownloadSongDialog();

    void set_download_dir(std::string dir) {
        m_download_dir = dir;
    }

private slots:
    void download_song_button_clicked();

private:
    void initialize_components();

    Ui::DownloadSongDialog* m_ui;

    QLineEdit* m_song_url_input_box;
    QPushButton* m_download_song_button;

    MainWindow* m_main_window;

    std::string m_download_dir; // Directory to download to
};


class DownloaderThread : public QThread {
    Q_OBJECT

public:
    DownloaderThread(std::string url,
                     std::string download_dir,
                     MainWindow* main_window) :
        m_url(url),
        m_download_dir(download_dir),
        m_main_window(main_window),
        QThread() {}

private:
    void run();

    std::string m_url;
    std::string m_download_dir;
    MainWindow* m_main_window;
};

#endif // DOWNLOAD_SONG_DIALOG_HPP
