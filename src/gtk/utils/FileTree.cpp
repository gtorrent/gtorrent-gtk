#include "FileTree.hpp"
#include <stack>

using namespace std;

FileTree::FileTree(libtorrent::file_storage files, shared_ptr<gt::Torrent> tor) : fs(files), t(tor), index(0)
{
}

FileTree::FileTree(FileTree *Parent) : parent(Parent), fs(Parent->fs), t(Parent->t)
{
}

FileTree::~FileTree()
{
	for(auto i : children) //DAT recursive destructor
		delete i.second;
}

string FileTree::fullname()
{
	string ret;
	stack<string> st;
	FileTree *tmp = parent;
	while(tmp)
	{
		st.push(tmp->filename);
		tmp = tmp->parent;
	}

	while(!st.empty())
	{
		ret += st.top() + '/';
		st.pop();
	}

	ret += filename;
	return ret;
}

void FileTree::add(string &str)
{
	if(str.find('/') != string::npos) // if true then the file is nested inside a folder
	{
		string dir = str.substr(0, str.find('/')); // so we get the name of that folder
		if(children.find(dir) == children.end())   // and check if the folder exist
		{
			children[dir] = new FileTree(this);//if the folder doesn't exist, we add it to our tree, explicitly scpecifying the parent
			children[dir]->filename = dir;
		}
		string file = str.substr(str.find('/') + 1); // we add the file, which may be nested deeper inside folders, but it's handled recursively
		children[dir]->add(file);
	}
	else //we reached the file itself
	{
		children[str] = new FileTree(this);
		children[str]->index = index++;
		children[str]->filename = str;
	}
}
