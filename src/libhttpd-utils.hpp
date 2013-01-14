#ifndef LIBHTTPDUTILS_HPP
#define LIBHTTPDUTILS_HPP

// start the webserver, returns when server terminates (hit enter key)
int startServer(int port, const char* serverType, GMS::Data* data);

#endif // LIBHTTPDUTILS_HPP
