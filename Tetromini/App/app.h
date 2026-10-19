#pragma once

// Add extern C def to avoid C++ function name mangling
#ifdef __cplusplus
extern "C" {
#endif

void app_init(void);

#ifdef __cplusplus
}
#endif