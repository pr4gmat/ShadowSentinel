#include "protection.h"
#include <string.h>

page_protection_t parse_protection(const char* perm) {
    page_protection_t p;
    memset(&p, 0, sizeof(p));

    p.read    = (perm[0] == 'r');
    p.write   = (perm[1] == 'w');
    p.execute = (perm[2] == 'x');
    p.shared  = (perm[3] == 's' || perm[3] == 'S');

    p.raw = 0;
    if (p.read)    p.raw |= PROT_READ;
    if (p.write)   p.raw |= PROT_WRITE;
    if (p.execute) p.raw |= PROT_EXEC;

    return p;
}

const char* protection_to_string(page_protection_t p) {
    static char buf[16];

    buf[0] = p.read    ? 'r' : '-';
    buf[1] = p.write   ? 'w' : '-';
    buf[2] = p.execute ? 'x' : '-';
    buf[3] = p.shared  ? 's' : 'p';
    buf[4] = '\0';

    return buf;
}