// SPDX-License-Identifier: MIT
/*
 * API for color sensors
 *
 *  Based on https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/pupdevices/pb_type_pupdevices_colorsensor.c
 *  Based on https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/util_pb/pb_color_map.c
 *
 * Copyright (c) 2018-2020 The Pybricks Authors
 * Modifications for TOPPERS/APS3 Kernel Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/cb_error.h>
#include <cbricks/pup/colorsensor.h>
#include <pbio/color.h>
#define CB_COLOR_MAP_SIZE 30

pbio_color_hsv_t cb_color_map_default[] = {
	{ PBIO_COLOR_HUE_RED, 100, 100 },
  { PBIO_COLOR_HUE_YELLOW, 100, 100	},
  { PBIO_COLOR_HUE_GREEN, 100, 100 },
  {	PBIO_COLOR_HUE_BLUE, 100, 100	},
  {	0, 0, 100	},//White
  {	0, 0, 0	},//None
};
typedef struct {
	int32_t size;
	pbio_color_hsv_t colors[CB_COLOR_MAP_SIZE];
} cb_color_map_t;
cb_color_map_t color_map;

void cb_color_map_rgb_to_hsv(const pbio_color_rgb_t *rgb, pbio_color_hsv_t *hsv) {

    // Standard conversion
    pbio_color_rgb_to_hsv(rgb, hsv);

    // For very low values, saturation is not reliable
    if (hsv->v <= 3) {
        hsv->s = 0;
    }

    // For very low values, hue is not reliable
    if (hsv->s <= 3) {
        hsv->h = 0;
    }

    // Slight shift for lower hues to make yellow somewhat more accurate
    if (hsv->h < 40) {
        uint8_t offset = ((hsv->h - 20) << 8) / 20;
        int32_t scale = 200 - ((100 * (offset * offset)) >> 16);
        hsv->h = hsv->h * scale / 100;
    }

    // Value and saturation correction
    hsv->s = hsv->s * (200 - hsv->s) / 100;
    hsv->v = hsv->v * (200 - hsv->v) / 100;
}

int32_t cb_get_hsv_cost(const pbio_color_hsv_t *x, const pbio_color_hsv_t *c) {

    // Calculate the hue error
    int32_t hue_error;

    if (c->s <= 5 || x->s <= 5) {
        // When comparing against unsaturated colors,
        // the hue error is not so relevant.
        hue_error = 0;
    } else {
        hue_error = c->h > x->h ? c->h - x->h : x->h - c->h;
        if (hue_error > 180) {
            hue_error = 360 - hue_error;
        }
    }

    // Calculate the value error:
    int32_t value_error = x->v > c->v ? x->v - c->v : c->v - x->v;

    // Calculate the saturation error, with extra penalty for low saturation
    int32_t saturation_error = x->s > c->s ? x->s - c->s : c->s - x->s;
    saturation_error += (100 - c->s) / 2;

    // Total error
    return hue_error * hue_error + 5 * saturation_error * saturation_error + 2 * value_error * value_error;
}

// Get a discrete color that matches the given hsv values most closely
pbio_color_hsv_t cb_color_map_get_color(cb_color_map_t *color_map, pbio_color_hsv_t *hsv) {

    // Initialize minimal cost to maximum
    pbio_color_hsv_t match;
    int32_t cost_now = INT32_MAX;
    int32_t cost_min = INT32_MAX;

		int32_t n = color_map->size;
	
		pbio_color_hsv_t *colors = color_map->colors;
    // Compute cost for each candidate
    for (size_t i = 0; i < n; i++) {

        // Evaluate the cost function
        cost_now = cb_get_hsv_cost(hsv, &colors[i]);

        // If cost is less than before, update the minimum and the match
        if (cost_now < cost_min) {
            cost_min = cost_now;
            match = colors[i];
        }
    }

    return match;
}


static pbio_error_t pup_color_sensor__get_hsv_reflected(pup_device_t *pdev, pbio_color_hsv_t *hsv){
	    // Read RGB
    int32_t data[4];
		pbio_error_t err;
    err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_COLOR_SENSOR__RGB_I, data);
		if(err != PBIO_SUCCESS){
			syslog(LOG_ERROR, "pup_color_sensor__get_hsv_reflected() failed.");
			return err;
		}
    const pbio_color_rgb_t rgb = {
        .r = data[0] == 1024 ? 255 : data[0] >> 2,
        .g = data[1] == 1024 ? 255 : data[1] >> 2,
        .b = data[2] == 1024 ? 255 : data[2] >> 2,
    };

    // Convert to HSV
    cb_color_map_rgb_to_hsv(&rgb, hsv);
		return err;
}

static pbio_error_t pup_color_sensor__get_hsv_ambient(pup_device_t *pdev, pbio_color_hsv_t *hsv){
	    // Read SHSV mode (light off). This data is not available in RGB format
    int32_t data[4];
		pbio_error_t err;
    err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_COLOR_SENSOR__SHSV, data);
		if(err != PBIO_SUCCESS){
			syslog(LOG_ERROR, "pup_color_sensor__get_hsv_ambient() failed.");
			return err;
		}

    // Scale saturation and value to match 0-100% range in typical applications
    hsv->h = data[0];
    hsv->s = data[1] / 10;
    if (hsv->s > 100) {
        hsv->s = 100;
    }
    hsv->v = data[2] / 10;
    if (hsv->v > 100) {
        hsv->v = 100;
    }
		return err;
}

pup_device_t *pup_color_sensor_get_device(pbio_port_id_t port) {
  // Get iodevices
	pup_device_t *pdev = pup_device_get_device(port, PBIO_IODEV_TYPE_ID_SPIKE_COLOR_SENSOR);
	pbio_color_hsv_t hsv;

	// Do one reading
	pup_color_sensor__get_hsv_reflected(pdev, &hsv);

	// init color_map
	// dafault detectable color are {RED, YELLOW, GREEN, BLUE, WHITE, NONE}
	color_map.size = sizeof(cb_color_map_default) / sizeof(pbio_color_hsv_t);
	for(size_t i = 0; i < color_map.size; i++){
		color_map.colors[i] = cb_color_map_default[i];
	}

  return pdev;
}

pbio_color_hsv_t pup_color_sensor_hsv(pup_device_t *pdev, bool surface) {
  pbio_error_t err;
	pbio_color_hsv_t hsv;

	if(surface){
		err = pup_color_sensor__get_hsv_reflected(pdev, &hsv);
	}else{
		err = pup_color_sensor__get_hsv_ambient(pdev, &hsv);
	}
	
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_color_sensor_hsv() failed.");
		hsv.h = 0; hsv.s = 0; hsv.v = 0;
  }
	return hsv;
}

pbio_color_hsv_t pup_color_sensor_color(pup_device_t *pdev, bool surface) {

  pbio_error_t err;
	pbio_color_hsv_t hsv;

	if(surface){
		err = pup_color_sensor__get_hsv_reflected(pdev, &hsv);
	}else{
		err = pup_color_sensor__get_hsv_ambient(pdev, &hsv);
	}
	
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_color_sensor_color() failed.");
		hsv.h = 0; hsv.s = 0; hsv.v = 0;
  }
	//need convert into one of the default color hsv.
	
	return cb_color_map_get_color(&color_map, &hsv);
}

int32_t pup_color_sensor_reflection(pup_device_t *pdev) {
	int32_t data[4];
  pbio_error_t err;

  err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_COLOR_SENSOR__RGB_I, data);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_color_sensor_reflection() failed.");
    data[0] = 0; data[1] = 0; data[2] = 0;
  }
  return (data[0] + data[1] + data[2]) * 100 / 3072;
}

int32_t pup_color_sensor_ambient(pup_device_t *pdev) {
	
  int32_t data[4];
  pbio_error_t err;

  err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_COLOR_SENSOR__SHSV, data);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_color_sensor_ambient() failed.");
    data[2] = 0;
  }
  return data[2] / 100;
}


pbio_error_t pup_color_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3) {
	
  int32_t brightness_values[3] = {bv1, bv2, bv3};
  return pup_device_set_values(pdev, PBIO_IODEV_MODE_PUP_COLOR_SENSOR__LIGHT,
                               brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}

pbio_error_t pup_color_sensor_light_on(pup_device_t *pdev) {
  return pup_color_sensor_light_set(pdev, 100, 100, 100);
}

pbio_error_t pup_color_sensor_light_off(pup_device_t *pdev) {
  return pup_color_sensor_light_set(pdev, 0, 0, 0);
}

pbio_color_hsv_t *pup_color_sensor_detectable_colors(int32_t size, pbio_color_hsv_t *colors){
	if(size > CB_COLOR_MAP_SIZE || size < 1){
		return color_map.colors;
	}

	color_map.size = size;
	for(size_t i = 0; i < size; i++){
		color_map.colors[i] = colors[i];
	}

	return NULL;
}
