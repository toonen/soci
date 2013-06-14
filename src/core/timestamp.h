//
// Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#if !defined(SOCI_TIMESTAMP_H_INCLUDED)
#define SOCI_TIMESTAMP_H_INCLUDED

#include <stdint.h>
#include <ctime>

namespace soci
{
    class timestamp : public std::tm
    {
      public:
        timestamp(void)
        {
            tm_year = 0;
            tm_mon = 0;
            tm_mday = 1;
            tm_hour = 0;
            tm_min = 0;
            tm_sec = 0;
            ts_nsec = 0;
            tm_wday = 0;
            tm_yday = 0;
            tm_isdst = -1;
        }

        timestamp(std::tm const & tm)
        {
            static_cast<std::tm &>(*this) = tm;
            ts_nsec = 0;
        }

        timestamp & operator=(std::tm const & tm)
        {
            static_cast<std::tm &>(*this) = tm;
            ts_nsec = 0;
            return *this;
        }

        int32_t ts_nsec;
    };
}
// end namespace soci

#endif // !defined(SOCI_TIMESTAMP_H_INCLUDED)
