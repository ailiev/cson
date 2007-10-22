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

#include <pir/common/logging.h>
#include <pir/common/utils.h>
#include <pir/common/function-objs.h>
#include <pir/common/exceptions.h>


#include "bnfc/Absyn.H"
#include "bnfc/Skeleton.H"
#include "bnfc/Parser.H"
// for logging
#include "bnfc/Printer.H"

#include <boost/optional/optional.hpp>

#include <vector>
#include <string>

#include <cstdio>




using namespace std;

using boost::optional;

namespace
{
    Log::logger_t logger = Log::makeLogger ("json.get-path");
}

// construct from a stream
PathFinder::PathFinder (std::FILE * fh)
    throw (parse_exception)
{
    // we know that only an TopLevel can come out of the parse here, so the
    // dynamic cast is OK.
    TopLevel *parse_tree = dynamic_cast<TopLevel*> (pTopLevelT (fh));

    if (parse_tree == NULL) {
	throw parse_exception ("PathFinder could not parse any "
			       "Json-C objects from its input file stream");
    }
    
    // duplicate what the other cstr does

    // take ownership
    _tree = auto_ptr<TopLevel> (parse_tree);
    _depth = 0;
}




optional<int> PathFinder::find (const vector<string>& path)
{
    _mode = SCALAR;
    _path = path;

    _depth = 0;

    _int_answer = boost::none;
    
    visitTopLevel (_tree.get());
    return _int_answer;
}

optional<vector<Value*> > PathFinder::findList (const vector<string>& path)
{
    _mode = LIST;
    _path = path;

    _depth = 0;

    _list_answer = boost::none;
    
    visitTopLevel (_tree.get());
    return _list_answer;
}


vector< vector<int> > PathFinder::squashList
(const std::vector<Value*> & vals)
{

    vector< vector<int> > answer (vals.size());
    
    std::transform (vals.begin(), vals.end(),
		    answer.begin(), squashValue);

    return answer;
}


// static
std::vector<int> PathFinder::squashValue (Value* vl)
{
    // do an in-order walk to get a list of all the terminal numeric values.
    // throws bad_arg_exception if a list is found in the values of this list.

    InOrderCollector coll (vl);
    return coll.collectValues();
}


void PathFinder::visitNumInt (NumInt* p)
{
    if (_mode == SCALAR) {
	if (_depth == _path.size()) {
	    LOG (Log::DEBUG, logger, "Found leaf NumInt " << p->integer_);
	    _int_answer = p->integer_;
	}
	else {
	    // reduce depth. TODO: not sure this suffices to solve the FIXME on
	    // visitAssoc().
	    --_depth;
	}
    }
}

// FIXME: this gets called for assocs which are peers in a list, as well as
// descendants in the syntax tree. thus can end up retrieving peer values in a
// structure.
void PathFinder::visitAssoc (Assoc* p)
{
    string name = p->ident_;

    // FIXME: the following would crash (SEGV) sometimes.
//     PrintAbsyn printer;
//     LOG (Log::DEBUG, logger,
// 	 "Visiting assoc " << name << " = " << printer.print (p->value_));

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
    LOG (Log::DEBUG, logger,
	 "Visiting a list with depth=" << _depth);
    
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


PathFinder::~PathFinder ()
{
    // _tree is auto-deleted, so nothing to do here.
}



void PathFinder::delete_found_list ()
{
    // not necessary, as the list only has pointers into _tree, which is deleted
    // once by its auto_ptr.
    if (_list_answer) {
	for_each (_list_answer->begin(), _list_answer->end(),
		  deleter<Value>());
    }
}



//
//
// class InOrderCollector
//
//

void InOrderCollector::visitNumInt (NumInt* p)
{
    LOG (Log::DEBUG, logger,
	 "collecting int " << p->integer_);
    
    _vals.push_back (p->integer_);
}

void InOrderCollector::visitListValue (ListValue *)
{
    throw bad_arg_exception
	("Cannot have a nested list as input to "
	 "Faerieplay circuit machine");
}
 
void InOrderCollector::visitValue (Value * v)
{
    // rather awkward dispatch here.
    if (typeid(*v) == typeid(ObjectVal)) {
	visitObjectVal (dynamic_cast<ObjectVal*>(v));
    }
    else if (typeid(*v) == typeid(NumVal)) {
	visitNumVal (dynamic_cast<NumVal*> (v));
    }
    else if (typeid(*v) == typeid(SListVal)) {
	visitSListVal (dynamic_cast<SListVal*> (v));
    }    
    else if (typeid(*v) == typeid(LitVal)) {
	visitLitVal (dynamic_cast<LitVal*>(v));
    }
    else if (typeid(*v) == typeid(StringVal)) {
	visitStringVal (dynamic_cast<StringVal*>(v));
    }
    else {
	throw runtime_exception ((string) "Unexpected type in visitValue(): "
				 + typeid(*v).name());
    }
}
