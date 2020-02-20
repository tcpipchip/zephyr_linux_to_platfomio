/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <sensor.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)


static volatile u8_t mfg_data[] = { 0x00, 0x00, 0xaa, 0xbb };

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 4)
};

/* Set Scan Response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void bt_ready(int err)
{
	if (err) {
		return;
	}

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		return;
	}

}

unsigned int count = 0;

// sensor
static struct device* dev_bme280;


// print BME280 data
void update_sensor_data()
{

    // get sensor data
    struct sensor_value temp, humidity;

    //sensor_sample_fetch(dev_bme280);
    //sensor_channel_get(dev_bme280, SENSOR_CHAN_AMBIENT_TEMP, &temp);	
    //sensor_channel_get(dev_bme280, SENSOR_CHAN_PRESS, &press);
    //sensor_channel_get(dev_bme280, SENSOR_CHAN_HUMIDITY, &humidity);

	mfg_data[2] = (uint8_t) temp.val1;
	mfg_data[3] = (uint8_t) humidity.val1;

	count++;

	mfg_data[2] = (uint8_t) count;
	mfg_data[3] = (uint8_t) count;

}


void main(void)
{
	int err;
	
	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
	}

	dev_bme280 = device_get_binding("BME280");

	while(1)
	{
		k_sleep(K_MSEC(4000));
		update_sensor_data();
		bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
	}

}
