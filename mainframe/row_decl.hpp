//          Copyright Santiago Urrego Botero 2022.



#ifndef INCLUDED_mainframe_row_decl_h
#define INCLUDED_mainframe_row_decl_h

namespace mf
{

template<bool IsConst, typename... Ts>
class _row_proxy;

template<bool IsConstDummy, typename... Ts>
class _base_frame_row;

template<typename... Ts>
using frame_row = _base_frame_row<false, Ts...>;

template<bool IsConst, bool IsReverse, typename... Ts>
class base_frame_iterator;

template<typename... Ts>
using frame_iterator = base_frame_iterator<false, false, Ts...>;

template<typename... Ts>
using const_frame_iterator = base_frame_iterator<true, false, Ts...>;

template<typename... Ts>
using reverse_frame_iterator = base_frame_iterator<false, true, Ts...>;

template<typename... Ts>
using const_reverse_frame_iterator = base_frame_iterator<true, true, Ts...>;

} // namespace mf


#endif // INCLUDED_mainframe_row_decl_h
 
