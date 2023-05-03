
#include <dmfserver/utility/dm_hash.h>


// BKDR Hash Function
static unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131;   // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*str){
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}