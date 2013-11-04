#include	<QFileInfo>
#include	"core.hh"
#include	"Media.hh"

/*
  int64_t duration;
  int	frameRate;
*/

Media::Media(const Media &other)
  : QListWidgetItem(QFileInfo(other._path).completeBaseName()),
    _path(other._path),
    _duration(other._duration),
    _fps(other._fps)
{
}

Media::Media(const QString &dirPath, const QString &name)
  : QListWidgetItem(QFileInfo(name).completeBaseName()), _path(dirPath + name), _duration(-1), _fps(0)
{
  //std::cout << _path.toLocal8Bit().constData() << std::endl;
}

Media::Media(const QString &path)
  : QListWidgetItem(QFileInfo(path).completeBaseName()), _path(path), _duration(-1), _fps(0)
{
}

const QString &Media::getPath() const
{
  return (_path);
}

const QString Media::getName() const
{
  return (static_cast<const QListWidgetItem *>(this)->text());
}

int	Media::getDuration(void) const
{
  return (_duration);
}

void	Media::setDuration(int64_t duration)
{
  _duration = duration;
}

float	Media::getFps(void) const
{
  return (_fps);
}

void	Media::setFps(float fps)
{
  _fps = fps;
}
