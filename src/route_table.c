#include <string.h>

#include <unistd.h>

#include "log.h"

#include "route_table.h"

#define R 255
#define TABLE_SIZE 1009

int rt_hash(const char* str) {
    int len = strlen(str);
    if (len == 0) return 0;

    int key = (int)str[0];
    for (int i = 1; i < len; ++i) {
        key = (key * R + (int)(str[i])) % TABLE_SIZE;
    }

    return key % TABLE_SIZE;
}

void rt_destroy_route(route_t* route) {
    if (!route) return;
    free((void*)route->subdir);
    free((void*)route->path);
    free((void*)route);
}

route_table_t* rt_create() {
    route_table_t* rt = malloc(sizeof(route_table_t));
    if (!rt) {
        wslog(ERRR, "Could not allocate memory for route table");
        return NULL;
    }

    rt->size = TABLE_SIZE;
    rt->count = 0;
    rt->routes = malloc(TABLE_SIZE * sizeof(route_t));
    if (!rt->routes) {
        wslog(ERRR, "Could not allocate memory for routes.");
        free(rt);
        return NULL;
    }

    for (int i = 0; i < rt->size; ++i) {
        rt->routes[i] = NULL;
    }

    return rt;
}

void rt_destroy(route_table_t* rt) {
    if (!rt) return;

    if (rt->routes) {
        for (int i = 0; i < rt->size; ++i) {
            rt_destroy_route(rt->routes[i]);
        }
    }

    free(rt->routes);
    free(rt);
}

int rt_add_route(route_table_t* rt, const char* subdir, const char* path) {
    int err;

    err = access(path, F_OK);
    if (err) {
        wslog(ERRR, "File with path (%s) does not exist.", path);
        return -1;
    }

    route_t* route = malloc(sizeof(route_t));
    if (!route) {
        wslog(ERRR, "Could not allocate memory for route");
        return -1;
    }

    route->subdir = malloc((strlen(subdir) + 1) * sizeof(char));
    if (!route->subdir) {
        wslog(ERRR, "Could not allocate memory for route subdirectory");
        rt_destroy_route(route);
        return -1;
    }
    strcpy(route->subdir, subdir);

    route->path = malloc((strlen(path) + 1) * sizeof(char));
    if (!route->path) {
        wslog(ERRR, "Could not allocate memory for route path");
        rt_destroy_route(route);
        return -1;
    }
    strcpy(route->path, path);

    int index = rt_hash(subdir);
    int max_checks = TABLE_SIZE;
    while (rt->routes[index] && --max_checks > 0) {
        index = (index + 1) % TABLE_SIZE;
    }
    if (max_checks == 0) {
        wslog(ERRR, "Table full, could not add new route");
        rt_destroy_route(route);
        return -1;
    }

    wslog(INFO, "Route subdirectory (%s) to path (%s) added.", subdir, route->path);
    rt->routes[index] = route;
    ++rt->size;
    return 0;
}

/*
int rt_remove_route(route_table_t* rt, const char* subdir) {
    int index = rt_hash(subdir);
    int max_checks = TABLE_SIZE;
    while (rt->routes[index] && !strcmp(rt->routes[index]->subdir, subdir) && --max_checks > 0) {
        index = (index + 1) % TABLE_SIZE;
    }
    if (!rt->routes[index] || max_checks == 0) {
        return -1;
    }

    wslog(INFO, "Route subdirectory (%s) to path (%s) removed.", subdir, rt->routes[index]->path);
    rt_destroy_route(rt->routes[index]);
    rt->routes[index] = NULL;
    return 0;
}
*/

int rt_get_path(route_table_t* rt, const char* subdir, char* buf) {
    int index = rt_hash(subdir);
    int max_checks = TABLE_SIZE;

    while (rt->routes[index] && strcmp(rt->routes[index]->subdir, subdir) && --max_checks > 0) {
        index = (index + 1) % TABLE_SIZE;
    }

    if (!rt->routes[index] || max_checks == 0) {
        if (!rt->not_found) {
            wslog(ERRR, "No route exists for subdirectory (%s), and no 404 path found.", subdir);
            return -1;
        }
        wslog(INFO, "No route for subdirectory (%s) exists, displaying 404 (%s).", subdir, rt->not_found);
        strcpy(buf, rt->not_found);
        return 0;
    }

    wslog(INFO, "Route found for subdirectory (%s) to path (%s).", subdir, rt->routes[index]->path);
    strcpy(buf, rt->routes[index]->path);
    return 0;
}

int rt_set_404(route_table_t* rt, const char* path) {
    int err;
    free((void *)rt->not_found);

    err = access(path, F_OK);
    if (err) {
        wslog(ERRR, "File with path (%s) does not exist.", path);
        return -1;
    }

    char* not_found = malloc((strlen(path) + 1) * sizeof(char));
    if (!not_found) {
        wslog(ERRR, "Could not allocate memory for 404 path.");
        return -1;
    }

    strcpy(not_found, path);
    rt->not_found = not_found;
    return 0;
}
