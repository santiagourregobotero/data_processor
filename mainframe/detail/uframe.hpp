//          Copyright Santiago Urrego Botero 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_mainframe_detail_uframe_h
#define INCLUDED_mainframe_detail_uframe_h

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <list>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "mainframe/detail/base.hpp"
#include "mainframe/expression.hpp"
#include "mainframe/frame_iterator.hpp"
#include "mainframe/missing.hpp"
#include "mainframe/series.hpp"
#include "mainframe/detail/frame.hpp"

namespace mf
{

template<typename... Ts>
class frame;

class uframe
{
public:
    uframe()                         = default;
    uframe(const uframe&)            = default;
    uframe(uframe&&)                 = default;
    uframe& operator=(const uframe&) = default;
    uframe& operator=(uframe&&)      = default;

    template<typename... Ts>
    uframe(const frame<Ts...>& f)
    {
        init_impl<0>(f);
    }

    void
    append_column(const useries& s)
    {
        m_columns.push_back(s);
        expand_consistent();
    }

    template<typename T>
    void
    append_column(const std::string& colname)
    {
        series<T> s;
        s.set_name(colname);
        s.resize(size());
        m_columns.push_back(s);
    }

    void
    clear()
    {
        for (auto& s : m_columns) {
            s.clear();
        }
    }

    useries&
    column(size_t i)
    {
        return m_columns.at(i);
    }

    const useries&
    column(size_t i) const
    {
        return m_columns.at(i);
    }

    size_t
    num_columns() const
    {
        return m_columns.size();
    }

    template<typename... Ts>
    operator frame<Ts...>() const
    {
        frame<Ts...> out;
        out.populate(m_columns);
        return out;
    }

    void
    prepend_column(const useries& s)
    {
        m_columns.insert(m_columns.begin(), s);
        expand_consistent();
    }

    template<typename T>
    void
    prepend_column(const std::string& colname)
    {
        series<T> s;
        s.set_name(colname);
        s.resize(size());
        m_columns.insert(m_columns.begin(), s);
    }

    void
    remove_column(size_t ind)
    {
        m_columns.erase(m_columns.begin()+ind);
    }

    void
    set_column(size_t idx, const useries& s)
    {
        m_columns.at(idx) = s;
        expand_consistent();
    }

    size_t
    size() const
    {
        if (m_columns.size() == 0) {
            return 0;
        }
        size_t sz = m_columns[0].size();
        for (auto& s : m_columns) {
            if (sz != s.size()) {
                throw std::logic_error{ "Inconsistent sizes!" };
            }
        }
        return sz;
    }

private:
    template<size_t Ind, typename... Ts>
    void
    init_impl(const frame<Ts...>& f)
    {
        columnindex<Ind> ci;
        useries s = f.column(ci);
        m_columns.push_back(s);
        if constexpr (Ind + 1 < sizeof...(Ts)) {
            init_impl<Ind + 1>(f);
        }
    }

    void
    expand_consistent()
    {
        if (m_columns.size() == 0) {
            return;
        }
        size_t sz = 0U;
        for (auto& s : m_columns) {
            sz = std::max(sz, s.size());
        }
        for (auto& s : m_columns) {
            s.resize(sz);
        }
    }

    std::vector<useries> m_columns;
};


} // namespace mf

#endif // INCLUDED_mainframe_detail_uframe_h
