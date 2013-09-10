#include	<iostream>
#include	<QDir>
#include	<QStringList>
#include	<QTextStream>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sstream>

#ifdef	WIN32
#include	<windows.h>
#endif
#include	"MainWindow.hh"

extern "C" {
#include	<libavformat/avformat.h>
}

const char	*confTab[] = {
  "eyecatch_begin",
  "eyecatch_end",
  "no_double",
  "selected_player",
  "vlc_path",
  "mplayer_path"
};

MainWindow::MainWindow() : _vbox(this),
			   _playlistDuration(0),
			   _lengthTime("duration: 0m00"),
			   _start("start"),
			   _shufle("shuffle"),
			   _pick("pick"),
			   _clearPlaylist("clear"),
			   _changeDirectory("change directory"),
			   _karaDirectory("karaoke"),
			   _eyecatchDirectory("eyecatch"),
			   _PlayerMenu("Player options"),
			   _eyecatchMenu("eyecatch options"),
			   _playMenu("play options"),
			   _decoderThread(*this)
{
  QDesktopWidget *desktop = QApplication::desktop();

  initRand();
  resize(desktop->width(), desktop->height());
  setWindowTitle("Asamiya Saki will rape all your familly");
  setWindowIcon(QIcon("resources/sukeban_deka_icone.jpg"));
  setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
 
  _FilesList.setColumnCount(3);
  QStringList ColumnNames;
  ColumnNames << "karaoke name" << "lenght" << "nbr";
  _FilesList.setHeaderLabels(ColumnNames);
  _FilesList.setColumnWidth( 0, 500 );
  _FilesList.setRootIsDecorated(false);

  _splitter.addWidget(&_FilesList);
  _splitter.addWidget(&_karaListInfo);

  _karaListInfo.setLayout(&_RightLayout);

  _karaListInfoLayout.addWidget(&_lengthTime);

  _RightLayout.addLayout(&_karaListInfoLayout);
  _RightLayout.addWidget(&_karaList);

  _hbox2ndOptions.addWidget(&_changeDirectory);
  _hbox2ndOptions.addWidget(&_clearPlaylist);
  _hbox2ndOptions.addWidget(&_start);

  _menuBar.addMenu(&_PlayerMenu);
  _menuBar.addMenu(&_eyecatchMenu);
  _menuBar.addMenu(&_playMenu);

  _hboxOptions.addWidget(&_menuBar);
  _hboxOptions.addWidget(&_shufle);
  _hboxOptions.addWidget(&_pick);
  _hboxOptions.addWidget(&_savePlaylistButton);
  _hboxOptions.addWidget(&_loadPlaylistButton);
  _savePlaylistButton.setText("Save");
  _loadPlaylistButton.setText("Load");
 

  _beginEyecatch = _eyecatchMenu.addAction("begin eyecatch");
  _beginEyecatch->setCheckable(true);
  _endEyecatch = _eyecatchMenu.addAction("end eyecatch");
  _endEyecatch->setCheckable(true);

  _noDouble = _playMenu.addAction("no double ?");
  _noFullScreen = _playMenu.addAction("no Full Screen ?");
  _noDouble->setCheckable(true);
  _noFullScreen->setChecked(false);
  _noFullScreen->setCheckable(true);
  _noFullScreen->setChecked(false);

  _changePlayerLocation = _PlayerMenu.addAction("select player location");
  _selectMplayer = _PlayerMenu.addAction("select Mplayer as Player");
  _selectVLC = _PlayerMenu.addAction("select VLC as Player");
  _selectMplayer->setCheckable(true);
  _selectMplayer->setChecked(true);
  _selectVLC->setCheckable(true);

  _vbox.addLayout(&_hboxOptions);
  _vbox.addLayout(&_hbox2ndOptions);
  _vbox.addWidget(&_splitter);
  _vbox.addWidget(&_find);
  _find.hide();
  _vbox.addWidget(&_find2);
  _find2.hide();
  connector();
  readKaraDirectory();
  readEyecatchDirectory();

  loadConfig();
}

MainWindow::~MainWindow()
{
  delete _changePlayerLocation;
}

void	MainWindow::connector(void)
{
  connect(&_FilesList,
	  SIGNAL(itemActivated(QTreeWidgetItem *, int)),
	  this,
	  SLOT(addToPlaylist(QTreeWidgetItem *)));

  connect(&_karaList,
	  SIGNAL(itemActivated(QListWidgetItem *)),
	  this,
	  SLOT(rmItemFromKaraList(QListWidgetItem *)));

  /*actions*/
  connect(_changePlayerLocation, SIGNAL(triggered()), this, SLOT(changePlayerLocation(void)));
  connect(_selectMplayer, SIGNAL(triggered()), this, SLOT(selectMplayer(void)));
  connect(_selectVLC, SIGNAL(triggered()), this, SLOT(selectVLC(void)));

  connect(_beginEyecatch, SIGNAL(triggered()), this, SLOT(beginEyecatch(void)));
  connect(_endEyecatch, SIGNAL(triggered()), this, SLOT(endEyecatch(void)));
  connect(_noDouble, SIGNAL(triggered()), this, SLOT(noDouble(void)));
  connect(_noFullScreen, SIGNAL(triggered()), this, SLOT(noDouble(void)));


  /*button*/
  connect(&_start, SIGNAL(clicked(bool)), this, SLOT(start(void)));
  connect(&_shufle, SIGNAL(clicked(bool)), this, SLOT(shufle(void)));
  connect(&_pick, SIGNAL(clicked(bool)), this, SLOT(pick(void)));
  connect(&_clearPlaylist, SIGNAL(clicked(bool)), this, SLOT(clearPlaylist(void)));
  connect(&_changeDirectory, SIGNAL(clicked(bool)), this, SLOT(changeDirectory(void)));

  connect(&_savePlaylistButton, SIGNAL(clicked(bool)), this, SLOT(savePlaylist(void)));
  connect(&_loadPlaylistButton, SIGNAL(clicked(bool)), this, SLOT(loadPlaylist(void)));

  /*check box*/

  /* mine */
  connect(&_find, SIGNAL(textEdited(QString)), this, SLOT(ctrlfedited(void)));
  connect(&_find2, SIGNAL(textEdited(QString)), this, SLOT(ctrlgedited(void)));
}

void	MainWindow::loadPlaylist()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Playlist"),
						  "./",
						  tr("Playlist (*.pls)"));
  QFile f(filename);
  f.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream in(&f);
  Media* nitem;
  QString line;
  std::cout << filename.toUtf8().constData();
  // load data in f
  while (!in.atEnd()) {
    line = in.readLine();
    nitem = new Media(line);
////////////// start of copypaste
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    if (!avformat_open_input(&pFormatCtx, static_cast<Media *>(nitem)->getPath().toLocal8Bit().constData(), NULL, NULL))
      {
	avformat_find_stream_info(pFormatCtx, NULL);
	  static_cast<Media *>(nitem)->setDuration(pFormatCtx->duration / AV_TIME_BASE);
	  static_cast<Media *>(nitem)->setFps((float)pFormatCtx->streams[0]->r_frame_rate.num /
					      (float)pFormatCtx->streams[0]->r_frame_rate.den);
      }
      avformat_free_context(pFormatCtx);
      ////////////// end of copypaste
      addToPlaylist(static_cast<QTreeWidgetItem *>(nitem));
    //newItem = new Media(static_cast<Media*>(item)->getPath());
    //String line = in.readLine();
  }
  // end load
  f.close();
}

void	MainWindow::savePlaylist()
{
  int i;
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Playlist"),
						  "./lastplaylist.pls",
						  tr("Playlist (*.pls)"));
  QFile f(filename);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  // store data in f
  for(i=0;i<_karaList.count();i++) {
    //f.write(static_cast<Media*>(_karaList.item(i))->getPath().toUtf8().constData());
    out << static_cast<Media*>(_karaList.item(i))->getPath().toUtf8().constData() << "\n";
  }
  // end store
  f.close();
}

QTreeWidget &MainWindow::getFileList()
{
  return (_FilesList);
}

void	MainWindow::readKaraDirectory()
{
  QDir	dir(_karaDirectory);
  if (!dir.exists())
    {
      qWarning("Cannot find the karaoke directory");
      return ;
    }

  // std::cout << (dir.path() + SLASH).toLocal8Bit().constData() << std::endl;

  QStringList  filesName = dir.entryList();
  QStringList::const_iterator constIterator;
  int	i = 0;

  for (constIterator = filesName.constBegin(); constIterator != filesName.constEnd();
       ++constIterator)
    {
      if (isVideo(*constIterator))
	{
	  QTreeWidgetItem* nitem = new Media((dir.path() + SLASH), *constIterator);
	  static_cast<Media *>(nitem)->setDuration(-1);
	  nitem->setText(0, ((Media *)nitem)->getName());
	  nitem->setText(1, "loading");
	  nitem->setText(2, QString::number(i + 1));
	  _FilesList.addTopLevelItem(nitem);
	  ++i;
	}
    }
  _decoderThread.start();
}


void	MainWindow::readEyecatchDirectory()
{
  QDir	dir(_eyecatchDirectory);
  if (!dir.exists())
    {
      qWarning("Cannot find the eyecatch directory");
      return ;
    }

  QStringList  filesName = dir.entryList();
  QStringList::const_iterator constIterator;

  for (constIterator = filesName.constBegin(); constIterator != filesName.constEnd();
       ++constIterator)
    {
      if (isVideo(*constIterator))
	_eyecatchList.push_back(*constIterator);
    }
}

bool	MainWindow::isVideo(const QString &str)
{
  return (str.contains(".avi")
	  || str.contains(".mkv")
	  || str.contains(".flv")
	  || str.contains(".mp4")
	  || str.contains(".ogv"));
}


void MainWindow::genereASS(const Media &media) const
{
  QProcess *p = new QProcess();
  QStringList args;
  std::ostringstream ss;

  args << media.getPath();
  ss << media.getFps();
  args << QString::fromStdString(ss.str());
  p->execute("toy2ass",args);
  delete p;
}

MainWindow::Conf  MainWindow::getConfTabIdx(const QString &str)
{
  int	i;

  for (i = 0; i < END; ++i)
    {
      if (str == confTab[static_cast<Conf>(i)])
	return (static_cast<Conf>(i));
    }
  return (END);
}

void	MainWindow::loadConfig()
{
  QFile f("asraf.conf");
  f.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream in(&f);
  QString line;
  while (!in.atEnd()) {
    line = in.readLine();
    QString left = line.section(':', 0, 0);
    QString right = line.section(':', 1);
    int idx = getConfTabIdx(left);
    switch (idx)
      {
      case EYECATCH_BEGIN:
	_beginEyecatch->setChecked(right.toInt());
	break;
      case EYECATCH_END:
	_endEyecatch->setChecked(right.toInt());
	break;
      case NO_DOUBLE:
	_noDouble->setChecked(right.toInt());
	break;
      case SELECTED_PLAYER:
	if (right == "vlc")
	  changePlayer(VLC);
	else
	  changePlayer(MPLAYER);
	break;
      case VLC_PATH:
      case MPLAYER_PATH:
	_player = right;
	break;
      default:
	break;
      }
    std::cout << left.toLocal8Bit().constData() << std::endl;  
    std::cout << right.toLocal8Bit().constData() << std::endl;  
  }
  f.close();
}

void	MainWindow::saveConfig()
{
  QFile f("asraf.conf");
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  // store data in f

  out << confTab[EYECATCH_BEGIN] << ':' <<  _beginEyecatch->isChecked() << '\n';
  out << confTab[EYECATCH_END] << ':' << _endEyecatch->isChecked() << '\n';
  out << confTab[NO_DOUBLE] << ':' << _noDouble->isChecked() << '\n';
  
  out << confTab[SELECTED_PLAYER] << ':';
  if (_selectVLC->isChecked())
    {
      out << "vlc" << '\n';
      out << confTab[VLC_PATH] << ':' << _player << '\n';
    }
  else
    {
      out << "mplayer" << '\n';
      out << confTab[MPLAYER_PATH] << ':' << _player << '\n';
    }


  // end store
  f.close();  
}

/*------------------- Slots methodes -------------------*/

/*Files list slots*/

bool	MainWindow::addToPlaylist(QTreeWidgetItem *item)
{
  QListWidgetItem* newItem = new Media(static_cast<Media&>(*item));
  QString pathAss = changeExtansion(static_cast<Media*>(item)->getPath(), "ass");
  if (access(pathAss.toLocal8Bit().constData(), 0))
    {
      std::cout << "cant find " << pathAss.toLocal8Bit().constData() << std::endl;
      if (!access(changeExtansion(static_cast<Media*>(item)->getPath(), "frm").toLocal8Bit().constData(), 0))
	genereASS(*static_cast<Media*>(item));
      // try use OcamlScript
    }
  if (!_noDouble->isChecked())
    {
      _karaList.addItem(newItem);
      _playlistDuration += static_cast<Media*>(item)->getDuration();
      _lengthTime.setText("duration: " + durationToString(_playlistDuration));
      return (true);
    }
  else
    {
      if (_karaList.findItems(static_cast<Media*>(item)->getName(), Qt::MatchCaseSensitive).empty())
	{
	  _karaList.addItem(newItem);
	  _playlistDuration += static_cast<Media*>(item)->getDuration();
	  _lengthTime.setText("duration: " + durationToString(_playlistDuration));
	  return (true);
	}
      else
	delete newItem;
      return (false);
    }
}


/*Kara list slots*/

void	MainWindow::rmItemFromKaraList(QListWidgetItem *litem)

{
  Media *item = static_cast<Media*>(litem);
  _playlistDuration -= item->getDuration();
  _lengthTime.setText("duration: " + durationToString(_playlistDuration));
  delete item;
}


/*ctrl f*/
void MainWindow::keyPressEvent(QKeyEvent *e)
{
  static bool	hasCtlrFBeenPress = false;
  static bool	hasCtlrGBeenPress = false;

  // static QString	toFind;

  if ( (e->key() == Qt::Key_F) && QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
      if (!hasCtlrFBeenPress)
	{
	  _find.show();
	  _find.setFocus();
	  hasCtlrFBeenPress = true;
	}
      else
	{
	  _find.hide();
	  hasCtlrFBeenPress = false;
	}
    }
  if ( (e->key() == Qt::Key_G) && QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
      if (!hasCtlrGBeenPress)
	{
	  _find2.show();
	  _find2.setFocus();
	  _ctrlg = -1;
	  hasCtlrGBeenPress = true;
	}
      else
	{
	  _find2.hide();
	  hasCtlrGBeenPress = false;
	}
    }
  if (e->key() == Qt::Key_Down ) {
    _ctrlg++;
    MainWindow::ctrlgedited();
  }
  if (e->key() == Qt::Key_Up && _ctrlg > 0) {
    _ctrlg--;
    MainWindow::ctrlgedited();
  }
}

void  MainWindow::changePlayer(int i)
{
  _playerOpt = "";
  if (i == MPLAYER)
    {
      if (_noFullScreen->isChecked() == false)
      	{
      	  _playerOpt = " -fs";
      	}
      _playerOpt += " -ass ";
      //_playerOpt += " -ass -framedrop -autosync 30 -mc 2.0 ";
      _player = getPlayerCmd();
    }
  else
    {
      if (_noFullScreen->isChecked() == false)
      	_playerOpt = " -f ";
      _player = getPlayerCmd<VLC>();      
    }
}


/*Button slots*/

void MainWindow::start(void)
{
  int	i = 0;
  QString	listsKara;
  QString	endlist;

  saveConfig();

#ifndef Q_OS_WIN32
  pid_t		forkRet;

  forkRet = fork();
  if (forkRet)
    return;
#endif
  if (_beginEyecatch->isChecked() | _endEyecatch->isChecked())
    {
      int	len = _eyecatchList.size();
      if (_beginEyecatch->isChecked() && len)
        {
	  listsKara += " ";
	  listsKara += "\"";
          listsKara += _eyecatchDirectory;
          listsKara += SLASH;
          listsKara += _eyecatchList[pRand() % len];
          listsKara += "\"";
          listsKara += _playerOpt;
        }
      if (_endEyecatch->isChecked() && len)
        {
          endlist += " ";
          endlist += "\"";
          endlist += _eyecatchDirectory.replace('/', SLASH);
          endlist += SLASH;
          endlist += _eyecatchList[pRand() % len];
          endlist += "\"";
          endlist += _playerOpt;
        }
    }

  while (_karaList.item(i))
    {
      listsKara += " ";
      listsKara += "\"";
      listsKara += static_cast<Media*>(_karaList.item(i))->getPath();
      listsKara += "\"";
      listsKara += _playerOpt;
      ++i;
    }
  listsKara += endlist;
  execPlaylist(_player, listsKara);
}

void MainWindow::shufle(void)
{
  int	len = _karaList.count();
  int	i = 0;

  if (len < 2)
    return ;

  QListWidgetItem *tmp;

  while (i < (len * 2))
    {
      tmp = _karaList.takeItem(pRand() % len);
      _karaList.insertItem(pRand() % (len - 1), tmp);
      ++i;
    }
}

void MainWindow::pick(void)
{
  int	len = _FilesList.topLevelItemCount();
  int	karaListLen = _karaList.count();
  if ((len - karaListLen) <= 0)
    return;
  while (!addToPlaylist(static_cast<Media *>(_FilesList.topLevelItem(pRand() % len))));
}

void MainWindow::clearPlaylist(void)
{
  QListWidgetItem *litem;
  while((litem = _karaList.item(0)) != NULL)
    {
      rmItemFromKaraList(litem);
    }
}

void MainWindow::changeDirectory(void)
{
  QString path = QFileDialog::getExistingDirectory(0);

  //std::cout << path.toLocal8Bit().constData() << std::endl;
  if ( path.isNull() == false )
    {
      _karaDirectory = path;
    }
  clearDirList();
  readKaraDirectory();
}

void MainWindow::changePlayerLocation(void)
{
  QString path = QFileDialog::getOpenFileName(0);

  if ( path.isNull() == false )
    {
      //toWinPath(path);
      _player = path;
    }
}

void MainWindow::ctrlfedited(void)
{
  // std::cout << _find.text().toUtf8().constData() << std::endl;
  //QStringList sl = _find.text().split(" ");
  QList<QTreeWidgetItem *> iList = _FilesList.findItems(_find.text(), Qt::MatchContains);;
  int i;
  for (i = 0; i < _FilesList.topLevelItemCount(); i++)
    {
      _FilesList.topLevelItem(i)->setHidden(true);
    }
  for (i = 0; i < iList.size(); i++)
    {
      iList[i]->setHidden(false);
    }
}


void MainWindow::ctrlgedited(void)
{
  // std::cout << _find2.text().toUtf8().constData() << std::endl;
  QList<QTreeWidgetItem *> iList = _FilesList.findItems(_find2.text(), Qt::MatchContains);
  if (iList.size() > 0) {
    if (_ctrlg < 0) _ctrlg = 0;
    if (_ctrlg >= iList.size()) _ctrlg = iList.size()-1;
    _FilesList.setCurrentItem(iList[_ctrlg]);
  }
}

void MainWindow::selectVLC(void)
{
  _selectMplayer->setChecked(false);
  _selectVLC->setChecked(true);
  changePlayer(VLC);
}

void MainWindow::selectMplayer(void)
{
  _selectMplayer->setChecked(true);
  _selectVLC->setChecked(false);
  changePlayer(MPLAYER);
}

void MainWindow::noDouble(void)
{
  // _double = !_double;
}

void MainWindow::beginEyecatch(void)
{
}

void MainWindow::endEyecatch(void)
{
}

/*------------------- !Slots methodes -------------------*/


void MainWindow::clearDirList(void)
{
  for(;;)
    {
      QTreeWidgetItem* item = _FilesList.takeTopLevelItem(0);
      if (item == NULL)
	return;
      delete item;
    }
}
