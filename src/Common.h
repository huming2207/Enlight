//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_COMMON_H
#define ENLIGHT_COMMON_H

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#if !CONFIG_FREERTOS_UNICORE
#define HAL_APP_RUNNING_CORE 0
#else
#define HAL_APP_RUNNING_CORE 1
#endif

// Convert the version info from hex digit to string
// From: https://stackoverflow.com/questions/5459868/c-preprocessor-concatenate-int-to-string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifndef ENLIGHT_GIT_REV
#define ENLIGHT_GIT_REV "Unknown"
#endif

#define ENLIGHT_GIT_REV_STR STR(ENLIGHT_GIT_REV)

#define ENLIGHT_VERSION "1.0-"
#define ENLIGHT_VERSION_FULL ENLIGHT_VERSION ENLIGHT_GIT_REV_STR

#endif //ENLIGHT_COMMON_H
