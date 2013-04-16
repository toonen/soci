//
// Copyright (C) 2011-2013 Denis Chapligin
// Copyright (C) 2004-2006 Maciej Sobczak, Stephen Hutton
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define SOCI_DB2_SOURCE
#include "soci-db2.h"

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

using namespace soci;
using namespace soci::details;

db2_soci_error::db2_soci_error(std::string const & p_msg, SQLRETURN p_rc, SQLSMALLINT p_htype, SQLHANDLE p_hndl) :
        soci_error(p_msg),
        errorCode(p_rc)
{
    SQLCHAR sql_state[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sql_code;
    std::vector<SQLCHAR> msg_text(SQL_MAX_MESSAGE_LENGTH + 1);
    SQLSMALLINT msg_len;

    // NOTE: DB2 diag record ordering is defined such that the first record will likely contain the most significant message.
    // The records are prepended to the various lists so that, when printed/logged, the last message will be the most important.

    if (p_rc == SQL_INVALID_HANDLE)
    {
        messages_.push_front("[SOCI] DB2 CLI function was passed an invalid handle");
        return;
    }

    int rec_num = 1;
    while (true)
    {
        SQLRETURN rc = SQLGetDiagRec(p_htype, p_hndl, rec_num, sql_state, &sql_code, &msg_text[0], msg_text.size(), &msg_len);
        if (rc == SQL_SUCCESS)
        {
            std::string msg((const char *) &msg_text[0]);
            msg.erase(msg.find_last_not_of(" \n") + 1);
            messages_.push_front(msg);
            sql_states_.push_front((const char *) sql_state);
            sql_codes_.push_front(sql_code);
        }
        else if (rc == SQL_NO_DATA)
        {
            if (rec_num == 1)
            {
                if (p_rc == SQL_SUCCESS)
                {
                    messages_.push_front("[SOCI] No error");
                    sql_states_.push_front("00000");
                    sql_codes_.push_front(SQL_SUCCESS);
                }
                else
                {
                    std::ostringstream msg;
                    msg << "[SOCI] " << "DB2 reported an " << ((p_rc >= 0) ? "issue" : "error") << " (" << p_rc << ")" <<
                        " but further diagnostic information was not available";
                    messages_.push_front(msg.str());
                    sql_states_.push_front("01000");
                    sql_codes_.push_front(p_rc);
                }
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
                messages_.push_front("[SOCI] SQLGetDiagRec was passed an invalid handle");
            }
            else if (rc == SQL_ERROR)
            {
                messages_.push_front("[SOCI] SQLGetDiagRec returned SQL_ERROR");
            }
            else
            {
                std::ostringstream msg;
                msg << "[SOCI] SQLGetDiagRec returned an unexpected value (" << rc << ")";
                messages_.push_front(msg.str());
            }
            sql_states_.push_front("HY000");
            sql_codes_.push_front(rc);
            break;
        }

        rec_num += 1;
    }
}

bool db2_soci_error::has_state(const std::string & p_state)
{
    std::list<std::string>::iterator iter;

    for (iter = sql_states_.begin(); iter != sql_states_.end(); iter++)
    {
        if (*iter == p_state)
        {
            return true;
        }
    }

    return false;
}

bool db2_soci_error::has_state_class(const std::string & p_state_class)
{
    std::list<std::string>::iterator iter;

    for (iter = sql_states_.begin(); iter != sql_states_.end(); iter++)
    {
        if (iter->substr(0,2) == p_state_class)
        {
            return true;
        }
    }

    return false;
}
