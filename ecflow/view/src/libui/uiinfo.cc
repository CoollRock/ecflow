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
#include <Xm/DrawnB.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>

#include "Hyper.h"
#define XmCreateDrawnButton CreateHyper
#include "uiinfo.h"

extern void info_pop_callback(Widget, XtPointer, XtPointer) ;
extern void shell_popup_callback(Widget, XtPointer, XtPointer) ;
info_form_p info_form = (info_form_p) NULL;



void info_form_c::create (Widget parent, char *widget_name)
{
	Widget children[3];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget scrolledWin1 = (Widget)NULL;
	Widget scrollbar25 = (Widget)NULL;
	Widget scrollbar26 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "info_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	info_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = info_form;
	XtSetArg(al[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;
	scrolledWin1 = XmCreateScrolledWindow ( info_form, "scrolledWin1", al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNhorizontalScrollBar, &scrollbar25 ); ac++;
	XtSetArg(al[ac], XmNverticalScrollBar, &scrollbar26 ); ac++;
	XtGetValues(scrolledWin1, al, ac );
	ac = 0;
	text_ = XmCreateDrawnButton ( scrolledWin1, "text_", al, ac );

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 5); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 5); ac++;
	XtSetValues ( scrolledWin1,al, ac );
	ac = 0;
	XtAddCallback (text_, XmNactivateCallback,&info_form_c:: hyperCB, (XtPointer) this);
	children[ac++] = text_;
	XtManageChildren(children, ac);
	ac = 0;
	XmScrolledWindowSetAreas(scrolledWin1, scrollbar25, scrollbar26, text_ );
	children[ac++] = scrolledWin1;
	XtManageChildren(children, ac);
	ac = 0;
}

void info_form_c::hyperCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	info_form_p instance = (info_form_p) client_data;
	instance->hyperCB ( widget, call_data );
}

