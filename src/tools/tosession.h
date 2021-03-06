
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2013 Numerous Other Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file COPYING.txt; if not, please see
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef TOSESSION_H
#define TOSESSION_H

#include "widgets/totoolwidget.h"
#include "ui_tosessionsetupui.h"
#include "ui_tosessiondisconnectdlgui.h"
#include "core/tosettingtab.h"
#include "tools/toresultlong.h"

#include <QLabel>
#include <QMenu>
#include <QAction>

#include <list>

class QComboBox;
class QSplitter;
class QTabWidget;

class toConnection;
class toMain;
class toResultBar;
class toResultSchema;
class toResultItem;
class toResultLock;
class toResultStats;
class toResultTableView;
class toSGAStatement;
class toSessionFilter;
class toTreeWidgetItem;
class toWaitEvents;
class toRefreshCombo;

#define TO_SESSION_WAIT "toSession:SessionWait"
#define TO_SESSION_IO   "toSession:SessionIO"
#define TO_SESSION_TXN  "toSession:SessionTXN"

namespace ToConfiguration
{
    class Session : public ConfigContext
    {
            Q_OBJECT;
            Q_ENUMS(OptionTypeEnum);
        public:
            Session() : ConfigContext("Session", ENUM_REF(Session,OptionTypeEnum)) {};
            enum OptionTypeEnum
            {
                KillProcUseKillProcBool = 17000,
                KillProcName,
                KillProcSID,
                KillProcSerial,
                KillProcInstance,
                KillProcInstanceBool,
                KillProcImmediate,
                KillProcImmediateBool,
                KillSessionModeInt
            };

            enum KillSessionModeEnum
            {
                Disconnect = 1,
                Kill,
                KillImmediate
            };
            QVariant defaultValue(int option) const;
    };
};

class toSession : public toToolWidget
{
        Q_OBJECT;

        toResultTableView *Sessions;
        QTabWidget        *ResultTab;

        QWidget *CurrentTab;

        toResultSchema *Select;

        toSGAStatement    *CurrentStatement;
        toSGAStatement    *PreviousStatement;
        toResultStats     *SessionStatistics;
        toResultLong      *LongOps;
        toResultItem      *ConnectInfo;
        toResultTableView *LockedObjects;
        toResultLock      *PendingLocks;
        toResultTableView *AccessedObjects;
#ifdef TORA_EXPERIMENTAL
        toResultBar       *WaitBar;
        toResultBar       *IOBar;
#endif
        toWaitEvents      *Waits;
        QSplitter         *OpenSplitter;
        QSplitter         *StatisticSplitter;
        toSGAStatement    *OpenStatement;
        toResultTableView *OpenCursors;
        toResultItem      *Transaction;
        QString            LastSession;
        QMenu             *ToolMenu;
        toRefreshCombo    *Refresh;
        QLabel            *Total;
        toSessionFilter   *SessionFilter;

        QAction *refreshAct;
        QAction *enableTimedAct;
        QAction *disableTimedAct;
        QAction *disconnectAct;

        QString Session;
        QString Serial;

        void updateSchemas(void);
        void enableStatistics(bool enable);

        friend class toSessionSetting;
    public:
        toSession(QWidget *parent, toConnection &connection);
        ~toSession();

        bool canHandle(const toConnection &conn) override;

        static QString sessionKillProcOracle(ToConfiguration::Session::KillSessionModeEnum, const QMap<QString,QString> params);
    public slots:
        void slotChangeTab(int);
        void slotChangeItem();
        void slotChangeCursor();
        void slotRefresh(void);
        void slotRefreshTabs(void);
        void slotEnableStatistics(void)
        {
            enableStatistics(true);
        }
        void slotDisableStatistics(void)
        {
            enableStatistics(false);
        }
        void slotCancelBackend(void);
        void slotDisconnectSession(void);
        void slotWindowActivated(toToolWidget*) override;
        void slotDone(void);
        void slotSelectAll(void);
        void slotSelectNone(void);
        void slotFilterChanged(const QString &text);
    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

        static const QString DISCONNECT;
        static const QString ALTER;
        static const QString KPROC;
};

#ifdef TOEXTENDED_MYSQL
#include "tosessionmysql.h"
#endif

class toSessionDisconnect : public QDialog, public Ui::toDisconnectDlg
{
    Q_OBJECT;
    friend class toSession;
public:
    toSessionDisconnect(toResultTableView *sessionView, QWidget *parent = 0, const char *name = 0);
protected:
    enum ReturnType
    {
        Rejected = Rejected,
        Accepted =  QDialog::Accepted,
        Copy = 10
    };
private slots:
    void slotKillDisconnect();
    void slotKill();
    void slotKillImmediate();
    void slotCopy();
    void slotExecuteAll();
private:
    toResultTableView *SessionView;
};

class toSessionSetting
    : public QWidget
    , public Ui::toSessionSetupUI
    , public toSettingTab
{
        Q_OBJECT;
        toTool *Tool;

    public:
        toSessionSetting(toTool *tool, QWidget* parent = 0, const char* name = 0);

        void saveSetting(void) override;
    private slots:
        void killProcToggled(bool);
        void composeKillProc();

    private:
};

#endif
