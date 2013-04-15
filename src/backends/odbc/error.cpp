//
// Copyright (C) 2004-2006 Maciej Sobczak, Stephen Hutton, David Courtney
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define SOCI_ODBC_SOURCE
#include "soci-odbc.h"
#include <sstream>

using namespace soci;
using namespace soci::details;

odbc_soci_error::odbc_soci_error(const SQLSMALLINT p_htype,
    const SQLHANDLE p_hndl, std::string const & p_msg) :
        soci_error(p_msg)
{
    SQLCHAR sql_state[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sql_code;
    std::vector<SQLCHAR> msg_text(SQL_MAX_MESSAGE_LENGTH + 1);
    SQLSMALLINT msg_len;

    int rec_num = 1;
    while (true)
    {
        SQLRETURN rc = SQLGetDiagRec(p_htype, p_hndl, rec_num, sql_state,
            &sql_code, &msg_text[0], msg_text.size(), &msg_len);
        if (rc == SQL_SUCCESS)
        {
            // ODBC diag record ordering is defined such that the first record
            // will likely contain the most significant message.  The records
            // are prepended to the various lists so that, when printed/logged,
            // the last message will be the most important.
            std::string msg((const char *) &msg_text[0]);
            msg.erase(msg.find_last_not_of(" \n") + 1);
            messages_.push_front(msg);
            sqlstates_.push_front((const char *) sql_state);
            sqlcodes_.push_front(sql_code);
        }
        else if (rc == SQL_NO_DATA)
        {
            if (rec_num == 1)
            {
                messages_.push_front("[SOCI] No error information");
                sqlstates_.push_front("01000");
                sqlcodes_.push_front(0);
            }
            break;
        }
        else if (rc == SQL_SUCCESS_WITH_INFO)
        {
            msg_text.resize(msg_len + 1);
            continue;
        }
        else
        {
            if (rc == SQL_INVALID_HANDLE)
            {
                messages_.push_front("[SOCI] Invalid handle");
            }
            else if (rc == SQL_ERROR)
            {
                messages_.push_front("[SOCI] SQLGetDiagRec error");
            }
            else
            {
                std::ostringstream msg;
                msg << "[SOCI] Unexpected SQLGetDiagRec return value (" <<
                    rc << ")";
                messages_.push_front(msg.str());
            }
            sqlstates_.push_front("HY000");
            sqlcodes_.push_front(rc);
            break;
        }

        rec_num += 1;
    }
}

bool odbc_soci_error::has_state(const std::string & p_state) const
{
    std::list<std::string>::const_iterator iter;

    for (iter = sqlstates_.begin(); iter != sqlstates_.end(); iter++)
    {
        if (*iter == p_state)
        {
            return true;
        }
    }

    return false;
}

bool odbc_soci_error::has_state_class(
    const std::string & p_state_class) const
{
    std::list<std::string>::const_iterator iter;

    for (iter = sqlstates_.begin(); iter != sqlstates_.end(); iter++)
    {
        if (iter->substr(0,2) == p_state_class)
        {
            return true;
        }
    }

    return false;
}
