// albert - a simple application launcher for linux
// Copyright (C) 2014 Manuel Schneider
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "albertengine.h"
#include <QSettings>
#include <QDir>
#include <algorithm>

//REMOVE
#include <string>
#include <iostream>
#include <functional>

using std::string;

bool lexicographically (Items::AbstractItem*  i, Items::AbstractItem* j)
{
	return 0 > i->name().compare(j->name(), Qt::CaseInsensitive);
}

//bool sortByName(const Items::AbstractItem &lhs, const Items::AbstractItem &rhs) { return lhs.name() < rhs.name(); }


/**********************************************************************//**
 * @brief AlbertEngine::AlbertEngine
 * @param parent
 */
AlbertEngine::AlbertEngine(QObject *parent) :
	QObject(parent)
{
}

/**********************************************************************//**
 * @brief AlbertEngine::~AlbertEngine
 */
AlbertEngine::~AlbertEngine()
{
	for (auto it = _index.begin(); it != _index.end(); ++it)
		delete (*it);
}

/**********************************************************************//**
 * @brief AlbertEngine::buildIndex
 */
void AlbertEngine::buildIndex()
{

	QSettings conf;
	std::cout << "Config: " << conf.fileName().toStdString() << std::endl;
	QStringList paths = conf.value("paths").toStringList();

	// Define a lambda for recursion
	std::function<void(const QString& p)> rec_dirsearch = [&] (const QString& p) {
		QDir dir(p);
		dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
		_index.push_back(new Items::Directory(dir.dirName(), dir.canonicalPath()));

		// go recursive into subdirs
		QFileInfoList list = dir.entryInfoList();
		for ( QFileInfo &fi : list)
		{
			if (fi.isDir()){
				rec_dirsearch(fi.absoluteFilePath());
				std::cout << "Entry(dir): " << fi.absoluteFilePath().toStdString() << std::endl;
			}
			if (fi.isFile()) {
				if (fi.isExecutable()){
					_index.push_back(new Items::Executable(fi.baseName(), fi.absoluteFilePath()));
					std::cout << "Entry(exe): " << fi.absoluteFilePath().toStdString() << std::endl;
				}
				if (fi.suffix() == "deskop"){
					_index.push_back(new Items::DesktopApp(fi.baseName(), fi.absoluteFilePath()));
					std::cout << "Entry(desktop): " << fi.absoluteFilePath().toStdString() << std::endl;
				}
			}
			// Else is document
				// TODO Check with libmagic
		}
	};

	// Finally do this recursion for all paths
	std::cout << "pathsize: " << paths.size() << std::endl;
	for ( auto path : paths)
		rec_dirsearch(path);


//	std::sort(_index.begin(), _index.end(), lexicographically);
//	std::cout << "Size: " << _index.size() << std::endl;
//	for ( auto i : _index)
//		std::cout << i->name().toStdString() << std::endl;




}


/**********************************************************************//**
 * @brief AlbertEngine::loadIndex
 */
void AlbertEngine::loadIndex()
{

}

/**********************************************************************//**
 * @brief AlbertEngine::saveIndex
 */
void AlbertEngine::saveIndex()
{

}

/**********************************************************************//**
 * @brief AlbertEngine::saveIndex
 */
std::vector<Items::AbstractItem *> AlbertEngine::request(QString req)
{
	QString reqlo = req.toLower();
	std::vector<Items::AbstractItem *>::const_iterator it, first, last, ub, lb;
	std::iterator_traits<std::vector<Items::AbstractItem *>::const_iterator>::difference_type count, step;

	// lower bound
	first = _index.cbegin();
	last = _index.cend();
	count = distance(first,last);
	while (count>0) {
		it = first; step=count/2; advance (it,step);
		std::cout << "it:  " << (*it)->name().toStdString() << std::endl;
		if (reqlo.toStdString().compare(0, reqlo.size(), (*it)->name().toLower().toStdString())>0) {
			first=++it; count-=step+1;
		}
		else
			count=step;
	}
	lb = it;

	// upper bound
	first = _index.cbegin();
	last = _index.cend();
	count = distance(first,last);
	while (count>0) {
		it = first;
		step=count/2;
		advance (it,step);

		std::cout << "it:  " << (*it)->name().toStdString() << std::endl;
		if (!(reqlo.toStdString().compare(0, reqlo.size(), (*it)->name().toLower().toStdString())>0)) {
			first=++it;
			count-=step+1;
		}
		else
			count=step;
	}
	ub = it;

	std::cout << "req: " << req.toStdString() << std::endl;
	std::cout << "lb: " << (*lb)->name().toStdString() << std::endl;
	std::cout << "ub: " << (*--ub)->name().toStdString() << std::endl;

	return std::vector<Items::AbstractItem *>();
}