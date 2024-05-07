#include "empty_song_dir_dialog.hpp"
#include "ui_emptysongdirdialog.h"

#include "mainwindow.hpp"

EmptySongDirDialog::EmptySongDirDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::EmptySongDirDialog)
{
    m_ui->setupUi(this);
    m_main_window = static_cast<MainWindow*>(parent);
    m_download_song_dialog = m_main_window->get_download_song_dialog();
    initialize_components();
}

EmptySongDirDialog::~EmptySongDirDialog()
{
    delete m_ui;
}

void EmptySongDirDialog::initialize_components()
{
    m_nav_to_download_button = m_ui-> nav_to_download;
    connect(m_nav_to_download_button,
            SIGNAL(clicked()),
            this,
            SLOT(nav_to_download_button_clicked()));
}

void EmptySongDirDialog::nav_to_download_button_clicked()
{
    m_download_song_dialog->show();
    this->close();
}
