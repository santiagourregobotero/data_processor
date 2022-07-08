//          Copyright Ted Middleton 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_mainframe_grouped_frame_h
#define INCLUDED_mainframe_grouped_frame_h

#include "mainframe/frame.hpp"
#include <map>

namespace mf
{

template<size_t... Inds>
struct index_defn
{};

template<typename IndexDefn, typename... Ts>
class grouped_frame;

namespace detail {

// Does the index def contain that column index?
// index_defn_contains<3, index_defn<0, 2, 1>>::value == false
// index_defn_contains<1, index_defn<0, 2, 1>>::value == true
template<size_t ColNum, typename IndexDefn>
struct index_defn_contains : std::false_type
{};

template<size_t ColNum, size_t... Inds>
struct index_defn_contains<ColNum, index_defn<ColNum, Inds...>> : std::true_type
{};

template<size_t ColNum, size_t Ind, size_t... Inds>
struct index_defn_contains<ColNum, index_defn<Ind, Inds...>> : index_defn_contains<ColNum, index_defn<Inds...>>
{};

// Get the ColNum'th index
// index_defn_deref<3, index_defn<32, 12, 11, 34>>::value == 34
// index_defn_deref<1, index_defn<32, 12, 11, 34>>::value == 12
template<size_t ColNum, typename IndexDefn>
struct index_defn_deref;

template<size_t ColNum, size_t Ind, size_t... Inds>
struct index_defn_deref<ColNum, index_defn<Ind, Inds...>>
{
    static const size_t value = index_defn_deref<ColNum-1, index_defn<Inds...>>::value;
};

template<size_t Ind, size_t... Inds>
struct index_defn_deref<0, index_defn<Ind, Inds...>>
{
    static const size_t value = Ind;
};

// Make a Tpl with the index defn's columns/items
// index_defn_select<index_defn<0, 4>, tuple<char, short, int, long>>::type == tuple<char, long>
// index_defn_select<index_defn<2, 0>, frame<year_month, double, float>>::type == frame<float, year_month>
template<typename IndexDefn, typename Tpl>
struct index_defn_select;

template<size_t Ind, size_t... Inds, template<typename...> typename Tpl, typename... Ts>
struct index_defn_select<index_defn<Ind, Inds...>, Tpl<Ts...>>
{
    using ind_type = typename pack_element<Ind, Ts...>::type;
    using remaining = typename index_defn_select<index_defn<Inds...>, Tpl<Ts...>>::type;
    using type = typename prepend<ind_type, remaining>::type;
};

template<size_t Ind, template<typename...> typename Tpl, typename... Ts>
struct index_defn_select<index_defn<Ind>, Tpl<Ts...>>
{
    using ind_type = typename pack_element<Ind, Ts...>::type;
    using type = Tpl<ind_type>;
};

// Prepend an index to an index defn
// index_defn_prepend<6, index_defn<0, 1, 2>>::type == index_defn<6, 0, 1, 2>
template<size_t Ind, typename IndexDefn>
struct index_defn_prepend;

template<size_t Ind, size_t... Inds>
struct index_defn_prepend<Ind, index_defn<Inds...>>
{
    using type = index_defn<Ind, Inds...>;
};

template<size_t CurrInd, size_t Num, typename IndexDefn>
struct index_defn_invert_impl
{
    using remaining = typename index_defn_invert_impl<CurrInd+1, Num, IndexDefn>::type;
    using type = typename std::conditional<
        index_defn_contains<CurrInd, IndexDefn>::value,
        remaining,
        typename index_defn_prepend<CurrInd, remaining>::type>::type;
};

template<size_t CurrInd, typename IndexDefn>
struct index_defn_invert_impl<CurrInd, CurrInd, IndexDefn>
{
    using type = index_defn<>;
};

// Invert the index definition for a Tpl
// index_defn_invert<index_defn<0, 3>, frame<char, short, int, long>>::type == index_defn<1, 2>
// index_defn_invert<index_defn<3, 0>, frame<char, short, int, long>>::type == index_defn<1, 2>
template<typename IndexDefn, typename Tpl>
struct index_defn_invert;

template<typename IndexDefn, template <typename...> typename Tpl, typename... Ts>
struct index_defn_invert<IndexDefn, Tpl<Ts...>>
{
    using type = typename index_defn_invert_impl<0, sizeof...(Ts), IndexDefn>::type;
};


// A frame with only the indexed columns in it.
// eg indexed_frame<index_defn<4, 0>, frame<short, double, double, int, year_month>>::type
// == frame<year_month, short>
// This is used to build the frame index
template<typename IndexDefn, typename Frame>
struct indexed_frame;

template<size_t Ind, size_t... Inds, template <typename...> typename Frame, typename... Ts>
struct indexed_frame<index_defn<Ind, Inds...>, Frame<Ts...>>
{
    using ind_type = typename pack_element<Ind, Ts...>::type;
    using remaining = typename indexed_frame<index_defn<Inds...>, Frame<Ts...>>::type;
    using type = typename prepend<ind_type, remaining>::type;

    static type get( Frame<Ts...>& f )
    {
        return f.columns(columnindex<Ind>{}, columnindex<Inds>{}...);
    }
};

template<size_t Ind, template <typename...> typename Frame, typename... Ts>
struct indexed_frame<index_defn<Ind>, Frame<Ts...>>
{
    using ind_type = typename pack_element<Ind, Ts...>::type;
    using type = frame<ind_type>;
};

// A frame with all of the indexed columns removed
// eg unindexed_frame<index_defn<4, 0>, frame<short, double, double, int, year_month>>::type
// is frame<double, double, int>
template<typename IndexDefn, typename Frame>
using unindexed_frame = indexed_frame<typename index_defn_invert<IndexDefn, Frame>::type, Frame>;

// A frame derived from another frame and two index definitions.
//   result_frame<
//     index_defn<4, 0>, 
//     index_defn<2, 3>, 
//     frame<short, double, float, int, year_month>>::type
//   == frame<year_month, short, float, int>
// This is used to execute grouped functions like max() - the functions specify columns, 
// which is what the second index_defn is for. The first index_defn is for the 
// indexed/group'd-by columns. So for example, if we had
//   frame<year, month, double, int, float, bool> f;
//   auto gf = f.groupby( _1, _5 ); // month and bool
//   auto mx = gf.max( _2, _4 );
//  ...inside the max() function we will need
//   auto rf = result_frame<index_defn<1, 5>, index_defn<2, 4>, frame<Ts...>>::get( m_frame );
// and rf will be of type
//  frame<month, bool, double, float>
template<typename IndexDefn1, typename IndexDefn2, typename Frame>
struct result_frame;

template<size_t... Inds1, size_t... Inds2, typename Frame>
struct result_frame<index_defn<Inds1...>, index_defn<Inds2...>, Frame>
{
    using indexed_cols1 = typename indexed_frame<index_defn<Inds1...>, Frame>::type;
    using indexed_cols2 = typename indexed_frame<index_defn<Inds2...>, Frame>::type;
    using type = typename join_frames<indexed_cols1, indexed_cols2>::type;

    static type get( Frame& f )
    {
        return f.columns(columnindex<Inds1>{}..., columnindex<Inds2>{}...);
    }
};


} // namespace detail
 

template<size_t... Inds, typename... Ts>
class grouped_frame<index_defn<Inds...>, Ts...>
{
    // Pulling types out of namespaces for convenience
    template<typename Tpl, size_t... Is>
    using rearrange = detail::rearrange<Tpl, Is...>;
    template<typename... Us>
    using index_key = frame_row<Us...>;
    template<size_t Ind, typename... Us>
    using pack_element = detail::pack_element<Ind, Us...>;
    template<typename IndexDefn, typename Frame>
    using indexed_frame = detail::indexed_frame<IndexDefn, Frame>;
    template<typename T, typename U>
    using join_frames = detail::join_frames<T, U>;

    using index_definition = index_defn<Inds...>;
    using index_frame = typename indexed_frame<index_definition, frame<Ts...>>::type;
    using key_type = typename rearrange<index_key<Ts...>, Inds...>::type;
    using map_type = std::multimap<key_type, size_t>;
public:
    grouped_frame(frame<Ts...> f)
        : m_frame(f)
    {}
//
//    template<size_t Ind>
//    result_type<typename pack_element<Ind, Ts...>::type>
//    max(columnindex<Ind>) const
//    {
//        using std::numeric_limits;
//        using R = typename pack_element<Ind, Ts...>::type;
//        build_index();
//        result_type<R> out;
//        key_type last_key;
//        R num = numeric_limits<R>::min();
//        auto it = m_idx.cbegin();
//        int i = 0;
//        for ( ; it != m_idx.cend(); ++it, ++i ) {
//            const key_type& key = it->first();
//            const size_t rowind = it->second();
//            if ( i != 0 && key != last_key ) {
//                num = numeric_limits<R>::min();
//                auto row = m_frame[rowind];
//
//                
//
//
//                // What do we do here? We have a frame like
//                //
//                //  frame<month, double, int, double> f1
//                //  f1.set_column_names( "month", "price", "quantity", "change" )
//                //
//                // and then 
//                //
//                //  auto gf = f1.groupby( _1, _2 )
//                //  (or auto gf = f1.groupby( "price", "quantity" ) )
//                //
//                // returns a gf that is
//                //
//                //  grouped_frame<index_defn<1, 2>, month, double, int, double> gf
//                // 
//                // and then 
//                //
//                //  auto f2 = gf.max( _3 ) 
//                //  (or gf.max( "change" ) )
//                //
//                // which returns (from this function)
//                // 
//                //  frame<double, int, double> f2
//                //  (col names are "price", "quantity", "max(change)")
//                //
//                // how to do this?
//                // how about using uframe? 
//                //
//                // option 1: uframe
//                // option 2: a frame_view?
//                //  - this might have real value past this problem
//                //  - a real frame_view should abstract rows as well as columns 
//                //    through
//                // option 3: more flexible/sophisticated _row_proxy
//                //  - this seems hacky 
//                // option 4: some kind of join?
//                //  - how would this work? what would be joined?
//                // option 5: better indexers
//                //  - how would this work?
//                //  - frame<int, int, int> f1;
//                //    f1.set_column_names( "one", "two", "three" );
//                //    f1.push_back(  1,  2,  3 );
//                //    f1.push_back( 11, 12, 13 );
//                //    f1.push_back( 21, 22, 23 );
//                //    f1.push_back( 31, 32, 33 );
//                //    f1.push_back( 41, 42, 43 );
//                //    f1.push_back( 51, 52, 53 );
//                //
//                //    cout << f1;
//                //
//                //      | one | two | three
//                //    __|_____|_____|_______
//                //     0|   1 |    2|     3 
//                //     1|  11 |   12|    13 
//                //     2|  21 |   22|    23 
//                //     3|  31 |   32|    33 
//                //     4|  41 |   42|    43 
//                //     5|  51 |   52|    53 
//                //
//                //    f1[ 1->3 ][ _1, _2 ] 
//                //    f1[ _1, _2 ][ 1->3 ]
//                //
//                //      | two | three
//                //    __|_____|_______
//                //     0|   12|    13 
//                //     1|   22|    23 
//                //     2|   32|    33 
//                //
//                //    f1[ 0->-1 ][ _1, _0, _2 ] 
//                //    f1[ _1, _0, _2 ][ 0->-1 ]
//                //
//                //      | two | one | three
//                //    __|_____|_____|_______
//                //     0|    2|   1 |     3 
//                //     1|   12|  11 |    13 
//                //     2|   22|  21 |    23 
//                //     3|   32|  31 |    33 
//                //     4|   42|  41 |    43 
//                //     5|   52|  51 |    53 
//                //
//                //    f1[ 0, 2, 4 ]
//                //
//                //      | one | two | three
//                //    __|_____|_____|_______
//                //     0|   1 |    2|     3 
//                //     1|  21 |   22|    23 
//                //     2|  41 |   42|    43 
//                //
//
//            }
//            else {
//
//            }
//        }
//    }
//
//    template<size_t Ind>
//    result_type<typename pack_element<Ind, Ts...>::type>
//    min(columnindex<Ind>) const
//    {
//        using R = typename pack_element<Ind, Ts...>::type;
//        build_index();
//        result_type<R> out;
//        return out;
//    }
//

    typename join_frames<index_frame, frame<int>>::type
    count() const
    {
        using result_frame = typename join_frames<index_frame, frame<int>>::type;
        using row_type = typename result_frame::row_type;
        build_index();
        result_frame out;

        key_type pk;
        auto it = m_idx.cbegin();
        auto end = m_idx.cend();
        for ( ; it != end;  ) {
            auto next = m_idx.upper_bound( it->first );

            int total = 0;
            auto eit = it;
            for ( ; eit != next; ++eit, ++total );

            row_type row;
            //uuuh...what now?


            it = next;
        }

        return out;
    }

//    template<size_t Ind>
//    result_type<typename pack_element<Ind, Ts...>::type>
//    mean(columnindex<Ind>) const
//    {
//        using R = typename pack_element<Ind, Ts...>::type;
//        build_index();
//        result_type<R> out;
//        return out;
//    }
//
private:

    void
    build_index()
    {
        if (m_idx.size() == m_frame.size()) {
            return;
        }

        index_frame ifr{ index_frame::get( m_frame ) };

        for ( auto i = 0U; i < m_frame.size(); ++i ) {
            auto row = ifr[i];
            key_type key{ row };
            m_idx.insert( std::make_pair( key, i ) );
        }
    }

    map_type m_idx;
    frame<Ts...> m_frame;
};

//template<size_t... Idx, typename... Ts>
//grouped_frame<index_defn<Idx...>, Ts...>
//groupby(frame<Ts...> f, columnindex<Idx>...)
//{
//    return grouped_frame<index_defn<Idx...>, Ts...>{ f };
//}


} // namespace mf

#endif // INCLUDED_mainframe_grouped_frame_h
