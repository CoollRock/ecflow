/***************************** LICENSE START ***********************************

 Copyright 2009-2020 ECMWF and INPE. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef TREENODEWIDGET_HPP_
#define TREENODEWIDGET_HPP_

#include "ui_TreeNodeWidget.h"

#include "NodeWidget.hpp"
#include "VProperty.hpp"

class AttributeFilter;
class NodeStateFilter;
class ServerFilter;
class VParamFilterMenu;
class VSettings;
class VTreeServer;
class TreeNodeWidget : public NodeWidget, public VPropertyObserver, protected Ui::TreeNodeWidget
{
Q_OBJECT

public:
	TreeNodeWidget(ServerFilter*,QWidget* parent=nullptr);
	~TreeNodeWidget() override;

	void populateDockTitleBar(DashboardDockTitleWidget* tw) override;

	void rerender() override;
    bool initialSelectionInView() override;
    void writeSettings(VComboSettings*) override;
    void readSettings(VComboSettings*) override;

    void notifyChange(VProperty*) override;

protected Q_SLOTS:
	void on_actionBreadcrumbs_triggered(bool b);
	void slotSelectionChangedInView(VInfo_ptr info);
	void slotAttsChanged();
    void firstScanEnded(const VTreeServer*);

protected:
    enum ViewLayoutMode {StandardLayoutMode,CompactLayoutMode};

    void initAtts();
    void detachedChanged() override {}
    void setViewLayoutMode(ViewLayoutMode);

	VParamFilterMenu *stateFilterMenu_;
	VParamFilterMenu *attrFilterMenu_;
	VParamFilterMenu *iconFilterMenu_;

    ViewLayoutMode viewLayoutMode_;
    VProperty* layoutProp_;

	static AttributeFilter* lastAtts_;

    std::string firstSelectionPath_;
};

#endif
