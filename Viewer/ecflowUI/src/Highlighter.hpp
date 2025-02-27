//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef HIGHLIGHTER_HPP_
#define HIGHLIGHTER_HPP_

#include <QtGlobal>
#include <string>
#include <QSyntaxHighlighter>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#include <QRegExp>
#endif
#include <QRegularExpression>

class Highlighter : public QSyntaxHighlighter
{
public:
	Highlighter(QTextDocument *parent,QString id);
    static void init(const std::string& parFile);
    void toHtml(QString& html);

protected:
	void highlightBlock(const QString &text) override;
	void addRule(QString,QTextCharFormat);

private:
	void load(QString);

    struct HighlightingRule
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QRegularExpression pattern;
#else
        QRegExp pattern;
#endif
        QTextCharFormat format;
    };

	QList<HighlightingRule> rules_;
	static std::string parFile_;
};

#endif
