#include "download_song_dialog.hpp"
#include "ui_download_song_dialog.h"

#include "../service/spotdl.hpp"
#include "../service/ytdlp.hpp"
#include "../utils/string.hpp"
#include "mainwindow.hpp"

#include <QLineEdit>

#include <QDebug>


void
DownloaderThread::run() {
    bool download_res;

    switch (m_platform) {
    case MusicPlatform::SPOTIFY:
        download_res = download_spotify_song(m_url, m_download_dir);
        break;
    case MusicPlatform::YOUTUBE:
        download_res = download_youtube_audio(m_url, m_download_dir);
        break;
    }
    DownloadStatus status = download_res
            ? DownloadStatus::DONE
            : DownloadStatus::FAILED;
    m_caller->download_finished_callback(m_url, status);
}

QString
SongDownloadItem::generate_text_content(std::string url, DownloadStatus status) {
    std::string status_emoji = stringify_download_status(status, true /* is emoji */);
    return QString((status_emoji + "\t" + url).c_str());
}

DownloadSongDialog::DownloadSongDialog(QWidget *parent) : QDialog(parent),
                                                          m_ui(new Ui::DownloadSongDialog)
{
    m_main_window = static_cast<MainWindow*>(parent);
    m_ui->setupUi(this);
    initialize_components();
}

DownloadSongDialog::~DownloadSongDialog()
{
    delete m_download_progress_list_header;
    delete m_ui;
}

void DownloadSongDialog::initialize_components()
{
    m_song_url_input_box = m_ui->song_url_input_box;
    m_song_url_input_box->setPlaceholderText(
            "Spotify track URL (e.g. https://open.spotify.com/track/7MXVkk9YMctZqd1Srtv4MB)");

    m_download_progress_list = m_ui->download_progress_list;
    std::string header_content =
            "Status\tURL";
    m_download_progress_list_header = new QListWidgetItem(tr(header_content.c_str()),
                                                          m_download_progress_list);

    m_download_song_button = m_ui->download_button;
    connect(m_download_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(download_song_button_clicked()));
}

void DownloadSongDialog::download_song_button_clicked()
{
    std::string url = m_song_url_input_box->text().toStdString();
    if (url.empty()) {
        return;
    }

    MusicPlatform platform;
    if (starts_with(url, "https://www.youtube.com")) {
        platform = MusicPlatform::YOUTUBE;
    }
    else if (starts_with(url, "https://open.spotify.com")) {
        platform = MusicPlatform::SPOTIFY;
    }
    else {
        // TODO: Report error here
        return;
    }

    SongDownloadItem* song_download_item = new SongDownloadItem(m_download_progress_list,
                                                                url,
                                                                m_download_progress_list->count());

    m_download_progress_list->addItem(song_download_item);

    DownloaderThread* downloader = new DownloaderThread(url, platform, m_download_dir, this);
    downloader->start();
}

void
DownloadSongDialog::download_finished_callback(std::string url, DownloadStatus status) {
    // Find the corresponding SongDownloadItem instance and update status
    // Start at index 1 since header will be at 0
    for (unsigned int i = 1; i < m_download_progress_list->count(); i++) {
        auto item = static_cast<SongDownloadItem*>(m_download_progress_list->item(i));
        if (item->url() == url) {
            item->set_status(status);
            break;
        }
    }

    if (status == DownloadStatus::DONE) {
        m_main_window->get_notified_song_downloaded(m_download_dir);
    }
}
