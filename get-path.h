// -*- c++ -*-
/*
Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

This product includes cryptographic software written by Eric Young
(eay@cryptsoft.com)

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _GET_PATH_H
#define _GET_PATH_H

#include <json/bnfc/Json/Absyn.H>
#include <json/bnfc/Json/Skeleton.H>

#include <faerieplay/common/exceptions.h>

#include <vector>
#include <string>
#include <memory>

#include <iostream>

#include <boost/optional/optional.hpp>

#include <cstdio>


/// Class to extract values from a data tree, like a simple Xpath.
class PathFinder : public Skeleton
{
private:

    enum search_mode_t
    {
	SCALAR,
	LIST
    };

    std::auto_ptr<TopLevel> _tree;
    
    std::vector<std::string> _path;
    
    
    search_mode_t _mode;
    size_t _depth;

    boost::optional<int> _int_answer;
    boost::optional<std::vector<Value* > > _list_answer;

    
    
public:

    /// Create with a parsed data tree, and take ownership of the pointer.
    /// \see pTopLevelT
    PathFinder (std::auto_ptr<TopLevel> tree)
	: _tree	    (tree),
	  _depth    (0)
	{}

    /// Construct from a stream.
    /// @throws parse_exception if input stream could not be parsed.
    PathFinder (std::FILE* is) throw (parse_exception);
    

    /// Find a scalar integer.
    boost::optional<int> find (const std::vector<std::string>& path);

    /// Find a list.
    boost::optional<std::vector<Value* > >
    findList (const std::vector<std::string>& path);


    /// Squash each element of a list into a list of int. Answer intended to
    /// be used by the Faerieplay runtime to initialize an input array.
    static std::vector< std::vector<int> >
    squashList (const std::vector<Value*> &);

    ~PathFinder ();

    
private:

    void visitNumInt (NumInt* p);

    void visitAssoc (Assoc* p);

    void visitListValue (ListValue * tree);

    static std::vector<int> squashValue (Value*);

    // trying to avoid leaking those Value*'s in the list.
    void delete_found_list ();
    
};




/// \internal Quick visitor class to collect the terminal values in a tree.

class InOrderCollector : public Skeleton
{
private:
    std::vector<int> _vals;

    Value * _v;
    
public:
    /// Does not modify the Value
    InOrderCollector (Value * v)
	: _v	    (v)
	{}


    std::vector<int> collectValues ()
	{
	    visitValue (_v);
	    return _vals;
	}
    
    
    void visitNumInt (NumInt* p);

    void visitValue (Value * v);

    void visitListValue (ListValue * tree);
};


#endif // _GET_PATH_H
