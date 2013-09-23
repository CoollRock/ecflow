//=============================================================================================
// Name        : 
// Author      : 
// Revision    : $Revision: #1 $ 
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


/*
** Generated by X-Designer   // edited in the lack of X-Designer
*/
/*
**LIBS: -lXm -lXt -lX11
*/

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>


#include "uijobstatus.h"

script_p jobstatus_form = (script_p) NULL;



void jobstatus_form_c::create (Widget parent, char *widget_name)
{
	Widget children[4];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget scrolledText1 = (Widget)NULL;
	Widget button2 = (Widget)NULL;
	Widget button3 = (Widget)NULL;
	Widget button1 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "jobstatus_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	jobstatus_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = jobstatus_form;
	XtSetArg(al[ac], XmNeditable, FALSE); ac++;
	XtSetArg(al[ac], XmNcursorPositionVisible, FALSE); ac++;
	XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
	text_ = XmCreateScrolledText ( jobstatus_form, "text_", al, ac );
	ac = 0;
	scrolledText1 = XtParent ( text_ );

	XtSetArg(al[ac], XmNhighlightThickness, 1); ac++;
	XtSetArg(al[ac], XmNshadowThickness, 1); ac++;
	XtSetArg(al[ac], XmNeditable, FALSE); ac++;
	XtSetArg(al[ac], XmNcursorPositionVisible, FALSE); ac++;
	name_ = XmCreateTextField ( jobstatus_form, "name_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNspacing, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	tools_ = XmCreateRowColumn ( jobstatus_form, "tools_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
	XtSetArg(al[ac], XmNdefaultButtonShadowThickness, 0); ac++;
	button2 = XmCreatePushButton ( tools_, "Use external viewer", al, ac );
	ac = 0;
	button3 = XmCreatePushButton ( tools_, "Search", al, ac );
        XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
        XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
        XtSetArg(al[ac], XmNdefaultButtonShadowThickness, 0); ac++;
        button1 = XmCreatePushButton ( tools_, "Update", al, ac );
        ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, name_); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( scrolledText1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightOffset, 0); ac++;
	XtSetArg(al[ac], XmNrightWidget, tools_); ac++;
	XtSetValues ( name_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 0); ac++;
	XtSetValues ( tools_,al, ac );
	ac = 0;
	XtManageChild(text_);
	XtAddCallback (button2, XmNactivateCallback,&jobstatus_form_c:: externalCB, (XtPointer) this);
	XtAddCallback (button3, XmNactivateCallback,&jobstatus_form_c:: searchCB, (XtPointer) this);
        XtAddCallback (button1, XmNactivateCallback,&jobstatus_form_c:: updateCB, (XtPointer) this);
	children[ac++] = button2;
	children[ac++] = button3;
	children[ac++] = button1;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = name_;
	children[ac++] = tools_;
	XtManageChildren(children, ac);
	ac = 0;
}

void jobstatus_form_c::searchCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	jobstatus_form_p instance = (jobstatus_form_p) client_data;
	instance->searchCB ( widget, call_data );
}

void jobstatus_form_c::externalCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	jobstatus_form_p instance = (jobstatus_form_p) client_data;
	instance->externalCB ( widget, call_data );
}

void jobstatus_form_c::updateCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
        jobstatus_form_p instance = (jobstatus_form_p) client_data;
        instance->updateCB ( widget, call_data );
}

