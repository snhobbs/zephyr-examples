#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif
extern void setup_leds();
extern void led_work_handler(struct k_work *work);

#ifdef __cplusplus
};
#endif
