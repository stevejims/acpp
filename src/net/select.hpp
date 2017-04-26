#ifndef NET_SELECT_HPP
#define NET_SELECT_HPP

#include "sys/select.h"

namespace net {

class fdset {
public:
    fdset();
    void add(int fd);
    void del(int fd);
    bool readable(int fd) const;

    friend void select_inplace(fdset &);
    friend fdset select(const fdset &);
    
private:
    sys_fdset_t fdset_;
};

} /* namespace net */

#endif /* NET_SELECT_HPP */
