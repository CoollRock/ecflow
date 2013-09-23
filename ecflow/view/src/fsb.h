#ifndef fsb_H
#define fsb_H

//=============================================================================================
// Name        : 
// Author      : 
// Revision    : $Revision: #4 $ 
//
// Copyright 2009-2012 ECMWF. 
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description : 
//=============================================================================================

#ifdef NO_BOOL
#include "bool.h"
#endif

#include "uifsb.h"
#include "dialog.h"

class fsb : public dialog<fsb,fsb_shell_c> {
public:

// -- Exceptions
	// None

// -- Contructors

	fsb();

// -- Destructor

	~fsb(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods

	static const char* ask(const char* title, 
		const char* file   = 0,
		const char* filter = 0,
		const char* dir    = 0);

	// Uncomment for persistent, remove otherwise
	// static os_typespec* get_os_typespec();

protected:

// -- Members
	// None

// -- Methods
	
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	fsb(const fsb&);
	fsb& operator=(const fsb&);


	char* file_;

// -- Methods

	void set(const char*,const char*);

	const char* choose(const char*,const char*,
		const char*,const char*);


// -- Overridden methods

	virtual void okCB( Widget, XtPointer ) ;

// -- Class members


// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const fsb& p)
	//	{ p.print(s); return s; }

};

inline void destroy(fsb**) {}

// If persistent, uncomment, otherwise remove
//#ifdef _ODI_OSSG_
//OS_MARK_SCHEMA_TYPE(fsb);
//#endif

#endif
