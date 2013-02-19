/*
** mrb_sysfsgpio - Sysfs GPIO interface
**
** Copyright (c) EmAF project 2013-
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <stdio.h>
#include <string.h>

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/class.h"
#include "mruby/data.h"

#define GPIO_EXPORT	"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define GPIO_DIRECTION	"/sys/class/gpio/gpio%d/direction"
#define GPIO_DIRECTION_HIGH	"high"
#define GPIO_DIRECTION_LOW	"low"
#define GPIO_DIRECTION_IN	"in"
#define GPIO_DIRECTION_OUT	"out"
#define GPIO_VALUE	"/sys/class/gpio/gpio%d/value"
#define GPIO_VALUE_H	'1'
#define GPIO_VALUE_L	'0'

#define PATHMAX	(256)

enum direction {
	HIGH,
	LOW,
	OUT,
	IN
};


struct mrb_gpio {
	unsigned int	gpio_no;
	enum direction	gpio_direction;
};

void
mrb_c_gpio_unexport(mrb_int gpio_no)
{
	FILE* fp;

	fp = fopen(GPIO_UNEXPORT,"ab");
	rewind(fp);
	fprintf(fp,"%d",gpio_no);
	fclose(fp);
}

static void
mrb_gpio_free(mrb_state* mrb, void* ptr)
{
	mrb_free(mrb, ptr);
}

static struct mrb_data_type mrb_gpio_type = { "GPIO", mrb_gpio_free };

/*
static mrb_value
mrb_gpio_wrap(mrb_state* mrb, struct RClass* tc, struct mrb_gpio* gpio)
{
	return mrb_obj_value(Data_Wrap_Struct(mrb, tc, &mrb_gpio_type, gpio));
}
*/

void
mrb_c_gpio_export(struct mrb_gpio* gpio)
{
	FILE* fp;

	fp = fopen(GPIO_EXPORT,"ab");
	if ( !fp ) {
        	printf("open error\n");
	}
	rewind(fp);
	fprintf(fp,"%d",gpio->gpio_no);
	fclose(fp);

}

void
mrb_c_gpio_direction(struct mrb_gpio* gpio)
{
	FILE* fp;
	char path[PATHMAX];
	sprintf(path,GPIO_DIRECTION,gpio->gpio_no);
	fp = fopen(path,"rb+");
	if ( !fp ) {
        	printf("open error\n");
	}
	rewind(fp);
	switch(gpio->gpio_direction) {
		case HIGH:
			fprintf(fp,GPIO_DIRECTION_HIGH);
			break;
		case LOW:
			fprintf(fp,GPIO_DIRECTION_LOW);
			break;
		case OUT:
			fprintf(fp,GPIO_DIRECTION_OUT);
			break;
		case IN:
			fprintf(fp,GPIO_DIRECTION_IN);
			break;
	}
	fclose(fp);
}

static struct mrb_gpio*
mrb_c_gpio_alloc(mrb_state* mrb)
{
        mrb_int gpio_no;
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_malloc(mrb, sizeof(struct mrb_gpio));
	if ( !gpio )
		return NULL;
        mrb_get_args(mrb, "i", &gpio_no);

	gpio->gpio_no = gpio_no;
	gpio->gpio_direction = IN;

	return gpio;
}

mrb_value mrb_c_gpio_initialize(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = mrb_c_gpio_alloc(mrb);
	DATA_PTR(self) = gpio;
	DATA_TYPE(self) = &mrb_gpio_type;
	return self;
}

mrb_value mrb_c_gpio_attach(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	mrb_c_gpio_export(gpio);
	mrb_c_gpio_direction(gpio);

	return self;
}

mrb_value mrb_c_gpio_detach(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	mrb_c_gpio_unexport(gpio->gpio_no);

	return self;
}

mrb_value mrb_c_gpio_direction_high(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	gpio->gpio_direction = HIGH;
	return self;
}

mrb_value mrb_c_gpio_direction_low(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	gpio->gpio_direction = LOW;
	return self;
}

mrb_value mrb_c_gpio_direction_out(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	gpio->gpio_direction = OUT;
	return self;
}

mrb_value mrb_c_gpio_direction_in(mrb_state* mrb, mrb_value self)
{
	struct mrb_gpio* gpio;

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	gpio->gpio_direction = IN;
	return self;
}

mrb_value mrb_c_gpio_read(mrb_state* mrb, mrb_value self)
{
	FILE* fp;
	struct mrb_gpio* gpio;
	char path[PATHMAX];
	char buf[2];

	gpio = (struct mrb_gpio *)mrb_get_datatype(mrb, self, &mrb_gpio_type);
	if ( !gpio ) {
		return mrb_nil_value();
	}
	if ( gpio->gpio_direction != IN ) {
		return mrb_nil_value();
	}

	sprintf(path,GPIO_VALUE,gpio->gpio_no);
	fp = fopen(path,"r+");
	rewind(fp);
	fgets(buf, 2, fp);
	fclose(fp);

	if ( buf[0] == GPIO_VALUE_L ) {
		return mrb_fixnum_value(0);
	}
	else if ( buf[0] == GPIO_VALUE_H ) {
		return mrb_fixnum_value(1);
	}
	return mrb_nil_value();
}


void
mrb_mruby_sysfsgpio_gem_init(mrb_state* mrb)
{
        struct RClass *tc;

        /* define class */
        tc = mrb_define_class(mrb,"GPIO",mrb->object_class);
	MRB_SET_INSTANCE_TT(tc, MRB_TT_DATA);

        mrb_define_method(mrb, tc, "initialize", mrb_c_gpio_initialize, ARGS_REQ(1));
        mrb_define_method(mrb, tc, "attach", mrb_c_gpio_attach, ARGS_NONE());
        mrb_define_method(mrb, tc, "detach", mrb_c_gpio_detach, ARGS_NONE());
        mrb_define_method(mrb, tc, "direction_high", mrb_c_gpio_direction_high, ARGS_NONE());
        mrb_define_method(mrb, tc, "direction_low", mrb_c_gpio_direction_low, ARGS_NONE());
        mrb_define_method(mrb, tc, "direction_out", mrb_c_gpio_direction_out, ARGS_NONE());
        mrb_define_method(mrb, tc, "direction_in", mrb_c_gpio_direction_in, ARGS_NONE());
        mrb_define_method(mrb, tc, "read", mrb_c_gpio_read, ARGS_NONE());
}

void
mrb_mruby_sysfsgpio_gem_final(mrb_state* mrb)
{
}
