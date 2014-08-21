#pragma once

#include <map>
#include <string>
#include <memory>

#include <libtorrent/file_storage.hpp>
#include <gtorrent/Torrent.hpp>

// It's safe to assume alphabetical order, so we can mix the
// files in the hierarchy with various torrent information on the files
// and the torrent state

class FileTree
{
public:
	FileTree *parent = nullptr;
	libtorrent::file_storage fs;
	std::shared_ptr<gt::Torrent> t;
	int index;
	std::string filename;
	std::string fullname();
	std::map<std::string, FileTree*> children;
	void add(std::string &str);
	FileTree(libtorrent::file_storage files, std::shared_ptr<gt::Torrent> tor);
	~FileTree();
	FileTree(FileTree *Parent);
};
