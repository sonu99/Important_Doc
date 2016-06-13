
....................................................................................................................................
        LM35 temp sensor drivr flow doc
....................................................................................................................................
/*
I2C controller driver....
I2C based LM75 Driver...
.................................
we are using here 3 function.
1>probe
2>detect
3>Remove
----------------------------------------------------------------------------------------------------------
Step 01: Mention the list of devices that you are interested in 
Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function 
struct i2c_driver mini2440_lm75_driver = {
    .class = I2C_CLASS_HWMON,
    .probe = mini2440_lm75_probe,
    .remove = mini2440_lm75_remove,
    .driver = {
        .name = "lm75",
    },
    .id_table = &mini2440_lm75_device_id,
    .detect = mini2440_lm75_detect,
    .address_list = mini2440_lm75_address_list,	 
};
now host driver know about your driver so if LM75 detected into i2c bus it will call your detect function with handller of that device.
....if your driver is ready to handle device then you can inform i2c host driver using board->type it will check the same name..if it is true then
it will call probe function..and it will display room temp..

------>if device is remove then I2C controller driver will call Remove function and your remove pointer should set handller to null...
struct i2c_client *lm75_client = NULL;
---------------------------------------------------------------------------------------------------------------
if you want to set hyst and os temp then..you have to write function..and from user side you have to use echo cmd...with device name..so it will
call write function of your device and you can pass data to write function..
*/
