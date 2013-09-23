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
#include <Xm/FileSB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/ScrollBar.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>


#include "uifsb.h"

fsb_shell_p fsb_shell = (fsb_shell_p) NULL;



void fsb_shell_c::create (Widget parent, char *widget_name)
{
	Widget children[14];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget button42 = (Widget)NULL;
	Widget button44 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "fsb_shell";

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	fsb_shell = XmCreateDialogShell ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = fsb_shell;
	XtSetArg(al[ac], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); ac++;
	form_ = XmCreateFileSelectionBox ( fsb_shell, "form_", al, ac );
	ac = 0;
	button42 = XmSelectionBoxGetChild ( form_, XmDIALOG_CANCEL_BUTTON );
	button44 = XmSelectionBoxGetChild ( form_, XmDIALOG_OK_BUTTON );


	label_ = XmCreateLabel ( form_, "label_", al, ac );
	XtAddCallback (form_, XmNokCallback,&fsb_shell_c:: okCB, (XtPointer) this);
	XtAddCallback (form_, XmNcancelCallback,&fsb_shell_c:: cancelCB, (XtPointer) this);
	XtAddCallback (form_, XmNhelpCallback,&fsb_shell_c:: helpCB, (XtPointer) this);
	children[ac++] = label_;
	XtManageChildren(children, ac);
	ac = 0;
}

void fsb_shell_c::helpCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	fsb_shell_p instance = (fsb_shell_p) client_data;
	instance->helpCB ( widget, call_data );
}

void fsb_shell_c::cancelCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	fsb_shell_p instance = (fsb_shell_p) client_data;
	instance->cancelCB ( widget, call_data );
}

void fsb_shell_c::okCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	fsb_shell_p instance = (fsb_shell_p) client_data;
	instance->okCB ( widget, call_data );
}

