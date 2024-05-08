#ifndef DOWNLOAD_SONG_DIALOG_HPP
#define DOWNLOAD_SONG_DIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>
#include <QListWidget>
#include <QLabel>
#include <QTimer>

#include <functional>

namespace Ui {
class DownloadSongDialog;
}

class MainWindow;


enum class MusicPlatform {
    SPOTIFY,
    YOUTUBE
};


enum class DownloadStatus {
    IN_PROGRESS,
    DONE,
    FAILED
};


inline std::string stringify_download_status(DownloadStatus status, bool is_emoji=false) {
    switch (status) {
    case DownloadStatus::IN_PROGRESS:
        return is_emoji ? "   ⏳" : "Downloading";
    case DownloadStatus::DONE:
        return is_emoji ? "   ✅" : "Done";
    case DownloadStatus::FAILED:
        return is_emoji ? "   ❌" : "Failed";
    }
    return "";
}


class DownloadSongDialog : public QDialog
{
    Q_OBJECT
friend class DownloaderThread;

public:
    explicit DownloadSongDialog(QWidget *parent = nullptr);
    ~DownloadSongDialog();

private slots:
    void download_song_button_clicked();
    void dir_select_button_clicked();

private:
    void initialize_components();

    bool validate_input();

    void download_finished_callback(std::string url, DownloadStatus status);

    void warning_popup(std::string error_message);

    Ui::DownloadSongDialog* m_ui;

    QLineEdit* m_song_url_input_box;
    QLabel* m_warning_label;
    QLabel* m_dir_label;
    QPushButton* m_download_song_button;
    QPushButton* m_dir_select_button;
    QListWidget* m_download_progress_list;
    QListWidgetItem* m_download_progress_list_header;

    MainWindow* m_main_window;

    std::string m_download_dir; // Directory to download to
};


class SongDownloadItem : public QListWidgetItem {
public:
    SongDownloadItem(QListWidget* container,
                     std::string url,
                     unsigned int index,
                     DownloadStatus status=DownloadStatus::IN_PROGRESS) :
            m_url(url),
            m_index(index),
            m_status(status),
            QListWidgetItem(SongDownloadItem::generate_text_content(url, status)) {}

    std::string url() const { return m_url; }
    unsigned int index() const { return m_index; }
    DownloadStatus status() const { return m_status; }

    void set_status(DownloadStatus status) {
        m_status = status;
        this->setText(SongDownloadItem::generate_text_content(m_url, m_status));
    }

private:
    static QString generate_text_content(std::string url, DownloadStatus status);

    std::string m_url;
    unsigned int m_index;
    DownloadStatus m_status;
};


class DownloaderThread : public QThread {
    Q_OBJECT

public:
    DownloaderThread(std::string url,
                     MusicPlatform platform,
                     std::string download_dir,
                     DownloadSongDialog* caller) :
        m_url(url),
        m_platform(platform),
        m_download_dir(download_dir),
        m_caller(caller),
        QThread() {}

private:
    void run();

    std::string m_url;
    MusicPlatform m_platform;
    std::string m_download_dir;
    DownloadSongDialog* m_caller;
};

#endif // DOWNLOAD_SONG_DIALOG_HPP
