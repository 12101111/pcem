#include <stdlib.h>
#include "ibm.h"
#include "device.h"
#include "io.h"
#include "joystick.h"
#include "timer.h"
#include "x86.h"

#include "gameport.h"
#include "joystick_ch_flightstick_pro.h"
#include "joystick_standard.h"
#include "joystick_sw_pad.h"
#include "joystick_tm_fcs.h"

int joystick_type;

static joystick_if_t *joystick_list[] =
{
        &joystick_standard,
        &joystick_standard_4button,
        &joystick_standard_6button,
        &joystick_standard_8button,
        &joystick_ch_flightstick_pro,
        &joystick_sw_pad,
        &joystick_tm_fcs,
        NULL
};

char *joystick_get_name(int joystick)
{
        if (!joystick_list[joystick])
                return NULL;
        return joystick_list[joystick]->name;
}

int joystick_get_max_joysticks(int joystick)
{
        return joystick_list[joystick]->max_joysticks;
}
        
int joystick_get_axis_count(int joystick)
{
        return joystick_list[joystick]->axis_count;
}

int joystick_get_button_count(int joystick)
{
        return joystick_list[joystick]->button_count;
}

int joystick_get_pov_count(int joystick)
{
        return joystick_list[joystick]->pov_count;
}

char *joystick_get_axis_name(int joystick, int id)
{
        return joystick_list[joystick]->axis_names[id];
}

char *joystick_get_button_name(int joystick, int id)
{
        return joystick_list[joystick]->button_names[id];
}

char *joystick_get_pov_name(int joystick, int id)
{
        return joystick_list[joystick]->pov_names[id];
}

typedef struct gameport_axis_t
{
        pc_timer_t timer;
        int axis_nr;
        struct gameport_t *gameport;
} gameport_axis_t;
        
typedef struct gameport_t
{
        uint8_t state;
        
        gameport_axis_t axis[4];
        
        joystick_if_t *joystick;
        void *joystick_dat;
} gameport_t;

static gameport_t *gameport_global = NULL;

static void gameport_time(gameport_t *gameport, int nr, int axis)
{
        if (axis == AXIS_NOT_PRESENT)
        {
                timer_disable(&gameport->axis[nr].timer);
        }
        else
        {
                axis += 32768;
                axis = (axis * 100) / 65; /*Axis now in ohms*/
                axis = (axis * 11) / 1000;
                timer_set_delay_u64(&gameport->axis[nr].timer, TIMER_USEC * (axis + 24)); /*max = 11.115 ms*/
        }
}

void gameport_write(uint16_t addr, uint8_t val, void *p)
{
        gameport_t *gameport = (gameport_t *)p;

        gameport->state |= 0x0f;
//        pclog("gameport_write : joysticks_present=%i\n", joysticks_present);

        gameport_time(gameport, 0, gameport->joystick->read_axis(gameport->joystick_dat, 0));
        gameport_time(gameport, 1, gameport->joystick->read_axis(gameport->joystick_dat, 1));
        gameport_time(gameport, 2, gameport->joystick->read_axis(gameport->joystick_dat, 2));
        gameport_time(gameport, 3, gameport->joystick->read_axis(gameport->joystick_dat, 3));

        gameport->joystick->write(gameport->joystick_dat);
        
        cycles -= ISA_CYCLES(8);
}

uint8_t gameport_read(uint16_t addr, void *p)
{
        gameport_t *gameport = (gameport_t *)p;
        uint8_t ret;

//        if (joysticks_present)
                ret = gameport->state | gameport->joystick->read(gameport->joystick_dat);//0xf0;
//        else
//                ret = 0xff;

//        pclog("gameport_read: ret=%02x %08x:%08x isa_cycles=%i  %i\n", ret, cs, cpu_state.pc, isa_cycles, gameport->axis[0].count);

        cycles -= ISA_CYCLES(8);

        return ret;
}

void gameport_timer_over(void *p)
{
        gameport_axis_t *axis = (gameport_axis_t *)p;
        gameport_t *gameport = axis->gameport;
        
        gameport->state &= ~(1 << axis->axis_nr);
        
        if (axis == &gameport->axis[0])
                gameport->joystick->a0_over(gameport->joystick_dat);
}

void *gameport_init_common()
{
        gameport_t *gameport = malloc(sizeof(gameport_t));
        
        memset(gameport, 0, sizeof(gameport_t));
        
        gameport->axis[0].gameport = gameport;
        gameport->axis[1].gameport = gameport;
        gameport->axis[2].gameport = gameport;
        gameport->axis[3].gameport = gameport;

        gameport->axis[0].axis_nr = 0;
        gameport->axis[1].axis_nr = 1;
        gameport->axis[2].axis_nr = 2;
        gameport->axis[3].axis_nr = 3;
        
        timer_add(&gameport->axis[0].timer, gameport_timer_over, &gameport->axis[0], 0);
        timer_add(&gameport->axis[1].timer, gameport_timer_over, &gameport->axis[1], 0);
        timer_add(&gameport->axis[2].timer, gameport_timer_over, &gameport->axis[2], 0);
        timer_add(&gameport->axis[3].timer, gameport_timer_over, &gameport->axis[3], 0);
  
        gameport->joystick = joystick_list[joystick_type];            
        gameport->joystick_dat = gameport->joystick->init();
        
        gameport_global = gameport;
        
        return gameport;
}

void gameport_update_joystick_type()
{
        gameport_t *gameport = gameport_global;
        
        if (gameport)
        {
                gameport->joystick->close(gameport->joystick_dat);        
                gameport->joystick = joystick_list[joystick_type];
                gameport->joystick_dat = gameport->joystick->init();
        }
}

void *gameport_init()
{
        gameport_t *gameport = gameport_init_common();

        io_sethandler(0x0200, 0x0008, gameport_read, NULL, NULL, gameport_write, NULL, NULL, gameport);

        return gameport;
}

void *gameport_201_init()
{
        gameport_t *gameport = gameport_init_common();

        io_sethandler(0x0201, 0x0001, gameport_read, NULL, NULL, gameport_write, NULL, NULL, gameport);

        return gameport;
}

void gameport_close(void *p)
{
        gameport_t *gameport = (gameport_t *)p;
        
        gameport->joystick->close(gameport->joystick_dat);

        gameport_global = NULL;

        free(gameport);
}

device_t gameport_device =
{
        "Game port",
        0,
        gameport_init,
        gameport_close,
        NULL,
        NULL,
        NULL,
        NULL
};

device_t gameport_201_device =
{
        "Game port (port 201h only)",
        0,
        gameport_201_init,
        gameport_close,
        NULL,
        NULL,
        NULL,
        NULL
};
