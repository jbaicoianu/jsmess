/***************************************************************************

Namco 54XX

This custom chip is a Fujitsu MB8844 MCU programmed to act as a noise
generator. It is used for explosions, the shoot sound in Bosconian, and the
tire screech sound in Pole Position.

CMD = command from main CPU
OUTn = sound outputs (3 channels)

The chip reads the command when the /IRQ is pulled down.

      +------+
 EXTAL|1   28|Vcc
  XTAL|2   27|CMD7
/RESET|3   26|CMD6
OUT0.0|4   25|CMD5
OUT0.1|5   24|CMD4
OUT0.2|6   23|/IRQ
OUT0.3|7   22|n.c. [1]
OUT1.0|8   21|n.c. [1]
OUT1.1|9   20|OUT2.3
OUT1.2|10  19|OUT2.2
OUT1.3|11  18|OUT2.1
  CMD0|12  17|OUT2.0
  CMD1|13  16|CMD3
   GND|14  15|CMD2
      +------+


[1] The RNG that drives the type A output is output on pin 21, and the one that
drives the type B output is output on pin 22, but those pins are not connected
on the board.


The command format is very simple:

0x: nop
1x: play sound type A
2x: play sound type B
3x: set parameters (type A) (followed by 4 bytes)
4x: set parameters (type B) (followed by 4 bytes)
5x: play sound type C
6x: set parameters (type C) (followed by 5 bytes)
7x: set volume for sound type C to x
8x-Fx: nop

***************************************************************************/

#include "driver.h"
#include "namco54.h"
#include "cpu/mb88xx/mb88xx.h"

typedef struct _namco_54xx_state namco_54xx_state;
struct _namco_54xx_state
{
	const device_config *cpu;
	const device_config *discrete;
	int basenode;
	UINT8 latched_cmd;
};

INLINE namco_54xx_state *get_safe_token(const device_config *device)
{
	assert(device != NULL);
	assert(device->token != NULL);
	assert(device->type == NAMCO_54XX);

	return (namco_54xx_state *)device->token;
}



static TIMER_CALLBACK( namco_54xx_latch_callback )
{
	namco_54xx_state *state = get_safe_token((const device_config *)ptr);
	state->latched_cmd = param;
}

static READ8_HANDLER( namco_54xx_K_r )
{
	namco_54xx_state *state = get_safe_token(space->cpu->owner);
	return state->latched_cmd >> 4;
}

static READ8_HANDLER( namco_54xx_R0_r )
{
	namco_54xx_state *state = get_safe_token(space->cpu->owner);
	return state->latched_cmd & 0x0f;
}


static WRITE8_HANDLER( namco_54xx_O_w )
{
	namco_54xx_state *state = get_safe_token(space->cpu->owner);
	UINT8 out = (data & 0x0f);
	if (data & 0x10)
		discrete_sound_w(state->discrete, NAMCO_54XX_1_DATA(state->basenode), out);
	else
		discrete_sound_w(state->discrete, NAMCO_54XX_0_DATA(state->basenode), out);
}

static WRITE8_HANDLER( namco_54xx_R1_w )
{
	namco_54xx_state *state = get_safe_token(space->cpu->owner);
	UINT8 out = (data & 0x0f);

	discrete_sound_w(state->discrete, NAMCO_54XX_2_DATA(state->basenode), out);
}




static TIMER_CALLBACK( namco_54xx_irq_clear )
{
	namco_54xx_state *state = get_safe_token((const device_config *)ptr);
	cpu_set_input_line(state->cpu, 0, CLEAR_LINE);
}

WRITE8_DEVICE_HANDLER( namco_54xx_write )
{
	namco_54xx_state *state = get_safe_token(device);

	timer_call_after_resynch(device->machine, (void *)device, data, namco_54xx_latch_callback);

	cpu_set_input_line(state->cpu, 0, ASSERT_LINE);

	// The execution time of one instruction is ~4us, so we must make sure to
	// give the cpu time to poll the /IRQ input before we clear it.
	// The input clock to the 06XX interface chip is 64H, that is
	// 18432000/6/64 = 48kHz, so it makes sense for the irq line to be
	// asserted for one clock cycle ~= 21us.
	timer_set(device->machine, ATTOTIME_IN_USEC(21), (void *)device, 0, namco_54xx_irq_clear);
}


/***************************************************************************
    DEVICE INTERFACE
***************************************************************************/

ADDRESS_MAP_START( namco_54xx_map_io, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(MB88_PORTK,  MB88_PORTK)  AM_READ(namco_54xx_K_r)
	AM_RANGE(MB88_PORTO,  MB88_PORTO)  AM_WRITE(namco_54xx_O_w)
	AM_RANGE(MB88_PORTR0, MB88_PORTR0) AM_READ(namco_54xx_R0_r)
	AM_RANGE(MB88_PORTR1, MB88_PORTR1) AM_WRITE(namco_54xx_R1_w)
	AM_RANGE(MB88_PORTR2, MB88_PORTR2) AM_NOP
ADDRESS_MAP_END


static MACHINE_DRIVER_START( namco_54xx )
	MDRV_CPU_ADD("mcu", MB8844, DERIVED_CLOCK(1,1))		/* parent clock, internally divided by 6 */
	MDRV_CPU_IO_MAP(namco_54xx_map_io)
MACHINE_DRIVER_END


ROM_START( namco_54xx )
	ROM_REGION( 0x400, "mcu", ROMREGION_LOADBYNAME )
	ROM_LOAD( "54xx.bin",     0x0000, 0x0400, CRC(ee7357e0) SHA1(01bdf984a49e8d0cc8761b2cc162fd6434d5afbe) )
ROM_END


/*-------------------------------------------------
    device start callback
-------------------------------------------------*/

static DEVICE_START( namco_54xx )
{
	namco_54xx_config *config = (namco_54xx_config *)device->inline_config;
	namco_54xx_state *state = get_safe_token(device);
	astring *tempstring = astring_alloc();

	/* find our CPU */
	state->cpu = cputag_get_cpu(device->machine, device_build_tag(tempstring, device, "mcu"));
	assert(state->cpu != NULL);
	astring_free(tempstring);
	
	/* find the attached discrete sound device */
	assert(config->discrete != NULL);
	state->discrete = devtag_get_device(device->machine, config->discrete);
	assert(state->discrete != NULL);
	state->basenode = config->firstnode;
}


/*-------------------------------------------------
    device reset callback
-------------------------------------------------*/

static DEVICE_RESET( namco_54xx )
{
//	namco_54xx_state *state = get_safe_token(device);
}


/*-------------------------------------------------
    device get info callback
-------------------------------------------------*/

DEVICE_GET_INFO( namco_54xx )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:			info->i = sizeof(namco_54xx_state);				break;
		case DEVINFO_INT_INLINE_CONFIG_BYTES:	info->i = sizeof(namco_54xx_config);			break;
		case DEVINFO_INT_CLASS:					info->i = DEVICE_CLASS_PERIPHERAL;				break;

		/* --- the following bits of info are returned as pointers --- */
		case DEVINFO_PTR_ROM_REGION:			info->romregion = ROM_NAME(namco_54xx);			break;
		case DEVINFO_PTR_MACHINE_CONFIG:		info->machine_config = MACHINE_DRIVER_NAME(namco_54xx); break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:					info->start = DEVICE_START_NAME(namco_54xx); 	break;
		case DEVINFO_FCT_RESET:					info->reset = DEVICE_RESET_NAME(namco_54xx); 	break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:					strcpy(info->s, "Namco 54xx");					break;
		case DEVINFO_STR_FAMILY:				strcpy(info->s, "Namco I/O");					break;
		case DEVINFO_STR_VERSION:				strcpy(info->s, "1.0");							break;
		case DEVINFO_STR_SOURCE_FILE:			strcpy(info->s, __FILE__);						break;
		case DEVINFO_STR_CREDITS:				strcpy(info->s, "Copyright Nicola Salmoria and the MAME Team"); break;
	}
}
