
//          Copyright Ted Middleton 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_miniframe_series_h
#define INCLUDED_miniframe_series_h

#include <type_traits>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <string>
#include <list>
#include <optional>
#include <variant>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <iostream>
#include <sstream>
#include <variant>

#include "miniframe/base.hpp"
#include "miniframe/series_vector.hpp"

namespace mf
{

// This is an untyped version of series
class column
{
public:
};

template< typename T >
class series
{
public:
    // types
    using value_type        = T;
    using size_type         = typename series_vector<T>::size_type;
    using difference_type   = typename series_vector<T>::difference_type;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer	    = const value_type*;
    using iterator          = typename series_vector<T>::iterator;
    using const_iterator    = typename series_vector<T>::const_iterator;
    using reverse_iterator  = typename series_vector<T>::reverse_iterator;
    using const_reverse_iterator = typename series_vector<T>::const_reverse_iterator;

    // ctors
    series() 
        : m_sharedvec( std::make_shared< series_vector<T> >() )
    {}

    series( size_type count, const T& value )
        : m_sharedvec( std::make_shared< series_vector<T> >( count, value ) )
    {}

    explicit series( size_type count )
        : m_sharedvec( std::make_shared< series_vector<T> >( count ) )
    {}

    template< typename InputIt >
    series( InputIt f, InputIt l )
        : m_sharedvec( std::make_shared< series_vector<T> >( f, l ) )
    {}

    series( const series& other ) = default;

    series( series&& other )
        : m_sharedvec( std::move( other.m_sharedvec ) )
    {
        // Don't leave other with nullptr
        other.m_sharedvec = std::make_shared< series_vector<T> >();
    }

    series( std::initializer_list<T> init )
        : m_sharedvec( std::make_shared< series_vector<T> >( init ) )
    {}
     
    virtual ~series() = default;

    // operator=
    series& operator=( series&& other ) 
    {
        m_sharedvec = std::move( other.m_sharedvec );
        other.m_sharedvec = std::make_shared< series_vector<T> >();
        return *this;
    }

    series& operator=( const series& ) = default;

    series& operator=( std::initializer_list<T> init )
    {
        m_sharedvec = std::make_shared< series_vector<T> >( init );
        return *this;
    }

    // assign
    void assign( size_type count, const T& value );
    template< typename InputIt >
    void assign( InputIt inbegin, InputIt inend );
    void assign( std::initializer_list<T> init );
    
    // at
    reference at( size_type n )
    {
        return m_sharedvec->at( n );
    }

    // operator=
    T& operator[]( size_type n )
    {
        return m_sharedvec[ n ];
    }

    // front & back
    reference front()
    {
        return m_sharedvec->front();
    }

    // data
    T* data()
    {
        return m_sharedvec->data();
    }

    // begin, cbegin, end, cend
    iterator begin()
    {
        return m_sharedvec->begin();
    }
    const_iterator begin() const
    {
        return m_sharedvec->begin();
    }
    const_iterator cbegin() const
    {
        return m_sharedvec->cbegin();
    }
    iterator end()
    {
        return m_sharedvec->end();
    }
    const_iterator end() const
    {
        return m_sharedvec->end();
    }
    const_iterator cend() const
    {
        return m_sharedvec->cend();
    }

    // rbegin, crbegin, rend, crend
    iterator rbegin()
    {
        return m_sharedvec->rbegin();
    }
    const_iterator rbegin() const
    {
        return m_sharedvec->rbegin();
    }
    const_iterator crbegin() const
    {
        return m_sharedvec->crbegin();
    }
    iterator rend()
    {
        return m_sharedvec->rend();
    }
    const_iterator rend() const
    {
        return m_sharedvec->rend();
    }
    const_iterator crend() const
    {
        return m_sharedvec->crend();
    }

    // capacity & modifiers
    bool empty() const
    {
        return m_sharedvec->empty();
    }
    size_type size() const 
    { 
        return m_sharedvec->size(); 
    }
    size_type max_size() const 
    { 
        return m_sharedvec->max_size(); 
    }
    void reserve( size_type _size ) 
    { 
        m_sharedvec->reserve( _size ); 
    }
    size_type capacity() const
    { 
        return m_sharedvec->capacity(); 
    }
    void shrink_to_fit()
    {
        m_sharedvec->shrink_to_fit();
    }
    void clear()
    {
        m_sharedvec->clear();
    }

    // insert & emplace
    iterator insert( const_iterator pos, const T& value )
    {
        return m_sharedvec->insert( pos, value );
    }
    iterator insert( const_iterator pos, T&& value )
    {
        return m_sharedvec->insert( pos, std::move( value ));
    }
    iterator insert( const_iterator pos, size_type count, const T& value )
    {
        return m_sharedvec->insert( pos, count, value );
    }
    template< typename InputIt >
    iterator insert( const_iterator pos, InputIt first, InputIt last )
    {
        m_sharedvec->insert( pos, first, last );
    }
    iterator insert( const_iterator pos, std::initializer_list<T> init )
    {
        m_sharedvec->insert( pos, init );
    }
    template< class... Args >
    iterator emplace( const_iterator pos, Args&&... args )
    {
        m_sharedvec->emplace( pos, std::forward( args... ) );
    }

    // erase
    iterator erase( const_iterator pos )
    {
        return m_sharedvec->erase( pos );
    }
    iterator erase( const_iterator first, const_iterator last )
    {
        return m_sharedvec->erase( first, last );
    }

    // push_back, emplace_back, pop_back
    void push_back( const T& value )
    {
        m_sharedvec->push_back( value );
    }
    void push_back( T&& value )
    {
        m_sharedvec->push_back( std::move( value ) );
    }
    template< typename... Args > 
    reference emplace_back( Args&&... args )
    {
        return m_sharedvec->emplace_back( std::forward( args... ) );
    }
    void pop_back()
    {
        m_sharedvec->pop_back();
    }

    // resize
    void resize( size_type newsize ) 
    { 
        return m_sharedvec->resize( newsize ); 
    }
    void resize( size_type newsize, const T& value ) 
    { 
        return m_sharedvec->resize( newsize, value );
    }

    // swap
 
    const std::string& name() const 
    { 
        return m_name; 
    }
    
    std::string& name() 
    { 
        return m_name; 
    }
    
    series unique() const
    { 
        series out;
        out.m_name = m_name;
        out.m_sharedvec = m_sharedvec->unique();
        return out;
    }

    iterator unref( iterator it )
    {
        iterator newit = it;
        if ( m_sharedvec.use_count() > 1 ) {
            auto oldbegin = m_sharedvec->begin();
            std::shared_ptr<series_vector<T>> n = 
                std::make_shared<series_vector<T>>( *m_sharedvec );
            m_sharedvec = n;
            newit += (m_sharedvec->begin() - oldbegin);
        }
        return newit;
    }

    template< typename U >
    friend std::ostream& operator<<( std::ostream&, const series<U>& );

private:

    std::string m_name;
    std::shared_ptr<series_vector<T>> m_sharedvec;
};

} // namespace mf


#endif // INCLUDED_miniframe_series_h

