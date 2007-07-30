#include "get-path.h"

#include <pir/common/logging.h>
#include <pir/common/utils.h>

#include <json/bnfc/Absyn.H>
#include <json/bnfc/Printer.H>
#include <json/bnfc/Parser.H>

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
		print_seq (cout, *i);
		cout << endl;
	    }
	}
	else {			// !answer
	    LOG (Log::INFO, logger, "List Not found");
	}
    }
    

}
