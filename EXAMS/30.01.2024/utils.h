#include <unistd.h>

#define MAX_BYTES_SEND 1024
#define DEBUG
#define PORT 6002

struct MSG
{
    char filename[1024];
    char op;
    size_t start_byte;
    size_t size_bytes;
};
typedef struct MSG message_container_t;