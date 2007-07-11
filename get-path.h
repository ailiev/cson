// -*- c++ -*-

#include <Absyn.H>
#include <Skeleton.H>
#include <Printer.H>
#include "Parser.H"

#include <vector>
#include <string>

#include <iostream>

#include <boost/optional/optional.hpp>

#include <stdio.h>




class PathFinder : public Skeleton
{
private:

    enum search_mode_t
    {
	SCALAR,
	LIST
    };

    Object * _tree;
    
    std::vector<std::string> _path;
    
    
    search_mode_t _mode;
    size_t _depth;

    bool _found;
    
    boost::optional<int> _int_answer;
    boost::optional<std::vector<Value*> > _list_answer;

    
    
public:

    PathFinder (Object * tree)
	: _tree	    (tree),
	  _depth    (0),
	  _found    (false)
	{}
    

    boost::optional<int> find (const std::vector<std::string>& path);
    boost::optional<std::vector<Value*> > findList (const std::vector<std::string>& path);


    
    void visitNumInt (NumInt* p);

    void visitAssoc (Assoc* p);

    void visitListValue (ListValue * tree);
};



