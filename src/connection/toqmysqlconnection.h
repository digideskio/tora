/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2009 Numerous Other Contributors
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 *      Specifically you are not permitted to link this program with the
 *      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
 *      And you are not permitted to distribute binaries compiled against
 *      these libraries.
 *
 *      You may link this product with any GPL'd Qt library.
 *
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef __QMYSQL_CONNECTION__
#define __QMYSQL_CONNECTION__

#include "core/toconnection.h"
#include "core/toconnectionsub.h"
#include "connection/toqsqlconnection.h"

#include <QtCore/QString>
#include <QtSql/QSqlDatabase>

// MySQL datatypes (From mysql_com.h)
enum enum_field_types { FIELD_TYPE_DECIMAL, FIELD_TYPE_TINY,
                        FIELD_TYPE_SHORT, FIELD_TYPE_LONG,
                        FIELD_TYPE_FLOAT, FIELD_TYPE_DOUBLE,
                        FIELD_TYPE_NULL, FIELD_TYPE_TIMESTAMP,
                        FIELD_TYPE_LONGLONG, FIELD_TYPE_INT24,
                        FIELD_TYPE_DATE, FIELD_TYPE_TIME,
                        FIELD_TYPE_DATETIME, FIELD_TYPE_YEAR,
                        FIELD_TYPE_NEWDATE,
                        FIELD_TYPE_ENUM = 247,
                        FIELD_TYPE_SET = 248,
                        FIELD_TYPE_TINY_BLOB = 249,
                        FIELD_TYPE_MEDIUM_BLOB = 250,
                        FIELD_TYPE_LONG_BLOB = 251,
                        FIELD_TYPE_BLOB = 252,
                        FIELD_TYPE_VAR_STRING = 253,
                        FIELD_TYPE_STRING = 254
                      };

class toQMySqlConnectionImpl: public toQSqlConnectionImpl
{
	friend class toQMySqlProvider;
protected:
    toQMySqlConnectionImpl(toConnection &conn);
public:
    /** Create a new connection to the database. */
    virtual toConnectionSub *createConnection(void);

    /** Close a connection to the database. */
    virtual void closeConnection(toConnectionSub *);
private:
};

class toQMySqlConnectionSub : public toConnectionSub
{
	friend class toQMySqlConnectionImpl;
public:
        toQMySqlConnectionSub(toConnection const& parent, QSqlDatabase const& db, QString const& dbname)
        	: ParentConnection(parent)
            , Connection(db)
        	, Name(dbname)
            , HasTransactions(false)
        {}

        ~toQMySqlConnectionSub()
        {
            LockingPtr<QSqlDatabase> ptr(Connection, Lock);
            ptr->close();
        }

        void throwError(const QString &sql);

        /** Implemented abstract method inherited from toConnectionSub */

        virtual void cancel(void)
        {
        	// NOT SUPPORTED ON MYSQL
        };

        /** Close connection. */
        virtual void close(void)
        {
        	throw QString("Not implemented yet: toQMySqlConnectionSub::close");
        }

        virtual void commit(void);

        virtual void rollback(void);

        virtual QString version();

        virtual QString sessionId();

        virtual queryImpl* createQuery(toQuery *query);

        virtual toQAdditionalDescriptions* decribe(toCache::ObjectRef const&)
        {
        	throw QString("Not implemented yet: toQMySqlConnectionSub::describe");
        }

        static QString ErrorString(const QSqlError &err, const QString &sql = QString::null);

        QSqlDatabase Connection;
        QMutex Lock;
        QString ConnectionID;
private:
        QString Name;
        bool HasTransactions;
        toConnection const& ParentConnection;
};

#endif