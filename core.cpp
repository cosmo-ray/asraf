#include <QDir>
#include <QString>
#include <QDebug>

#include	"core.hh"


template	<>
QString  getPlayerCmd<VLC>()
{
  return ("vlc");
}

template	<>
QString  getPlayerCmd<MPLAYER>()
{
  // std::cout << "still alive" << std::endl;
#ifdef	Q_OS_WIN32
    QString ret = QDir::currentPath();

  ret += "/mplayer";
  return (ret);
#else
  return ("mplayer");
#endif
}

long int pRand()
{
#ifdef	Q_OS_WIN32
  return (rand());
#else
  return (random());
#endif  
}

void	initRand()
{
#ifdef	Q_OS_WIN32
  srand(153);
#else
  srandom(time(0));
#endif
}
 
const	QString durationToString(int64_t duration)
{
  QString ret;

  ret.append(QString("%1").arg((int)(duration / 60)));
  ret += "m";
  ret.append(QString("%1").arg((int)(duration % 60)));
  return (ret);
}

void    toWinPath(QString &path)
{
#ifdef	Q_OS_WIN32
    path = path.replace("/", "\\");
#else
    (void) path;
#endif
}

void	execPlaylist(const QString &player, const QString &listsKara)
{
   QProcess p;

#ifdef Q_OS_WIN32
  system(QString(
		 QString("\"") + "\"" + player + "\""
		 + listsKara + "\""
         ).toLocal8Bit().constData()
	 );
#else
  system(QString(
		 player
		 + listsKara
         ).toLocal8Bit().constData()
	 );
  exit(0);
#endif
}

bool isAlphaNum(QCharRef c)
{
  return ((c >= 'a' && c <= 'z')
	  || (c >= '0' && c <= '9')
	  || (c  == ' ' || c == '\'')
	  || (c >= 'A' && c <= 'Z'));
}


QString changeExtansion(const QString &path, const char *ext)
{
  QString ret = path;
  int len = path.length();
  int i;
  
  for (i = 0; i < 3; ++i)
    {
      ret[len - i - 1] = ext[2 - i];
    }
  return (ret);
}
