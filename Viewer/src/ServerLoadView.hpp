//============================================================================
// Copyright 2009-2018 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef SERVERLOADVIEW_HPP
#define SERVERLOADVIEW_HPP

#include <QDateTime>
#include <QMap>
#include <QStringList>
#include <QVector>
#include <QWidget>

#include <QtCharts>
using namespace QtCharts;

#if 0

class LogItem
{
public:
    enum Type {MsgType,ErrorType};
    virtual Type type()=0;

    size_t time_;
    std::string txt_;
};

class MsgLogItem : public LogItem
{
public:
    Type type() {return MsgType;}
};

class ErrorLogItem : public LogItem
{
public:
    Type type() {return ErrorType;}
};

#endif

class ServerLoadData
{
public:
    ServerLoadData() {}

    //enum Type {ChildRequestType,UserRequestType
    enum TimeRes {SecondRes, MinuteRes};

    void loadLogFile(const std::string& logFile);
    void getChildReq(QLineSeries& series,TimeRes);
    void getUserReq(QLineSeries& series,TimeRes);
    void getTotalReq(QLineSeries& series,TimeRes);
    void getSuiteReq(QString suiteName,QLineSeries& series,TimeRes);

protected:
    QVector<QDateTime> time_;
    QVector<size_t> childReqPerSec_;
    QVector<size_t> userReqPerSec_;
    QStringList suiteTmp_;
    QMap<int,QVector<size_t> > suite_;
    QStringList suiteNames_;

private:
    struct SuiteLoad {
       SuiteLoad(const std::string& name) : suite_name_(name),  request_per_second_(1), total_request_per_second_(1) {}

       std::string suite_name_;
       size_t      request_per_second_;
       size_t      total_request_per_second_;
    };

    void clear();
    void add(std::vector<std::string> time_stamp,size_t child_requests_per_second,
             size_t user_request_per_second,std::vector<SuiteLoad>& suite_vec);

    void collate(const std::vector<SuiteLoad>& suite_vec);

    bool extract_suite_path(const std::string& line,bool child_cmd,std::vector<SuiteLoad>& suite_vec,
                            size_t& column_index);
};

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent);

    void doZoom(QRectF);

Q_SIGNALS:
    void chartZoomed(QRectF);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void adjustTimeAxis(qint64 periodInMs);
};

class ServerLoadView : public QWidget
{
    Q_OBJECT
public:
    explicit ServerLoadView(QWidget* parent=0);

    void load(const std::string& logFile);

protected Q_SLOTS:
    void slotZoom(QRectF);

protected:
    void build(QChart* chart,QLineSeries *series);

    QChart* chart_;
    ChartView* chartView_;
    QChart* chartUserReq_;
    QChart* chartChildReq_;
    QList<ChartView*> views_;

    ServerLoadData data_;
};

#endif // SERVERLOADVIEW_HPP
