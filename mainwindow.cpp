#include "mainwindow.h"

#include <math.h>
#include <unordered_set>

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>

#include "ui_mainwindow.h"
#include "utils.h"


const std::unordered_set<std::string> AUDIO_EXTENSIONS {"mp3", "flac", "wav"};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    PLAY_ICON(QIcon("./resources/play.png")),
    PAUSE_ICON(QIcon("./resources/pause.png"))
{
    m_ui->setupUi(this);
    initialize_components();

    this->setWindowTitle("Music Player");
    this->setWindowIcon(QIcon("resources/music.png"));
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void
MainWindow::initialize_components() {
    QList<QLabel*> labels = m_ui->centralwidget->findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->objectName() == "song_name_label") {
            m_song_name_label = label;
            m_song_name_label->setText("");
        }
        else if (label->objectName() == "artist_label") {
            m_artist_label = label;
            m_artist_label->setText("");
        }
    }

    QList<QPushButton*> push_buttons =
        m_ui->centralwidget->findChildren<QPushButton*>();
    for (QPushButton* pbutton : push_buttons) {
        if (pbutton->objectName() == "play_pause_button") {
            m_play_pause_button = pbutton;
            connect(m_play_pause_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(play_pause_button_clicked()));
        }
        else if (pbutton->objectName() == "dir_prompt_button") {
            m_dir_prompt_button = pbutton;
            connect(m_dir_prompt_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(dir_prompt_button_clicked()));
        }
        else if (pbutton->objectName() == "next_song_button") {
            m_next_song_button = pbutton;
            connect(m_next_song_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(next_song_button_clicked()));
        }
        else if (pbutton->objectName() == "prev_song_button") {
            m_prev_song_button = pbutton;
            connect(m_prev_song_button,
                    SIGNAL(clicked()),
                    this,
                    SLOT(prev_song_button_clicked()));
        }
    }

    m_songs_list = m_ui->centralwidget->findChild<QListWidget*>();
    connect(m_songs_list,
            SIGNAL(itemClicked(QListWidgetItem*)),
            this,
            SLOT(song_lists_item_clicked(QListWidgetItem*)));

    m_song_slider = m_ui->centralwidget->findChild<QSlider*>();
    connect(m_song_slider,
            SIGNAL(sliderReleased()),
            this,
            SLOT(song_slider_released()));
}

void
MainWindow::play_pause_button_clicked() {
    if (m_iplayer.is_playing()) {
        m_play_pause_button->setIcon(PLAY_ICON);
        m_iplayer.stop_song();
    } else {
        m_play_pause_button->setIcon(PAUSE_ICON);
        m_iplayer.continue_song();
    }
}

void
MainWindow::dir_prompt_button_clicked() {
    if (m_iplayer.is_playing()) play_pause_button_clicked();

    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Choose Directory"),
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty()) return;

    std::vector<std::string> audio_files = files_in_dir(dir.toStdString());

    for (const std::string& filepath : audio_files) {
        std::map<std::string, std::string> song_data = m_iplayer.add_song_to_list(filepath);
        if (song_data.size() == 0) continue;

        std::string song_display_info = song_data["artist"] + " - " + song_data["title"];
        m_songs_list->addItem(song_display_info.c_str());
    }

    if (m_songs_list->count() < 1) {
        QMessageBox::critical(
            this,
            tr("IPlayer"),
            tr("Directory does not contain any audio file!"));
        return;
    }

    m_play_pause_button->setEnabled(true);
    m_next_song_button->setEnabled(true);
    m_prev_song_button->setEnabled(true);
    m_song_slider->setEnabled(true);

    m_current_song_idx = 0;
    play_song();
}

void
MainWindow::song_lists_item_clicked(QListWidgetItem* item) {
    change_song(m_songs_list->row(item));
}

void
MainWindow::next_song_button_clicked() {
    change_song(m_current_song_idx < m_songs_list->count() - 1
        ? m_current_song_idx + 1
        : 0);
}

void
MainWindow::prev_song_button_clicked() {
    change_song(m_current_song_idx > 0
        ? m_current_song_idx - 1
        : m_songs_list->count() - 1);
}

void
MainWindow::change_song(size_t song_idx) {
    m_current_song_idx = song_idx;
    play_song();
}

void
MainWindow::song_progressed_duration_updated(float progressed_duration) {
    m_song_slider->setValue(round(progressed_duration));
}

void
MainWindow::song_slider_released() {
    int value = m_song_slider->value();
    m_iplayer.progress_to(value);
}

void
MainWindow::play_song() {
    // Play the song
    m_iplayer.play_song(m_current_song_idx);

    SongObserver* observer = new SongObserver(&m_iplayer);
    connect(observer,
            SIGNAL(current_song_ended()),
            SLOT(next_song_button_clicked()));
    connect(observer,
            SIGNAL(song_progressed_duration_changed(float)),
            SLOT(song_progressed_duration_updated(float)));
    connect(observer,
            SIGNAL(finished()),
            observer,
            SLOT(deleteLater()));
    observer->start();

    // Update UI
    ISongData current_song = m_iplayer.current_song();
    m_song_name_label->setText(current_song.get_song_title().c_str());
    m_artist_label->setText(current_song.get_artist().c_str());
    m_songs_list->item(m_current_song_idx)->setSelected(true);
    m_play_pause_button->setIcon(PAUSE_ICON);
    m_song_slider->setMaximum(ceil(m_iplayer.song_duration()));
    m_song_slider->setValue(0);
}
