//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef TRIGGERGRAPHWIDGET_HPP
#define TRIGGERGRAPHWIDGET_HPP

#include "VInfo.hpp"
#include "Aspect.hpp"
#include "TriggerCollector.hpp"

#include <QWidget>

class TriggerTableItem;
class TriggerTableCollector;
class TriggerItemWidget;

class VComboSettings;

namespace Ui {
    class TriggerGraphWidget;
}

class TriggerGraphWidget;


class TriggerGraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TriggerGraphWidget(QWidget *parent = nullptr);
    ~TriggerGraphWidget() override;

    void clear();
    void setInfo(VInfo_ptr info, bool dependency);
    VInfo_ptr info() const {return info_;}
    bool dependency() const {return dependency_;}
    void adjust(VInfo_ptr info, bool dependency, TriggerTableCollector* triggerTc1, TriggerTableCollector* tc2);
    void setTriggerCollector(TriggerTableCollector *tc1,TriggerTableCollector *tc2);
    void beginTriggerUpdate();
    void endTriggerUpdate();
    void nodeChanged(const VNode* node, const std::vector<ecf::Aspect::Type>& aspect);
    void scan();
    void setTriggeredScanner(TriggeredScanner*);
    //void scan(VNode*);

Q_SIGNALS:
    void infoPanelCommand(VInfo_ptr,QString);
    void dashboardCommand(VInfo_ptr,QString);

private:
    Ui::TriggerGraphWidget* ui_;

private:
    VInfo_ptr info_;
    bool dependency_ {false};
    TriggerTableCollector* triggerTc_ {nullptr};
    TriggerTableCollector* triggeredTc_ {nullptr};
    VInfo_ptr lastSelectedItem_;
    QString depLabelText_;
};

#endif // TRIGGERGRAPHWIDGET_HPP
