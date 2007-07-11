#include "get-path.h"

#include <pir/common/logging.h>
#include <pir/common/utils.h>

#include <Absyn.H>
#include <Skeleton.H>
#include <Printer.H>
#include <Parser.H>

#include <vector>
#include <string>

#include <iostream>

#include <boost/optional/optional.hpp>

#include <stdio.h>


using namespace std;

using boost::optional;

namespace
{
    Log::logger_t logger = Log::makeLogger ("get-path",
					    boost::none,
					    Just (Log::DEBUG));
}


optional<int> PathFinder::find (const vector<string>& path)
{
    _mode = SCALAR;
    _path = path;

    _depth = 0;
    
    visitObject (_tree);
    return _int_answer;
}

optional<vector<Value*> > PathFinder::findList (const vector<string>& path)
{
    _mode = LIST;
    _path = path;

    _depth = 0;
    
    visitObject (_tree);
    return _list_answer;
}


void PathFinder::visitNumInt (NumInt* p)
{
    clog << "*** Found leaf NumInt " << p->integer_ << endl;
    _int_answer = p->integer_;
}

void PathFinder::visitAssoc (Assoc* p)
{
    string name = p->ident_;

    PrintAbsyn printer;
	
    LOG (Log::DEBUG, logger,
	 "Visiting assoc " << name << " = " << printer.print (p->value_));

    if (_path.size() > _depth && _path[_depth] == name) {
	// correct branch, carry on
	++_depth;

	LOG (Log::DEBUG, logger,
	     "visitAssoc() recursing");

	// the Skeleton version recurses deeper into the syntax tree.
	Skeleton::visitAssoc (p);
    }
}


void PathFinder::visitListValue (ListValue * tree)
{
    clog << "Visiting a list with depth=" << _depth << endl;
    
    if (_mode == LIST && _depth == _path.size()) {
	// we have seen all the elements of the path, so this must be the right
	// list.
	
	// save the list
	_list_answer = *tree;

	_depth = 0;

	// no need to call parent and continue recursion.
	return;
    }

    Skeleton::visitListValue (tree);
}




int main (int argc, char* argv[])
{
    vector<string> path;

    for (int i=1; i < argc; i++) {
	path.push_back (argv[i]);
    }


    // we know that only an SList can come out of the parse here, so the dynamic
    // cast is OK.
    Object *parse_tree = dynamic_cast<Object*> (pObjectT (stdin));

    if (parse_tree == NULL) {
	cerr << "Error parsing" << endl;
	return 2;
    }
    
    PathFinder f (parse_tree);

    {
	optional<int> answer = f.find (path);
	if (answer) {
	    clog << "Found scalar value " << *answer << endl;
	}
	else {
	    clog << "Scalar Not found" << endl;
	}
    }

    {
	optional< vector<Value*> > answer = f.findList (path);
	if (answer) {
	    PrintAbsyn printer;
	    LOG (Log::INFO, logger,
		 "Found list value:") ;
	    for (vector<Value*>::iterator i = answer->begin();
		 i != answer->end();
		 ++i)
	    {
		LOG (Log::INFO, logger,
		     printer.print (*i));
	    }
	}
	else {
	    clog << "List Not found" << endl;
	}
    }
    

}
