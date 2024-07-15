// #import "fishhook.h"

#include <stdio.h>
#include <objc/runtime.h>
#include <objc/message.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <fishhook.h>

// Function pointer type for the original function
typedef CFStringRef (*OriginalFuncType)(void*);

// Pointer to store the original function
static OriginalFuncType original_SCNetworkInterfaceGetHardwareAddressString = NULL;

// Our replacement function
CFStringRef replaced_SCNetworkInterfaceGetHardwareAddressString(void* interface) {
    // Generate a random MAC address
    char mac[18];
    static const char hex_digits[] = "0123456789ABCDEF";
    
    // Ensure the first byte is even (for unicast) and locally administered
    mac[0] = hex_digits[2 * (rand() % 8)];
    mac[1] = hex_digits[rand() % 16];

    for (int i = 2; i < 17; i++) {
        if (i % 3 == 2) {
            mac[i] = ':';
        } else {
            mac[i] = hex_digits[rand() % 16];
        }
    }
    mac[17] = '\0';

    printf("[NSLOGIC] Returning random MAC: %s\n", mac);
    return CFStringCreateWithCString(NULL, mac, kCFStringEncodingASCII);
}

__attribute__((constructor))
static void initializer() {
    printf("[NSLOGIC] Library injection started...\n");

    // Seed the random number generator
    srand(time(NULL));

    // Rebind the symbol
    struct rebinding rebindings[] = {
        {"SCNetworkInterfaceGetHardwareAddressString", replaced_SCNetworkInterfaceGetHardwareAddressString, (void *)&original_SCNetworkInterfaceGetHardwareAddressString}
    };
    
    if (rebind_symbols(rebindings, 1) != 0) {
        printf("[NSLOGIC] Failed to rebind symbol\n");
        return;
    }

    printf("[NSLOGIC] Function successfully overridden\n");
}
