#ifndef ROUTING
#define ROUTING

#include <stdlib.h>

typedef struct {
    char* subdir;
    char* path;
} route_t;

typedef struct {
    route_t** routes;
    size_t size;
    unsigned int count;
    const char* not_found;
} route_table_t;

// Create a route table
route_table_t* rt_create();

// Destroy a route table
void rt_destroy(route_table_t* rt);

// Add a route
int rt_add_route(route_table_t* rt, const char* subdir, const char* path);

// Remove a route
// int rt_remove_route(route_table_t* rt, const char* subdir);

// Get the path associated with a route subdirectory
int rt_get_path(route_table_t* rt, const char* subdir, char* buf);

// Set the 404 path
int rt_set_404(route_table_t* rt, const char* path);

#endif // ROUTING
