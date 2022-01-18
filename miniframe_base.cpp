#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>
#include "miniframe_base.hpp"

using namespace std;


namespace mf
{

static bool g_use_termio = false;

void set_use_termio( bool enable )
{
    g_use_termio = enable;
}

bool use_termio()
{
    return g_use_termio;
}

int get_termwidth()
{
    struct winsize w;
    if ( 0 == ioctl( STDOUT_FILENO, TIOCGWINSZ, &w ) )
    {
        return w.ws_col;
    }
    // stdin might be a pipe, not our ctty
    return -1;
}

const char * get_horzrule( size_t num )
{
    static const char horzrule[] = 
        "________________________________________"
        "________________________________________"
        "________________________________________"
        "________________________________________"
        "________________________________________"
        "________________________________________"
        "________________________________________"
        "________________________________________";
    auto horzrulesize = sizeof( horzrule ) - 1;
    num = std::min( num, horzrulesize );
    return horzrule + ( horzrulesize - num );
}

namespace detail
{
template<>
std::ostream& stringify( std::ostream & o, const char & t, bool )
{
    o << static_cast<int>(t);
    return o;
}

template<>
std::ostream& stringify( std::ostream & o, const unsigned char & t, bool )
{
    o << static_cast<int>(t);
    return o;
}
}

} // namespace mf

