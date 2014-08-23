#include "FileTree.hpp"
#include <stack>

using namespace std;

FileTree::FileTree(libtorrent::file_storage files, shared_ptr<gt::Torrent> tor) : fs(files), t(tor), index(0)
{
	// TEST
	std::cout << "Printing file_storage..." << std::endl;
	std::cout << " Total files: " << files.num_files() << std::endl;
	for (int i = 0; i < files.num_files(); ++i)
		std::cout << "  " << i << ": " << files.at(i).path << std::endl;
}

FileTree::FileTree(FileTree *Parent) : parent(Parent), fs(Parent->fs), t(Parent->t), index(Parent->index)
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

void FileTree::print(FileTree &tree)
{
	for(auto i : tree.children)
	{
		std::cout << i.first << std::endl;
		print(*i.second);
	}
}

void FileTree::print()
{
	print(*this);
}

void FileTree::add(string &str, int i)
{
	if(str.find('/') != string::npos) // if true then the file is nested inside a folder
	{
		string dir_name = str.substr(0, str.find('/')); // so we get the name of that folder
		if(children.find(dir_name) == children.end())   // and check if the folder exist
		{
			children[dir_name] = new FileTree(this);//if the folder doesn't exist, we add it to our tree, explicitly scpecifying the parent
			children[dir_name]->filename = dir_name;
			is_dir = true;
		}
		string file = str.substr(str.find('/') + 1); // we add the file, which may be nested deeper inside folders, but it's handled recursively
		children[dir_name]->add(file, i);
	}
	else //we reached the file itself
	{
		is_dir = false;
		children[str] = new FileTree(this);
		children[str]->index = i;
		children[str]->filename = str;
	}
}
