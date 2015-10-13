//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef PROPERTYEDITOR_INC_
#define PROPERTYEDITOR_INC_

#include <QWidget>

#include "ui_PropertyEditor.h"

class QGridLayout;
class QTabWidget;

class PropertyLine;
class VProperty;

class PropertyEditor : public QWidget, protected Ui::PropertyEditor
{
Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent=0);
    ~PropertyEditor();

    void edit(VProperty*,QPixmap pixmap);
    void edit(VProperty*,QString label);
    bool applyChange();
    VProperty* property() const {return group_;}

Q_SIGNALS:
	void changed();

private:
    void clear();
    void build();
    void addHelpers();

    void addItem(VProperty*,QVBoxLayout*);
    PropertyLine* addLine(VProperty* vProp,QGridLayout* grid);
    void addGroup(VProperty*,QVBoxLayout*);
    void addGrid(VProperty*,QVBoxLayout*);
    void addGridRow(VProperty* prop,QGridLayout *grid);
    void addNotification(VProperty* prop,QVBoxLayout*);
    void addTabs(VProperty*,QVBoxLayout*);
    void addTab(VProperty*,QTabWidget*);
	void addNote(VProperty* vProp,QVBoxLayout*);
	void addNote(VProperty* vProp,QGridLayout* layout);

    VProperty* group_;
    QGridLayout* currentGrid_;
    QList<PropertyLine*> lineItems_;
    QString serverName_;
};


#endif

