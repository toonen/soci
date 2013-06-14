//
// Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#if !defined(SOCI_DATE_TYPES_H_INCLUDED)
#define SOCI_DATE_TYPES_H_INCLUDED

#include "type-conversion-traits.h"
#include "timestamp.h"
#include <ctime>

namespace soci
{
    template <>
    struct type_conversion<std::tm>
    {
        typedef soci::timestamp base_type;

        static void from_base(base_type const & in, indicator ind, std::tm & out)
        {
            if (ind != i_null)
            {
                out = static_cast<std::tm>(in);
            }
        }

        static void to_base(std::tm const & in, base_type & out, indicator & ind)
        {
            out = static_cast<soci::timestamp>(in);
            out.ts_nsec = 0;
            ind = i_ok;
        }
    };
}
// end namespace soci

#endif // !defined(SOCI_DATE_TYPES_H_INCLUDED)
