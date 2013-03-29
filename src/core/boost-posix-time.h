//
// Copyright (C) 2008 Maciej Sobczak
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#if !defined(SOCI_BOOST_POSIX_TIME_H_INCLUDED)
#define SOCI_BOOST_POSIX_TIME_H_INCLUDED

#include "type-conversion-traits.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace soci
{
    template<>
    struct type_conversion<boost::posix_time::ptime>
    {
        typedef soci::timestamp base_type;

        static void from_base(base_type const & in, indicator ind, boost::posix_time::ptime & out)
        {
            if (ind != i_null)
            {
#               if defined(BOOST_DATE_TIME_HAS_NANOSECONDS)
                {
                    out = boost::posix_time::ptime_from_tm(static_cast<std::tm>(in)) +
                        boost::posix_time::nanoseconds(in.ts_nsec);
                }
#               else
                {
                    out = boost::posix_time::ptime_from_tm(static_cast<std::tm>(in)) +
                        boost::posix_time::microseconds(in.ts_nsec / 1000);
                }
#               endif
            }
            else
            {
                out = boost::posix_time::not_a_date_time;
            }
        }

        static void to_base(boost::posix_time::ptime const & in, base_type & out, indicator & ind)
        {
            if (in.is_special() == false)
            {
                out = boost::posix_time::to_tm(in);

                const boost::posix_time::time_duration td = in.time_of_day();
                if (td.num_fractional_digits() <= 9)
                {
                    out.ts_nsec = td.fractional_seconds() * (1000000000 / td.ticks_per_second());
                }
                else
                {
                    out.ts_nsec = td.fractional_seconds() / (td.ticks_per_second() / 1000000000);
                }

                ind = i_ok;
            }
            else if (in.is_not_a_date_time())
            {
                ind = i_null;
            }
            else
            {
                throw soci_error("boost::posix_time min, max and infinite times are not supported");
            }
        }
    };
}
// namespace soci

#endif // !defined(SOCI_BOOST_POSIX_TIME_H_INCLUDED)
