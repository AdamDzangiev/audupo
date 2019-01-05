#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDir>
#include <QGraphicsDropShadowEffect>

#include "stylehelper.h"
#include <QXmlStreamReader>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_leftMouseButtonPressed(None)
{
    ui->setupUi(this);
    DownloadManager = new QNetworkAccessManager();
    connect(DownloadManager, &QNetworkAccessManager::finished, this, &Widget::onResultDownload);

    /// Настройка UI
    this->setWindowFlags(Qt::FramelessWindowHint);      // Отключаем оформление окна
    this->setAttribute(Qt::WA_TranslucentBackground);   // Делаем фон главного виджета прозрачным
    this->setStyleSheet(StyleHelper::getWindowStyleSheet());    // Устанавливаем стиль виджета
    this->setMouseTracking(true);   // Включаем отслеживание курсора без нажатых кнопокы

    // Создаём эффект тени
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(9); // Устанавливаем радиус размытия
    shadowEffect->setOffset(0);     // Устанавливаем смещение тени
    ui->widgetInterface->setGraphicsEffect(shadowEffect);   // Устанавливаем эффект тени на окно
    ui->widgetInterface->layout()->setMargin(0);            // Устанавливаем размер полей
    ui->widgetInterface->layout()->setSpacing(0);
    ui->label->setText("AUDUPO ПЛЕЕР");
    ui->label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // Установка стилей для всех элементов
    ui->currentTrack->setStyleSheet(StyleHelper::getLabelStyleSheet());
    ui->label->setStyleSheet(StyleHelper::getLabelStyleSheet());
    ui->btn_close->setStyleSheet(StyleHelper::getCloseStyleSheet());
    ui->btn_maximize->setStyleSheet(StyleHelper::getMaximizeStyleSheet());
    ui->btn_minimize->setStyleSheet(StyleHelper::getMinimizeStyleSheet());
    ui->btn_next->setStyleSheet(StyleHelper::getNextStyleSheet());
    ui->btn_previous->setStyleSheet(StyleHelper::getPreviousStyleSheet());
    ui->btn_stop->setStyleSheet(StyleHelper::getStopStyleSheet());
    ui->btn_play->setStyleSheet(StyleHelper::getPlayStyleSheet());
    ui->btn_pause->setStyleSheet(StyleHelper::getPauseStyleSheet());
    ui->btn_add->setStyleSheet(StyleHelper::getMenuStyleSheet());
    ui->playlistView->setStyleSheet(StyleHelper::getTableViewStyleSheet());
    ui->btn_add->setText(tr("Добавить плейлист"));
    ui->btn_next->setCursor(Qt::PointingHandCursor);
    ui->btn_previous->setCursor(Qt::PointingHandCursor);
    ui->btn_stop->setCursor(Qt::PointingHandCursor);
    ui->btn_play->setCursor(Qt::PointingHandCursor);
    ui->btn_pause->setCursor(Qt::PointingHandCursor);
    ui->btn_i->setText(tr("Инструкция"));
    ui->btn_i->setStyleSheet(StyleHelper::getMenuStyleSheet());

    ui->horizontalLayout->setSpacing(6);
    ///
    m_playListModel = new QStandardItemModel(this);
    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Audio Track")
                                               << tr("File Path"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(70);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);

    connect(ui->btn_previous, &QToolButton::clicked, m_playlist, &QMediaPlaylist::previous);
    connect(ui->btn_next, &QToolButton::clicked, m_playlist, &QMediaPlaylist::next);
    connect(ui->btn_play, &QToolButton::clicked, m_player, &QMediaPlayer::play);
    connect(ui->btn_pause, &QToolButton::clicked, m_player, &QMediaPlayer::pause);
    connect(ui->btn_stop, &QToolButton::clicked, m_player, &QMediaPlayer::stop);

    /// коннекты для кнопок сворачивания/максимизации/минимизации/закрытия
    connect(ui->btn_minimize, &QToolButton::clicked, this, &QWidget::showMinimized);
    connect(ui->btn_maximize, &QToolButton::clicked, [this](){
        if (this->isMaximized()) {
            ui->btn_maximize->setStyleSheet(StyleHelper::getMaximizeStyleSheet());
            this->layout()->setMargin(9);
            this->showNormal();
        } else {
            ui->btn_maximize->setStyleSheet(StyleHelper::getRestoreStyleSheet());
            this->layout()->setMargin(0);
            this->showMaximized();
        }
    });
    connect(ui->btn_close, &QToolButton::clicked, this, &QWidget::close);
    ///

    connect(ui->playlistView, &QTableView::doubleClicked, [this](const QModelIndex &index){
        filename_download = vectorplaylist[((index.row() * 5)+4)];
        indexqm = index.row();
        QFile* playlistfile = new QFile(music_dir + filename_download);
        if(!playlistfile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            getDataDownload();
            msgBox.setWindowTitle("Скачивание файла");
            msgBox.setText("Просим дождаться окончания скачивания файла");
            msgBox.exec();
            m_playlist->blockSignals(true);
        }else{
            m_playlist->setCurrentIndex(index.row());
        }

        qDebug() << index.row();
    });

    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->currentTrack->setText(m_playListModel->data(m_playListModel->index(index, 0)).toString());
    });
}

Widget::~Widget()
{
    delete ui;
    delete m_playListModel;
    delete m_playlist;
    delete m_player;
}

QPoint Widget::previousPosition() const
{
    return m_previousPosition;
}

void Widget::getDataDownload()
{
    QUrl url(site_url + filename_download);
    QNetworkRequest request;
    request.setUrl(url);
    DownloadManager->get(request);
}

void Widget::onResultDownload(QNetworkReply *reply)
{
    if(reply->error()){
        qDebug() << "ERROR: No dowload";
        qDebug() << reply->errorString();
        msgBox.setWindowTitle("Ошибка!");
        msgBox.setText("ERROR: No dowload");
        msgBox.exec();
    }else{
        QFile *file = new QFile(music_dir + filename_download);
        if(file->open(QFile::WriteOnly)){
            file->write(reply->readAll());
            file->close();
        }
        qDebug() << "Dowloading is completed";
        if(isDownload_playlist){
            m_playlist->blockSignals(false);
            m_playlist->setCurrentIndex(indexqm);
          }
        emit onReadyDownload();
    }
}

void Widget::setPreviousPosition(QPoint previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged(previousPosition);
}

bool Widget::xmlFile(QString playfile)
{
    QFile* playlistfile = new QFile(playfile);
    if(!playlistfile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка чтение файла" << playlistfile;
        return false;
        msgBox.setWindowTitle("Ошибка!");
        msgBox.setText("Ошибка чтение файла");
        msgBox.exec();
    }
    isDownload_playlist = true;
    QXmlStreamReader xml(playlistfile);
    parsePlayList(xml);
}

QMap<QString, QString> Widget::addElementDataToAudioMap2(QXmlStreamReader& xml)
{
    QMap<QString, QString> audio;
    if (xml.tokenType() != QXmlStreamReader::StartElement && xml.name() == "audio")
        return audio;
    parseTag(xml, audio);

}

void Widget::addElementDataToMap(QXmlStreamReader& xml, QMap<QString, QString>& map)
{
    if (xml.tokenType() != QXmlStreamReader::StartElement)
        return;
    QString elementName = xml.name().toString();
    xml.readNext();
    map.insert(elementName, xml.text().toString());
}

void Widget::parsePlayList(QXmlStreamReader& xml)
{
    while(!xml.atEnd() && !xml.hasError()){
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "list")
                continue;
            if(xml.name() == "audio")
            {
                addElementDataToAudioMap2(xml);
            }
        }
    }
}

void Widget::addElementToVector(QVector<QString>& vectorplaylist, QMap<QString, QString>& map){

    QMap<QString,QString>::iterator it = map.begin();
    for(;it != map.end(); ++it)
    {
        vectorplaylist.append(it.value());
    }

}

void Widget::parseTag(QXmlStreamReader& xml, QMap<QString, QString>& audio)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("id"))
        audio["id"] = attributes.value("id").toString();
    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "audio"))
    {
        if (xml.tokenType() == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "audio_group_name")
                addElementDataToMap(xml, audio);
            if (xml.name() == "track_name")
                addElementDataToMap(xml, audio);
            if (xml.name() == "time")
                addElementDataToMap(xml, audio);
            if (xml.name() == "url")
                addElementDataToMap(xml, audio);
        }
        xml.readNext();
    }
    addElementToVector(vectorplaylist,audio);
}

void Widget::sort_vector()
{
    int i;
    QString val;
    for(i = 0; i < vectorplaylist.size(); i++)
    {
        if(indexpl == 0 || i%5 == 0){
            val = vectorplaylist[indexpl];
            vectorplaylist[indexpl] = vectorplaylist[(indexpl+1)];
            vectorplaylist[(indexpl+1)] = val;
            indexpl = indexpl+5;
        }
    }
}

void Widget::on_btn_add_clicked()
{
    filename_download = "playlist.xml";
    getDataDownload();
    msgBox.setWindowTitle("Скачивание файла");
    msgBox.setText("Просим дождаться окончания скачивания файла");
    msgBox.exec();

    if(indexpl >= vectorplaylist.size()){
        indexpl = 0;
    }

    if(!isDownload_playlist)
    {
        xmlFile(music_dir + playlistfile);
        sort_vector();

        for(int i=0; i<vectorplaylist.size(); i=i+5)
        {
            m_playListModel->appendRow(new QStandardItem(vectorplaylist[(i+1)] + " - " + vectorplaylist[(i+3)] + " " + vectorplaylist[(i+2)]));
            m_playlist->addMedia(QUrl(music_dir + vectorplaylist[(i+4)]));
        }

    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton ) {
        m_leftMouseButtonPressed = checkResizableField(event);
        setPreviousPosition(event->pos());
    }
    return QWidget::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_leftMouseButtonPressed = None;
    }
    return QWidget::mouseReleaseEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    switch (m_leftMouseButtonPressed) {
    case Move: {
        if (isMaximized()) {
            ui->btn_maximize->setStyleSheet(StyleHelper::getMaximizeStyleSheet());
            this->layout()->setMargin(9);
            auto part = event->screenPos().x() / width();
            this->showNormal();
            auto offsetX = width() * part;
            setGeometry(event->screenPos().x() - offsetX, 0, width(), height());
            setPreviousPosition(QPoint(offsetX, event->y()));
        } else {
            auto dx = event->x() - m_previousPosition.x();
            auto dy = event->y() - m_previousPosition.y();
            setGeometry(x() + dx, y() + dy, width(), height());
        }
        break;
    }
    case Top: {
        if (!isMaximized()) {
            auto dy = event->y() - m_previousPosition.y();
            setGeometry(x(), y() + dy, width(), height() - dy);
        }
        break;
    }
    case Bottom: {
        if (!isMaximized()) {
            auto dy = event->y() - m_previousPosition.y();
            setGeometry(x(), y(), width(), height() + dy);
            setPreviousPosition(event->pos());
        }
        break;
    }
    case Left: {
        if (!isMaximized()) {
            auto dx = event->x() - m_previousPosition.x();
            setGeometry(x() + dx, y(), width() - dx, height());
        }
        break;
    }
    case Right: {
        if (!isMaximized()) {
            auto dx = event->x() - m_previousPosition.x();
            setGeometry(x(), y(), width() + dx, height());
            setPreviousPosition(event->pos());
        }
        break;
    }
    default:
        checkResizableField(event);
        break;
    }
    return QWidget::mouseMoveEvent(event);
}

Widget::MouseType Widget::checkResizableField(QMouseEvent *event)
{
    QPointF position = event->screenPos();
    qreal x = this->x();
    qreal y = this->y();
    qreal width = this->width();
    qreal height = this->height();

    QRectF rectTop(x + 9, y, width - 18, 7);
    QRectF rectBottom(x + 9, y + height - 7, width - 18, 7);
    QRectF rectLeft(x, y + 9, 7, height - 18);
    QRectF rectRight(x + width - 7, y + 9, 7, height - 18);
    QRectF rectInterface(x + 9, y + 9, width - 18, height - 18);

    if (rectTop.contains(position)) {
        setCursor(Qt::SizeVerCursor);
        return Top;
    } else if (rectBottom.contains(position)) {
        setCursor(Qt::SizeVerCursor);
        return Bottom;
    } else if (rectLeft.contains(position)) {
        setCursor(Qt::SizeHorCursor);
        return Left;
    } else if (rectRight.contains(position)) {
        setCursor(Qt::SizeHorCursor);
        return Right;
    } else if (rectInterface.contains(position)){
        setCursor(QCursor());
        return Move;
    } else {
        setCursor(QCursor());
        return None;
    }
}


void Widget::on_btn_i_clicked()
{
    msgBox.setWindowTitle("Внимательно прочтите!");
    msgBox.setText("1.Для добавления плейлиста нажмите кнопку Добавить\n2.В плейлисте нажать два раза на необходимую песню\n3.Дождаться скачивания и прослушать");
    msgBox.exec();
}
