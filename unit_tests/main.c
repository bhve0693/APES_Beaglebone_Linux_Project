#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fw_i2c.h"
#include "i2c_temp.h"
#include "unity.h"
#include "test_temperature.c"
#include "test_light.c"

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_init_pass);
    RUN_TEST(test_init_light_pass);
    RUN_TEST(test_read_temp_register_treg_addr);
    RUN_TEST(test_read_temp_register_configreg_addr);
    RUN_TEST(test_read_temp_register_datalow_addr);
    RUN_TEST(test_read_temp_register_datahigh_addr);
    RUN_TEST(test_read_temp_register_invalid_addr);

    RUN_TEST(test_write_temp_register_ptr_addr_fail);
    RUN_TEST(test_write_temp_register_ptr_addr_success);
    RUN_TEST(test_write_temp_register_configreg_addr);
    RUN_TEST(test_write_temp_register_datalow_addr);
    RUN_TEST(test_write_temp_register_datahigh_addr);
    RUN_TEST(test_write_temp_register_invalid_addr);


    RUN_TEST(test_shutdown_mode);
    RUN_TEST(test_continuous_conversion_mode_valid);
    RUN_TEST(test_continuous_conversion_mode_valid);
    RUN_TEST(test_temp_read);

    return UNITY_END();
}