#include "csbuild.h"
#include "proxy.h"
#include <pthread.h>

int main() {
    const char * port = "12345";
    Proxy my_proxy(port);
    my_proxy.init_Proxy();
    return 1;
}
