//          Copyright Santiago Urrego Botero 2022.
// Distributed under the Boost Software License, Version 1.0.


#ifndef INCLUDED_mainframe_columnindex_h
#define INCLUDED_mainframe_columnindex_h

#include <cstdlib>

namespace mf
{
template<size_t Ind>
struct expr_column;

template<typename T>
struct terminal;

template<size_t Ind>
using columnindex = terminal<expr_column<Ind>>;
}

#endif // INCLUDED_mainframe_columnindex_h

