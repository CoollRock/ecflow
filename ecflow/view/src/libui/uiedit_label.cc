/*
** Generated by X-Designer
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
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>


#include "uiedit_label.h"

edit_label_p edit_label_form = (edit_label_p) NULL;



void edit_label_form_c::create (Widget parent, char *widget_name)
{
	Widget children[3];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget button1 = (Widget)NULL;
	Widget form1 = (Widget)NULL;
	Widget frame1 = (Widget)NULL;
	Widget scrolledText1 = (Widget)NULL;
	Widget label1 = (Widget)NULL;
	Widget frame2 = (Widget)NULL;
	Widget scrolledText2 = (Widget)NULL;
	Widget label2 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "edit_label_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	edit_label_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = edit_label_form;
	tools_ = XmCreateRowColumn ( edit_label_form, "tools_", al, ac );
	button1 = XmCreatePushButton ( tools_, "Apply", al, ac );
	form1 = XmCreateForm ( edit_label_form, "form1", al, ac );
	frame1 = XmCreateFrame ( form1, "frame1", al, ac );
	XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
	value_ = XmCreateScrolledText ( frame1, "value_", al, ac );
	ac = 0;
	scrolledText1 = XtParent ( value_ );

	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	label1 = XmCreateLabel ( frame1, "Value:", al, ac );
	ac = 0;
	frame2 = XmCreateFrame ( form1, "frame2", al, ac );
	XtSetArg(al[ac], XmNsensitive, FALSE); ac++;
	XtSetArg(al[ac], XmNeditable, FALSE); ac++;
	XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
	default_ = XmCreateScrolledText ( frame2, "default_", al, ac );
	ac = 0;
	scrolledText2 = XtParent ( default_ );

	XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
	label2 = XmCreateLabel ( frame2, "Default:", al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( tools_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, tools_); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( form1,al, ac );
	ac = 0;
	XtAddCallback (button1, XmNactivateCallback,&edit_label_form_c:: applyCB, (XtPointer) this);
	children[ac++] = button1;
	XtManageChildren(children, ac);
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 50); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( frame1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 50); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( frame2,al, ac );
	ac = 0;
	XtAddCallback (value_, XmNvalueChangedCallback,&edit_label_form_c:: changedCB, (XtPointer) this);
	XtManageChild(value_);
	children[ac++] = label1;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (default_, XmNvalueChangedCallback,&edit_label_form_c:: changedCB, (XtPointer) this);
	XtManageChild(default_);
	children[ac++] = label2;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = frame1;
	children[ac++] = frame2;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = tools_;
	children[ac++] = form1;
	XtManageChildren(children, ac);
	ac = 0;
}

void edit_label_form_c::changedCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_label_form_p instance = (edit_label_form_p) client_data;
	instance->changedCB ( widget, call_data );
}

void edit_label_form_c::applyCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_label_form_p instance = (edit_label_form_p) client_data;
	instance->applyCB ( widget, call_data );
}

