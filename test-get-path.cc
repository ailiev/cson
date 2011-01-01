/*
 * Simple library to parse and access a JSON-like mini-language for representing
 * structures.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
*/


#include "get-path.h"

#include <faerieplay/common/logging.h>
#include <faerieplay/common/utils.h>

#include <json/bnfc/Json/Absyn.H>
#include <json/bnfc/Json/Printer.H>
#include <json/bnfc/Json/Parser.H>

#include <boost/optional/optional.hpp>

#include <stdio.h>

#include <vector>
#include <string>

#include <algorithm>

#include <iostream>


using namespace std;

using boost::optional;

namespace
{
    Log::logger_t logger = Log::makeLogger ("json.get-path.test");
}




// test driver for the lookup functions.
// params: an element path, eg: "a b c" for path a/b/c
// the data file to be provided on stdin.
int main (int argc, char* argv[])
{
    vector<string> path;

    if (argc < 2) {
	clog << "Usage: " << argv[0] << " <space-separated path>" << endl
	     << "Provide data file on stdin" << endl;
	exit(0);
    }
    
    for (int i=1; i < argc; i++) {
	path.push_back (argv[i]);
    }


    try {
	PathFinder f (stdin);

	{
	    optional<int> answer = f.find (path);
	    if (answer) {
		cout << "scalar:" << *answer << endl;
	    }
	    else {
		LOG (Log::INFO, logger, "Scalar Not found");
	    }
	}

	{
	    optional< vector<Value*> > answer = f.findList (path);
	    if (answer) {
		PrintAbsyn printer;
		LOG (Log::INFO, logger,
		     "Found list value, printing Value's:") ;
		for (vector<Value*>::iterator i = answer->begin();
		     i != answer->end();
		     ++i)
		{
		    LOG (Log::INFO, logger,
			 printer.print (*i));
		}

		LOG (Log::INFO, logger,
		     "Squashing the values and printing the squashed lists");
		cout << "list:" << endl;
		vector<vector<int> > vals = PathFinder::squashList (*answer);
	    
		for (vector<vector<int> >::iterator i = vals.begin();
		     i != vals.end();
		     i++)
		{
		    cout << print_seq(*i);
		    cout << endl;
		}
	    }
	    else {			// !answer
		LOG (Log::INFO, logger, "List Not found");
	    }
	}

    }
    catch (const parse_exception& ex) {
	cerr << ex.what() << endl
	     << "Exiting" << endl;
    }
    

}
