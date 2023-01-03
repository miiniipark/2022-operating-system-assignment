#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "cmd.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Minhee Park");
MODULE_DESCRIPTION("A simple motor driver for controlling the motor speed using PWM");

/* Buffer for data */
static char buffer[255];

/* Variables for controlling the motor */
static int usec_on_min = 500;
static int usec_on_max = 1000;
static int usec_off_min = 500;
static int usec_off_max = 1000;
static int state = 0;

/* Variables for kthread */
static struct task_struct* task;

#define KTHREAD_NAME "motor_thread"

/* Variables for device and device class */
static dev_t motor_device_no;
static struct class* motor_class;
static struct cdev motor_device;

#define DRIVER_NAME "motor_driver"
#define DRIVER_CLASS "MotorModuleClass"
#define GPIO_23_OUT 23

/* This function is called, when trying to start the motor */
static int motor_thread(void* args)
{
	while (1) {
		gpio_set_value(GPIO_23_OUT, 1);
		usleep_range(usec_on_min, usec_on_max);
		gpio_set_value(GPIO_23_OUT, 0);
		usleep_range(usec_off_min, usec_off_max);
		if (kthread_should_stop()) {
			break;
		}
	}
	return 0;
}

/*
 * This function is called, when user writes data to buffer
 */
static ssize_t motor_write(struct file* instance, const char* user_buffer, size_t count, loff_t* offs)
{
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data from user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);

	/* Set the power-on time in microseconds */
	usec_on_min = (int)simple_strtol(buffer, NULL, 10);
	usec_on_max = usec_on_min * 2;
	pr_info("motor_write - Update the power-on time to %dms!\n", usec_on_min);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/*
 * This function is called, when the device file is opened
 */
static int motor_open(struct inode* device_file, struct file* instance)
{
	pr_info("%s opened...!!!\n", DRIVER_NAME);
	return 0;
}

/*
 * This function is called, when the device file is closed
 */
static int motor_close(struct inode* device_file, struct file* instance)
{
	pr_info("%s closed...!!!\n", DRIVER_NAME);
	return 0;
}

/*
 * This function is called, when the user calls ioctl in user mode
 */
static long int motor_ioctl(struct file* instance, unsigned cmd, unsigned long arg)
{
	switch (cmd) {
	case USEC_ON:
		copy_from_user(&usec_on_min, (int)arg, sizeof(usec_on_min));
		usec_on_max = usec_on_min * 2;
		pr_info("motor_ioctl - Update the power-on time to %dms!\n", usec_on_min);
		break;
	case USEC_OFF:
		copy_from_user(&usec_off_min, (int)arg, sizeof(usec_off_min));
		usec_off_max = usec_off_min * 2;
		pr_info("motor_ioctl - Update the power-off time to %dms!\n", usec_on_min);
		break;
	case STATE:
		copy_from_user(&state, (int)arg, sizeof(state));
		if (state == 1) {
			task = kthread_run(motor_thread, NULL, KTHREAD_NAME);
			pr_info("motor_ioctl - Motor thread started!\n");
		}
		else {
			kthread_stop(task);
			pr_info("motor_ioctl - Motor thread stopped!\n");
		}
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = motor_open,
	.release = motor_close,
	.write = motor_write,
	.unlocked_ioctl = motor_ioctl
};

/*
 * This function is called, when the module is loaded into the kernel
 */
static int __init motor_init(void)
{
	/* Allocate device number */
	if (alloc_chrdev_region(&motor_device_no, 0, 1, DRIVER_NAME) < 0) {
		pr_err("Unable to allocate device number!\n");
		return -1;
	}
	pr_info("%s - Device No. Major: %d, Minor: %d was registered!\n", DRIVER_NAME, MAJOR(motor_device_no), MINOR(motor_device_no));

	/* Create device class */
	if ((motor_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		pr_err("Unable to create device class!\n");
		goto ClassError;
	}

	/* Create device file */
	if (device_create(motor_class, NULL, motor_device_no, NULL, DRIVER_NAME) == NULL) {
		pr_err("Unable to create device file!\n");
		goto FileError;
	}

	/* Initialize device */
	cdev_init(&motor_device, &fops);

	/* Add device to kernel */
	if (cdev_add(&motor_device, motor_device_no, 1) < 0) {
		pr_err("Unable to add device to kernel!\n");
		goto AddError;
	}

	/* Request GPIO 23 */
	if (gpio_request(GPIO_23_OUT, "GPIO_23_OUT") < 0) {
		pr_err("Unable to request GPIO %d!\n", GPIO_23_OUT);
		goto RequestError;
	}

	/* Set GPIO 23 direction */
	if (gpio_direction_output(GPIO_23_OUT, 0) < 0) {
		pr_err("Unable to set GPIO %d to output!\n", GPIO_23_OUT);
		goto DirectionError;
	}

	pr_info("%s inserted...!!!\n", DRIVER_NAME);
	return 0;
DirectionError:
	gpio_free(GPIO_23_OUT);
RequestError:
	cdev_del(&motor_device);
AddError:
	device_destroy(motor_class, motor_device_no);
FileError:
	class_destroy(motor_class);
ClassError:
	unregister_chrdev_region(motor_device_no, 1);
	return -1;
}

/*
 * This function is called, when the module is removed from the kernel
 */
static void __exit motor_exit(void)
{
	gpio_set_value(GPIO_23_OUT, 0);
	gpio_free(GPIO_23_OUT);
	cdev_del(&motor_device);
	device_destroy(motor_class, motor_device_no);
	class_destroy(motor_class);
	unregister_chrdev_region(motor_device_no, 1);
	pr_info("%s removed...!!!\n", DRIVER_NAME);
}

module_init(motor_init);
module_exit(motor_exit);
