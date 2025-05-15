#include "blfm_taskmanager.h"

int main(void) {
    // Initialize and start the system task manager
    blfm_taskmanager_start();

    // Should never reach here
    while (1) {}
}
