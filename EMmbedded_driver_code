#include <linux/init.h>
#include <linux/module.h> 

#include <linux/fs.h> /* Included for character device driver support */
#include <linux/uaccess.h> /* Included for copy_to_user() */
#include <linux/cdev.h> /* Included for 2.6 or higher character driver support */

#include <linux/mod_devicetable.h> /* Included for struct i2c_device_id */
#include <linux/i2c.h> /* Included for I2C driver support */

MODULE_LICENSE("GPL");

/* Step 01: Mention the list of devices that you are interested in */
struct i2c_device_id mini2440_lm75_device_id = {
    .name = "lm75",
    .driver_data = 0,
};

/* Address list for the LM75 devices that we are interested */
const unsigned short mini2440_lm75_address_list[] = {0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, I2C_CLIENT_END};

/* Variable to hold the LM75 client pointer */
struct i2c_client *lm75_client = NULL;

int mini2440_lm75_probe(struct i2c_client *client, const struct i2c_device_id *device_id) {

    /* To hold the temperature read from the device */
    s32 temp = 0;

    printk("Inside the %s() function\n", __FUNCTION__);

    /* Check the device that got connected */
    printk("Device with I2C address 0x%x got connected\n", client->addr);

    /* Read the temperature of the device */
    temp = i2c_smbus_read_word_swapped(client /* Device */, 0 /* Register Offset, 0 => Temperature */);
    temp = (temp >> 7); /* D6 to D0 is not valid */
    temp = (temp >> 1); /* Because resolution is in 0.5 degree C. We are ok to loose 0.5 reads */
    printk("Temperature measured = %d\n", temp);

    /* Save the client pointer for later use */
    lm75_client = client;

    return 0;
}

int mini2440_lm75_remove(struct i2c_client *client) {
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Reset the client pointer */
    lm75_client = NULL;

    return 0;
}

int mini2440_lm75_detect(struct i2c_client *client, struct i2c_board_info *board) {
    printk("Inside the %s() function\n", __FUNCTION__);

    /* You can check the I2C functionality provided by the bus / platform driver */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA)) {
        return -EINVAL;
    }

    /* Inform the kernel about the driver that is handling this device */
    strcpy(board->type, "lm75"); /* lm75 should be the same string that you have registered / initialied in the i2c_driver structure */

    return 0;
}

/* Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function */
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

/* 
 * Function to read the TEMP, OS, HYST and copy them to the user space buffer.
 */
unsigned char buffer[256];
unsigned char toggle_read = 0;

ssize_t mini2440_lm75_read(struct file *pFile, char __user *pBuffer, size_t length, loff_t *pOffset)
{
    /* Variable to hold temperature, OS and HYST */
    s32 temp = 0; 
    s32 os = 0;
    s32 hyst = 0;

    /* Based on the toggle flag, return the actual data or zero */
    if (toggle_read) {
        toggle_read = 0;
        return 0;
    } else {
        toggle_read = 1;
    }

    if (lm75_client != NULL) {
        /* Read the temperature */
        temp = i2c_smbus_read_word_swapped(lm75_client /* Device */, 0 /* Register Offset, 0 => Temperature */);
        temp = (temp >> 7); /* D6 to D0 is not valid */
     
        /* Read the os */
        os = i2c_smbus_read_word_swapped(lm75_client /* Device */, 3 /* Register Offset, 3 => os */);
        os = (os >> 7); /* D6 to D0 is not valid */
     
        /* Read the hyst */
        hyst = i2c_smbus_read_word_swapped(lm75_client /* Device */, 2 /* Register Offset, 2 => hyst */);
        hyst = (hyst >> 7); /* D6 to D0 is not valid */
    }
   
    /* Format the data to be returned to the user */
    memset(buffer, 0, 256);
    sprintf(buffer, "Temperature = %d.%s, OS = %d.%s, HYST = %d.%s\n",
            temp >> 1, (temp & 0x1)?"5":"0", os >> 1, (os & 0x1)?"5":"0", hyst >> 1, (hyst & 0x1)?"5":"0");

    /* Copy the data to the user space buffer */
    copy_to_user(pBuffer, buffer, strlen(buffer) + 1 /* For the NULL Termination */);
 
    return strlen(buffer) + 1;
}

/* Function to convert a string to integer */
s32 atoi(const char *string) {
    
    int index = 0;
    int temp = 0;

    for (index = 0; index < strlen(string); index++) {
        /* printk("0x%x ", string[index]); */
        temp = temp * 10 + (string[index] - 0x30);
    }
    /* printk("\n"); */

    return temp;

}

/*
 * Write function to modify the hyst and os select on the device.
 * Since the same function is used for modifying the HYST and OS, we will use the following convention.
 * 
 *     Data Format: CDDD
 *         Where C   = 'O' for OS, and 'H' for HYST
 *               DDD = The Decimal Value of the temperature to be programmed.
 */
ssize_t mini2440_lm75_write(struct file *pFile, const char __user *pBuffer, size_t length, loff_t *pOffset)
{
    s32 temperature = 0;
    int index = 0;

    /* Read the data passed by the user program */
    copy_from_user(buffer, pBuffer, length);

    /* Remove an \n \r characters */
    for (index = length - 1; index > 0; index--) {
        if ((buffer[index] == '\n') || (buffer[index] == '\r')) {
            buffer[index] = '\0';
        }
    }

    /* Convert the string to number */
    temperature = atoi(&(buffer[1]));
    printk("Temperature to be programmed: %d\n", temperature);
 
    /* Depending on the first character in the buffer, update the OS or HSYT */
    if (buffer[0] == 'O') {
        i2c_smbus_write_word_swapped(lm75_client /* Device */, 3 /* Register Offset, 3 => os */, temperature << 8);
    } else if (buffer[0] == 'H') {
        i2c_smbus_write_word_swapped(lm75_client /* Device */, 2 /* Register Offset, 2 => hyst */, temperature << 8);
    }

    return length;
}

int mini2440_lm75_open(struct inode *pInode, struct file *pFile)
{
    return 0;
}

int mini2440_lm75_close(struct inode *pInode, struct file *pFile)
{
    return 0;
}
	
/* Define the file operations structure indicating the functions to be called when the device is accessed */
struct file_operations mini2440_lm75_file_ops = 
{
    .owner   = THIS_MODULE,
    .open    = mini2440_lm75_open,
    .release = mini2440_lm75_close,
    .read    = mini2440_lm75_read,
    .write   = mini2440_lm75_write,
};

/* To hold the major number for the device */
dev_t lm75_major_minor_number /* = MKDEV(240, 10) */;

/* Structure to hold the character driver information */
struct cdev lm75_device;

/* Macro to indicate the number of instances that we are going to support */
#define LM75_NUMBER_OF_INSTANCES    2

int mini2440_lm75_init(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 03: Register your i2c driver with the kernel */
    i2c_add_driver(&mini2440_lm75_driver);
    /* i2c_register_driver(THIS_MODULE, &mini2440_lm75_driver); */

    /* Inform the kernel about your character device driver */
    /* Register the character device driver using the 2.6 or higher style of interface */
    /* extern int register_chrdev_region(dev_t, unsigned, const char *); */
    // register_chrdev_region(lm75_major_minor_number, 1 /* Number of Devices */, "lm75_drv"); 
    /* extern int alloc_chrdev_region(dev_t *, unsigned, unsigned, const char *); */
    alloc_chrdev_region(&lm75_major_minor_number, 0 /* First Minor Number for the device */, LM75_NUMBER_OF_INSTANCES, "lm75_drv"); 

    /* Register the file operations for the major number / minor number device for 'n' number of instances */
    /* void cdev_init(struct cdev *, const struct file_operations *); */
    cdev_init(&lm75_device, &mini2440_lm75_file_ops);

    /* Link the major / minor number of the device, 'n' instance of the device and the file operations */
    /* int cdev_add(struct cdev *, dev_t, unsigned); */
    cdev_add(&lm75_device, lm75_major_minor_number /* First Device */, LM75_NUMBER_OF_INSTANCES /* Number of Instances */);

    return 0;
}

void mini2440_lm75_exit(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 99: Unregister your i2c  driver from the kernel */
    i2c_del_driver(&mini2440_lm75_driver);

    /* Inform the kernel that you are not going to handle the character driver any more */
    /* Delete the device */
    /* void cdev_del(struct cdev *); */
    cdev_del(&lm75_device);

    /* Release the major / minor number region */
    /* extern void unregister_chrdev_region(dev_t, unsigned); */
    unregister_chrdev_region(lm75_major_minor_number, LM75_NUMBER_OF_INSTANCES /* Number of instances */);

}

module_init(mini2440_lm75_init);
module_exit(mini2440_lm75_exit);

------------------------------------------------------------------
#include <linux/init.h>
#include <linux/module.h> 

#include <linux/fs.h> /* Included for character device driver support */
#include <linux/uaccess.h> /* Included for copy_to_user() */
#include <linux/cdev.h> /* Included for 2.6 or higher character driver support */

#include <linux/mod_devicetable.h> /* Included for struct i2c_device_id */
#include <linux/i2c.h> /* Included for I2C driver support */

MODULE_LICENSE("GPL");

/* Step 01: Mention the list of devices that you are interested in */
struct i2c_device_id mini2440_pca8574_device_id = {
    .name = "pca8574",
    .driver_data = 0,
};

/* Address list for the pca8574 devices that we are interested */
const unsigned short mini2440_pca8574_address_list[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, I2C_CLIENT_END};

/* Variable to hold the pca8574 client pointer */
struct i2c_client *pca8574_client = NULL;

int mini2440_pca8574_probe(struct i2c_client *client, const struct i2c_device_id *device_id) {

    printk("Inside the %s() function\n", __FUNCTION__);

    /* Check the device that got connected */
    printk("Device with I2C address 0x%x got connected\n", client->addr);

    /* Save the client handle for future use */
    pca8574_client = client;

    return 0;
}

int mini2440_pca8574_remove(struct i2c_client *client) {

    printk("Inside the %s() function\n", __FUNCTION__);

    /* Reset the client handle */
    pca8574_client = NULL;

    return 0;
}

int mini2440_pca8574_detect(struct i2c_client *client, struct i2c_board_info *board) {
    printk("Inside the %s() function\n", __FUNCTION__);

    /* You can check the I2C functionality provided by the bus / platform driver */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
        return -EINVAL;
    }

    /* Inform the kernel about the driver that is handling this device */
    strcpy(board->type, "pca8574"); /* pca8574 should be the same string that you have registered / initialied in the i2c_driver structure */

    return 0;
}

/* Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function */
/* Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function */
struct i2c_driver mini2440_pca8574_driver = {
    .class = I2C_CLASS_HWMON,
    .probe = mini2440_pca8574_probe,
    .remove = mini2440_pca8574_remove,
    .driver = {
        .name = "pca8574",
    },
    .id_table = &mini2440_pca8574_device_id,
    .detect = mini2440_pca8574_detect,
    .address_list = mini2440_pca8574_address_list,	 
};

/* 
 * Function to read the state of the IO expander pins 
 */
unsigned char buffer[256];
unsigned char toggle_read = 0;

ssize_t mini2440_pca8574_read(struct file *pFile, char __user *pBuffer, size_t length, loff_t *pOffset)
{
    /* To hold the IO expander state information */
    s32 data = 0;

    /* Based on the toggle flag, return the actual data or zero */
    if (toggle_read) {
        toggle_read = 0;
        return 0;
    } else {
        toggle_read = 1;
    }

    /* Read the state of the IO expander lines */
    if (pca8574_client != NULL) {
        data = i2c_smbus_read_byte(pca8574_client);
    }

    /* Format and copy the data to user space */
    memset(buffer, 0, 256);
    sprintf(buffer, "Line State: 0x%x\n", data);
    copy_to_user(pBuffer, buffer, strlen(buffer) + 1);

    return strlen(buffer) + 1; 
}

/* Function to convert a string to integer */
s32 atoi(const char *string) {
    
    int index = 0;
    int temp = 0;

    for (index = 0; index < strlen(string); index++) {
        /* printk("0x%x ", string[index]); */
        temp = temp * 10 + (string[index] - 0x30);
    }
    /* printk("\n"); */

    return temp;

}

/*
 * Function to write the state of the IO expander pins 
 */
ssize_t mini2440_pca8574_write(struct file *pFile, const char __user *pBuffer, size_t length, loff_t *pOffset)
{
    s32 data = 0;
    int index = 0;

    /* Get the data from the user space buffer */
    copy_from_user(buffer, pBuffer, length);

    /* Remove an \n \r characters */
    for (index = length - 1; index > 0; index--) {
        if ((buffer[index] == '\n') || (buffer[index] == '\r')) {
            buffer[index] = '\0';
        }
    }

    /* Convert string to integer */
    data = atoi(buffer);

    /* Set the data on the IO expander chip */
    i2c_smbus_write_byte(pca8574_client, data);

    return length;
}

int mini2440_pca8574_open(struct inode *pInode, struct file *pFile)
{
    return 0;
}

int mini2440_pca8574_close(struct inode *pInode, struct file *pFile)
{
    return 0;
}
	
/* Define the file operations structure indicating the functions to be called when the device is accessed */
struct file_operations mini2440_pca8574_file_ops = 
{
    .owner   = THIS_MODULE,
    .open    = mini2440_pca8574_open,
    .release = mini2440_pca8574_close,
    .read    = mini2440_pca8574_read,
    .write   = mini2440_pca8574_write,
};

/* To hold the major number for the device */
dev_t pca8574_major_minor_number /* = MKDEV(240, 10) */;

/* Structure to hold the character driver information */
struct cdev pca8574_device;

/* Macro to indicate the number of instances that we are going to support */
#define pca8574_NUMBER_OF_INSTANCES    2

int mini2440_pca8574_init(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 03: Register your i2c driver with the kernel */
    i2c_add_driver(&mini2440_pca8574_driver);
    /* i2c_register_driver(THIS_MODULE, &mini2440_pca8574_driver); */

    /* Inform the kernel about your character device driver */
    /* Register the character device driver using the 2.6 or higher style of interface */
    /* extern int register_chrdev_region(dev_t, unsigned, const char *); */
    // register_chrdev_region(pca8574_major_minor_number, 1 /* Number of Devices */, "pca8574_drv"); 
    /* extern int alloc_chrdev_region(dev_t *, unsigned, unsigned, const char *); */
    alloc_chrdev_region(&pca8574_major_minor_number, 0 /* First Minor Number for the device */, pca8574_NUMBER_OF_INSTANCES, "pca8574_drv"); 

    /* Register the file operations for the major number / minor number device for 'n' number of instances */
    /* void cdev_init(struct cdev *, const struct file_operations *); */
    cdev_init(&pca8574_device, &mini2440_pca8574_file_ops);

    /* Link the major / minor number of the device, 'n' instance of the device and the file operations */
    /* int cdev_add(struct cdev *, dev_t, unsigned); */
    cdev_add(&pca8574_device, pca8574_major_minor_number /* First Device */, pca8574_NUMBER_OF_INSTANCES /* Number of Instances */);

    return 0;
}

void mini2440_pca8574_exit(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 99: Unregister your i2c  driver from the kernel */
    i2c_del_driver(&mini2440_pca8574_driver);

    /* Inform the kernel that you are not going to handle the character driver any more */
    /* Delete the device */
    /* void cdev_del(struct cdev *); */
    cdev_del(&pca8574_device);

    /* Release the major / minor number region */
    /* extern void unregister_chrdev_region(dev_t, unsigned); */
    unregister_chrdev_region(pca8574_major_minor_number, pca8574_NUMBER_OF_INSTANCES /* Number of instances */);

}

module_init(mini2440_pca8574_init);
module_exit(mini2440_pca8574_exit);


-----------------------------------------------


#include <linux/init.h>
#include <linux/module.h> 

#include <linux/fs.h> /* Included for character device driver support */
#include <linux/uaccess.h> /* Included for copy_to_user() */
#include <linux/cdev.h> /* Included for 2.6 or higher character driver support */

#include <linux/mod_devicetable.h> /* Included for struct i2c_device_id */
#include <linux/i2c.h> /* Included for I2C driver support */
#include <linux/delay.h> 

MODULE_LICENSE("GPL");

/*
 * Pin Out for the PCA8574 and 16x2 LCD Display 
 *
 *  1 => GND
 *  2 => VCC
 *  3 => Brightness Control 
 *  4 => Instruction / Data 		=> P0 Pin 	(0: Instruction, 1: Data)
 *  5 => Read / Write 			=> P1 Pin	(0: Write, 1: Read)
 *  6 => Enable			 	=> P2 Pin
 *  7 => Do
 *  8 => D1
 *  9 => D2
 * 10 => D3
 * 11 => D4				=> P4 Pin
 * 12 => D5 				=> P5 Pin
 * 13 => D6				=> P6 Pin
 * 14 => D7				=> P7 Pin
 * 15 => LED+ (Backlight)
 * 16 => LED- (Backlight)		=> P3 Pin	(1: Back Light On, 0: Back Light Off)
 */

/* Variable to hold the pca8574 client pointer */
struct i2c_client *pca8574_client = NULL;

/* Function to write a character to the LCD */
/* We need to know whether the character being writtien is a instruction / data, 8 bit mode / 4 bit mode */
void mini2440_pca8574_character_write(unsigned char instruction, unsigned char bit_mode_8, unsigned char data) {

    /* To hold the data to be written to the device */
    unsigned char temp = 0;

    /* Is this an instruction */
    if (instruction == 1) { /* Instruction */
        temp = temp & ~(0x1); /* Clear P0 Pin */
    } else { /* Data */
        temp = temp | 0x1; /* Set P0 Pin */
    }

    /* Determine between write and read operation */
    temp = temp & ~0x2; /* Clear P1 Pin */

    /* Irrespective of 8 bit mode or 4 bit mode, send the upper nibble */
    temp = temp | (data & 0xF0);

    /* Turn ON the back light all the time */
    temp = temp | (1 << 3);

    /* Call the I2C API to send the upper nibble */
    i2c_smbus_write_byte(pca8574_client, temp); /* At this stage, enable bit is not asserted */ 
    udelay(1); /* Wait for the data to settle down */
    i2c_smbus_write_byte(pca8574_client, temp | (1 << 2)); /* Assert the enable pin */
    udelay(1); /* Wait for the device to read the data */
    i2c_smbus_write_byte(pca8574_client, temp); /* Clear the enable pin */

    /* If this is not 8 bit mode, send the lower nibble */
    if (bit_mode_8 == 0) {
        temp = temp & 0x0F; /* This gives me the state of the control lines */
        temp = temp | ((data & 0xF) << 4);
     
        /* Call the I2C API to send the lower nibble */
        i2c_smbus_write_byte(pca8574_client, temp); /* At this stage, enable bit is not asserted */ 
        udelay(1); /* Wait for the data to settle down */
        i2c_smbus_write_byte(pca8574_client, temp | (1 << 2)); /* Assert the enable pin */
        udelay(1); /* Wait for the device to read the data */
        i2c_smbus_write_byte(pca8574_client, temp); /* Clear the enable pin */
    }
}

/* To hold the welcome message */
char message[] = "THIS IS A MESSAGE FOR THE 16X2 DISPLAY FOR TESTING";
/* char message[] = "THIS IS A MESSAGE FOR THE 16X2"; */

/* Function to write some text to the display */
void mini2440_pca8574_write_text(char *message_buffer) {

    int index = 0;
  
    /* Move to the first row first column */
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x80 /* First Row */);

    /* Send the data to the display */
    for (index = 0; (index < 16) && (index < strlen(message_buffer)); index++) {
        mini2440_pca8574_character_write(0 /* Data */, 0 /* 4 bit mode */, message_buffer[index]);
    }

    /* Check whether there is any more data */
    if (index < strlen(message_buffer)) {

        /* Move to the second row first column */
        mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0xC0 /* Second Row */);

        for (; (index < 32) && (index < strlen(message_buffer)); index++) {
            mini2440_pca8574_character_write(0 /* Data */, 0 /* 4 bit mode */, message_buffer[index]);
        }
    }
}

/* Function to intialize the LCD in 4 bit mode */
void mini2440_pca8574_lcd_init(void) {

    /* Follow the spec and configure the display */
    mdelay(15); 

    /* Send the instruction to configure the display in 8 bit mode */
    mini2440_pca8574_character_write(1, 1, 0x30);

    /* Wait for 4.1 msec */
    mdelay(5);

    /* Send the instruction to configure the display in 8 bit mode */
    mini2440_pca8574_character_write(1, 1, 0x30);

    /* Wait for some time */
    udelay(100);

    /* Send the instruction to configure the display in 8 bit mode */
    mini2440_pca8574_character_write(1, 1, 0x30);

    /* Send the instruction to configure the display in 8 bit mode */
    mini2440_pca8574_character_write(1, 1, 0x20);

    /* From here on we are in 4 bit mode */

    /* Send the instruction to configure the display in 4 bit mode, 2 Row Display, 5x7 Font */
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x28);

    /* Send the instruction to turn OFF the display, cursor and blink */
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x08);

    /* Clear the screen */
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x01);

    /* Set the display in increment mode, and no shift */
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x06);

    /* End of Initialization */
  
    /* To check whether the display is working, turn ON the display, cursor and blink */ 
    mini2440_pca8574_character_write(1 /* Instruction */, 0 /* 4 bit mode */, 0x0F);

    /* Add some text for display */
    mini2440_pca8574_write_text(message);
}

/* Step 01: Mention the list of devices that you are interested in */
struct i2c_device_id mini2440_pca8574_device_id = {
    .name = "pca8574",
    .driver_data = 0,
};

/* Address list for the pca8574 devices that we are interested */
const unsigned short mini2440_pca8574_address_list[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, I2C_CLIENT_END};

int mini2440_pca8574_probe(struct i2c_client *client, const struct i2c_device_id *device_id) {

    printk("Inside the %s() function\n", __FUNCTION__);

    /* Check the device that got connected */
    printk("Device with I2C address 0x%x got connected\n", client->addr);

    /* Save the client handle for future use */
    pca8574_client = client;

    /* Initialize the device and get it ready */
    mini2440_pca8574_lcd_init();

    return 0;
}

int mini2440_pca8574_remove(struct i2c_client *client) {

    printk("Inside the %s() function\n", __FUNCTION__);

    /* Reset the client handle */
    pca8574_client = NULL;

    return 0;
}

int mini2440_pca8574_detect(struct i2c_client *client, struct i2c_board_info *board) {
    printk("Inside the %s() function\n", __FUNCTION__);

    /* You can check the I2C functionality provided by the bus / platform driver */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
        return -EINVAL;
    }

    /* Inform the kernel about the driver that is handling this device */
    strcpy(board->type, "pca8574"); /* pca8574 should be the same string that you have registered / initialied in the i2c_driver structure */

    return 0;
}

/* Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function */
/* Step 02: Initialize the driver structure with the list of devices that you are handling, probe function, disconnect function */
struct i2c_driver mini2440_pca8574_driver = {
    .class = I2C_CLASS_HWMON,
    .probe = mini2440_pca8574_probe,
    .remove = mini2440_pca8574_remove,
    .driver = {
        .name = "pca8574",
    },
    .id_table = &mini2440_pca8574_device_id,
    .detect = mini2440_pca8574_detect,
    .address_list = mini2440_pca8574_address_list,	 
};

/* 
 * Function to read the state of the IO expander pins 
 */
unsigned char buffer[256];
unsigned char toggle_read = 0;

ssize_t mini2440_pca8574_read(struct file *pFile, char __user *pBuffer, size_t length, loff_t *pOffset)
{
    /* To hold the IO expander state information */
    s32 data = 0;

    /* Based on the toggle flag, return the actual data or zero */
    if (toggle_read) {
        toggle_read = 0;
        return 0;
    } else {
        toggle_read = 1;
    }

    /* Read the state of the IO expander lines */
    if (pca8574_client != NULL) {
        data = i2c_smbus_read_byte(pca8574_client);
    }

    /* Format and copy the data to user space */
    memset(buffer, 0, 256);
    sprintf(buffer, "Line State: 0x%x\n", data);
    copy_to_user(pBuffer, buffer, strlen(buffer) + 1);

    return strlen(buffer) + 1; 
}

/* Function to convert a string to integer */
s32 atoi(const char *string) {
    
    int index = 0;
    int temp = 0;

    for (index = 0; index < strlen(string); index++) {
        /* printk("0x%x ", string[index]); */
        temp = temp * 10 + (string[index] - 0x30);
    }
    /* printk("\n"); */

    return temp;

}

/*
 * Function to write the state of the IO expander pins 
 */
ssize_t mini2440_pca8574_write(struct file *pFile, const char __user *pBuffer, size_t length, loff_t *pOffset)
{
    s32 data = 0;
    int index = 0;

    /* Get the data from the user space buffer */
    copy_from_user(buffer, pBuffer, length);

    /* Remove an \n \r characters */
    for (index = length - 1; index > 0; index--) {
        if ((buffer[index] == '\n') || (buffer[index] == '\r')) {
            buffer[index] = '\0';
        }
    }

    /* Convert string to integer */
    data = atoi(buffer);

    /* Set the data on the IO expander chip */
    i2c_smbus_write_byte(pca8574_client, data);

    return length;
}

int mini2440_pca8574_open(struct inode *pInode, struct file *pFile)
{
    return 0;
}

int mini2440_pca8574_close(struct inode *pInode, struct file *pFile)
{
    return 0;
}
	
/* Define the file operations structure indicating the functions to be called when the device is accessed */
struct file_operations mini2440_pca8574_file_ops = 
{
    .owner   = THIS_MODULE,
    .open    = mini2440_pca8574_open,
    .release = mini2440_pca8574_close,
    .read    = mini2440_pca8574_read,
    .write   = mini2440_pca8574_write,
};

/* To hold the major number for the device */
dev_t pca8574_major_minor_number /* = MKDEV(240, 10) */;

/* Structure to hold the character driver information */
struct cdev pca8574_device;

/* Macro to indicate the number of instances that we are going to support */
#define pca8574_NUMBER_OF_INSTANCES    2

int mini2440_pca8574_init(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 03: Register your i2c driver with the kernel */
    i2c_add_driver(&mini2440_pca8574_driver);
    /* i2c_register_driver(THIS_MODULE, &mini2440_pca8574_driver); */

    /* Inform the kernel about your character device driver */
    /* Register the character device driver using the 2.6 or higher style of interface */
    /* extern int register_chrdev_region(dev_t, unsigned, const char *); */
    // register_chrdev_region(pca8574_major_minor_number, 1 /* Number of Devices */, "pca8574_drv"); 
    /* extern int alloc_chrdev_region(dev_t *, unsigned, unsigned, const char *); */
    alloc_chrdev_region(&pca8574_major_minor_number, 0 /* First Minor Number for the device */, pca8574_NUMBER_OF_INSTANCES, "pca8574_drv"); 

    /* Register the file operations for the major number / minor number device for 'n' number of instances */
    /* void cdev_init(struct cdev *, const struct file_operations *); */
    cdev_init(&pca8574_device, &mini2440_pca8574_file_ops);

    /* Link the major / minor number of the device, 'n' instance of the device and the file operations */
    /* int cdev_add(struct cdev *, dev_t, unsigned); */
    cdev_add(&pca8574_device, pca8574_major_minor_number /* First Device */, pca8574_NUMBER_OF_INSTANCES /* Number of Instances */);

    return 0;
}

void mini2440_pca8574_exit(void)
{
    printk("Inside the %s() function\n", __FUNCTION__);

    /* Step 99: Unregister your i2c  driver from the kernel */
    i2c_del_driver(&mini2440_pca8574_driver);

    /* Inform the kernel that you are not going to handle the character driver any more */
    /* Delete the device */
    /* void cdev_del(struct cdev *); */
    cdev_del(&pca8574_device);

    /* Release the major / minor number region */
    /* extern void unregister_chrdev_region(dev_t, unsigned); */
    unregister_chrdev_region(pca8574_major_minor_number, pca8574_NUMBER_OF_INSTANCES /* Number of instances */);

}

module_init(mini2440_pca8574_init);
module_exit(mini2440_pca8574_exit);
