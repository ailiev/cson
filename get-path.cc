#include <Absyn.H>
#include <Skeleton.H>
#include <Printer.H>
#include "Parser.H"

#include <vector>
#include <string>

#include <iostream>

#include <stdio.h>

using namespace std;


class PathFinder : public Skeleton
{
private:

    vector<string> _path;

    vector<string> _subpath;

public:

    PathFinder (const vector<string>& path)
	: _path(path)
	{}
    

    void visitNumInt (NumInt* p) {
	cout << "*** Got leaf NumInt " << p->integer_ << endl;
    }

    void visitPair (Pair* p) {
	string name = p->string_;

	PrintAbsyn printer;
	
	clog << "Visiting Pair " << p->string_ << " = " << printer.print (p->value_) << endl;

	if (_path.size() > _subpath.size() && _path[_subpath.size()] == name) {
	    // correct branch, carry on
	    _subpath.push_back (name);
	    Skeleton::visitPair (p);
	}
    }
};



int main (int argc, char* argv[])
{
    vector<string> path;

    for (int i=1; i < argc; i++) {
	path.push_back (argv[i]);
    }

    Object *parse_tree = dynamic_cast<Object*> (pObjectT (stdin));

    PathFinder f (path);

    f.visitObject (parse_tree);

}
