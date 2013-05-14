#ifndef	CORE_HH
#define	CORE_HH

#include	<stdio.h>
#include	<string>
#include	<iostream>
#include	<QString>

#ifdef	WIN32
#define	SLASH	'\\'
#else
#define	SLASH	'/'
#endif

#define	VLC	0
#define	MPLAYER	1

template<int i = MPLAYER>
QString  getPlayerCmd();

template<>
QString  getPlayerCmd<VLC>();

void	 initRand();

const	QString durationToString(int64_t duration);

void    toWinPath(QString &path);

void	execPlaylist(const QString &player, const QString &listKara);

bool    isAlphaNum(QCharRef c);

#endif
