/***************************************************************************

    Hubler/Everts

    12/05/2009 Skeleton driver.

****************************************************************************/

/*

	TODO:

	- keyboard
	- cassette
	- tone generator
	- eprom programmer

*/

#include "driver.h"
#include "includes/huebler.h"
#include "cpu/z80/z80.h"
#include "cpu/z80/z80daisy.h"
#include "devices/cassette.h"
#include "machine/z80pio.h"
#include "machine/z80sio.h"
#include "machine/z80ctc.h"
#include "devices/messram.h"

/* Keyboard */

static void keyboard_scan(running_machine *machine)
{
	huebler_state *state = machine->driver_data;
//	static const char *const keynames[] = { "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", "Y8", "Y9", "Y10", "Y11", "Y12", "Y13", "Y14", "Y15" };
//	UINT8 data = input_port_read(machine, keynames[state->keylatch]);

	state->keylatch++;

	if (state->keylatch == 16)
	{
		state->keylatch = 0;
	}
}

static TIMER_DEVICE_CALLBACK( keyboard_tick )
{
	keyboard_scan(timer->machine);
}

/* Read/Write Handlers */

static READ8_DEVICE_HANDLER( huebler_z80sio_r )
{
	switch (offset)
	{
	case 0: return z80sio_d_r(device, 0);
	case 1: return z80sio_c_r(device, 0);
	case 2: return z80sio_d_r(device, 1);
	case 3: return z80sio_c_r(device, 1);
	}

	return 0;
}

static WRITE8_DEVICE_HANDLER( huebler_z80sio_w )
{
	switch (offset)
	{
	case 0: z80sio_d_w(device, 0, data); break;
	case 1: z80sio_c_w(device, 0, data); break;
	case 2: z80sio_d_w(device, 1, data); break;
	case 3: z80sio_c_w(device, 1, data); break;
	}
}

static READ8_HANDLER( keyboard_data_r )
{
	huebler_state *state = space->machine->driver_data;

	return state->keydata;
}

static READ8_HANDLER( keyboard_status_r )
{
	huebler_state *state = space->machine->driver_data;

	return state->keystatus;
}

/* Memory Maps */

static ADDRESS_MAP_START( huebler_mem, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xe7ff) AM_RAM
	AM_RANGE(0xe800, 0xefff) AM_RAM AM_BASE_MEMBER(huebler_state, video_ram)
	AM_RANGE(0xf000, 0xfbff) AM_ROM
	AM_RANGE(0xfc00, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( huebler_io, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
//	AM_RANGE(0x00, 0x00) AM_WRITE(power_off_w)
//	AM_RANGE(0x04, 0x04) AM_WRITE(tone_off_w)
//	AM_RANGE(0x05, 0x05) AM_WRITE(tone_on_w)
	AM_RANGE(0x08, 0x08) AM_READ(keyboard_data_r)
	AM_RANGE(0x09, 0x09) AM_READ(keyboard_status_r)
	AM_RANGE(0x0c, 0x0f) AM_DEVREADWRITE(Z80PIO2_TAG, z80pio_alt_r, z80pio_alt_w)
	AM_RANGE(0x10, 0x13) AM_DEVREADWRITE(Z80PIO1_TAG, z80pio_alt_r, z80pio_alt_w)
	AM_RANGE(0x14, 0x17) AM_DEVREADWRITE(Z80CTC_TAG, z80ctc_r, z80ctc_w)
	AM_RANGE(0x18, 0x1b) AM_DEVREADWRITE(Z80SIO_TAG, huebler_z80sio_r, huebler_z80sio_w)
ADDRESS_MAP_END

/* Input Ports */

static INPUT_PORTS_START( huebler )
	PORT_START("Y0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) 

	PORT_START("Y9")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y10")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y11")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y12")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y13")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y14")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 

	PORT_START("Y15")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) 
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED ) 
INPUT_PORTS_END

/* Video */

static VIDEO_START( huebler )
{
	huebler_state *state = machine->driver_data;

	/* find memory regions */
	state->char_rom = memory_region(machine, "chargen");
}

static VIDEO_UPDATE( huebler )
{
	huebler_state *state = screen->machine->driver_data;

	int y, sx, x, line;

	for (y = 0; y < 240; y++)
	{
		line = y % 10;

		for (sx = 0; sx < 64; sx++)
		{
			UINT16 videoram_addr = ((y / 10) * 64) + sx;
			UINT8 videoram_data = state->video_ram[videoram_addr & 0x7ff];

			UINT16 charrom_addr = ((videoram_data & 0x7f) << 3) | line;
			UINT8 data = state->char_rom[charrom_addr & 0x3ff];

			for (x = 0; x < 6; x++)
			{
				int color = ((line > 7) ? 0 : BIT(data, 7)) ^ BIT(videoram_data, 7);

				*BITMAP_ADDR16(bitmap, y, (sx * 6) + x) = color;

				data <<= 1;
			}
		}
	}

    return 0;
}

/* Z80-CTC Interface */

static WRITE_LINE_DEVICE_HANDLER( ctc_z0_w )
{
}

static WRITE_LINE_DEVICE_HANDLER( ctc_z1_w )
{
}

static WRITE_LINE_DEVICE_HANDLER( ctc_z2_w )
{
}

static Z80CTC_INTERFACE( ctc_intf )
{
	0,              	/* timer disables */
	DEVCB_CPU_INPUT_LINE(Z80_TAG, INPUT_LINE_IRQ0),	/* interrupt handler */
	DEVCB_LINE(ctc_z0_w),	/* ZC/TO0 callback */
	DEVCB_LINE(ctc_z1_w),	/* ZC/TO1 callback */
	DEVCB_LINE(ctc_z2_w) 	/* ZC/TO2 callback */
};

/* Z80-PIO Interface */

static const z80pio_interface pio1_intf =
{
	DEVCB_CPU_INPUT_LINE(Z80_TAG, INPUT_LINE_IRQ0),	/* callback when change interrupt status */
	DEVCB_NULL,						/* port A read callback */
	DEVCB_NULL,						/* port B read callback */
	DEVCB_NULL,						/* port A write callback */
	DEVCB_NULL,						/* port B write callback */
	DEVCB_NULL,						/* portA ready active callback */
	DEVCB_NULL						/* portB ready active callback */
};

static const z80pio_interface pio2_intf =
{
	DEVCB_CPU_INPUT_LINE(Z80_TAG, INPUT_LINE_IRQ0),	/* callback when change interrupt status */
	DEVCB_NULL,						/* port A read callback */
	DEVCB_NULL,						/* port B read callback */
	DEVCB_NULL,						/* port A write callback */
	DEVCB_NULL,						/* port B write callback */
	DEVCB_NULL,						/* portA ready active callback */
	DEVCB_NULL						/* portB ready active callback */
};

/* Z80-SIO Interface */

static void z80daisy_interrupt(const device_config *device, int state)
{
	cputag_set_input_line(device->machine, Z80_TAG, INPUT_LINE_IRQ0, state);
}

static const z80sio_interface sio_intf =
{
	z80daisy_interrupt,	/* interrupt handler */
	NULL,				/* DTR changed handler */
	NULL,				/* RTS changed handler */
	NULL,				/* BREAK changed handler */
	NULL,				/* transmit handler */
	NULL				/* receive handler */
};

/* Z80 Daisy Chain */

static const z80_daisy_chain huebler_daisy_chain[] =
{
	{ Z80CTC_TAG },
	{ Z80SIO_TAG },
	{ Z80PIO1_TAG },
	{ Z80PIO2_TAG },
	{ NULL }
};

/* Machine Initialization */

static MACHINE_START( huebler )
{
	huebler_state *state = machine->driver_data;

	/* find devices */
	state->cassette = devtag_get_device(machine, CASSETTE_TAG);

	/* find memory regions */
	state->keyboard_rom = memory_region(machine, "keyboard");
}

/* Machine Driver */

static const cassette_config huebler_cassette_config =
{
	cassette_default_formats,
	NULL,
	CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_MUTED
};

static MACHINE_DRIVER_START( huebler )
	MDRV_DRIVER_DATA(huebler_state)

	/* basic machine hardware */
    MDRV_CPU_ADD(Z80_TAG, Z80, XTAL_10MHz/4) /* U880D */
    MDRV_CPU_PROGRAM_MAP(huebler_mem)
    MDRV_CPU_IO_MAP(huebler_io)
	MDRV_CPU_CONFIG(huebler_daisy_chain)

    MDRV_MACHINE_START(huebler)

	MDRV_TIMER_ADD_PERIODIC("keyboard", keyboard_tick, HZ(1500))

    /* video hardware */
	MDRV_SCREEN_ADD(SCREEN_TAG, RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(9000000, 576, 0*6, 64*6, 320, 0*10, 24*10)

    MDRV_PALETTE_LENGTH(2)
    MDRV_PALETTE_INIT(black_and_white)

    MDRV_VIDEO_START(huebler)
    MDRV_VIDEO_UPDATE(huebler)

	/* devices */
	MDRV_Z80CTC_ADD(Z80CTC_TAG, XTAL_10MHz/4, ctc_intf)
	MDRV_Z80PIO_ADD(Z80PIO1_TAG, pio1_intf)
	MDRV_Z80PIO_ADD(Z80PIO2_TAG, pio2_intf)
	MDRV_Z80SIO_ADD(Z80SIO_TAG, XTAL_10MHz/4, sio_intf)

	MDRV_CASSETTE_ADD(CASSETTE_TAG, huebler_cassette_config)
	
	/* internal ram */
	MDRV_RAM_ADD("messram")
	MDRV_RAM_DEFAULT_SIZE("59K")	
MACHINE_DRIVER_END

/* ROMs */

ROM_START( huebler )
	ROM_REGION( 0x10000, Z80_TAG, 0 )
	ROM_DEFAULT_BIOS( "v21" )
	ROM_SYSTEM_BIOS( 0, "v21", "H.MON v2.1" )
	ROMX_LOAD( "mon21.bin", 0xf000, 0x0bdf, BAD_DUMP CRC(ba905563) SHA1(1fa0aeab5428731756bdfa74efa3c664898bf083), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v30", "H.MON v3.0" )
	ROMX_LOAD( "mon30.bin", 0xf000, 0x1000, CRC(033f8112) SHA1(0c6ae7b9d310dec093652db6e8ae84f8ebfdcd29), ROM_BIOS(2) )

	ROM_REGION( 0x4800, "hbasic", 0 )
	ROM_LOAD( "mon30p_hbasic33p.bin", 0x0000, 0x4800, CRC(c927e7be) SHA1(2d1f3ff4d882c40438a1281872c6037b2f07fdf2) )

	ROM_REGION( 0x400, "chargen", 0 )
	ROM_LOAD( "hemcfont.bin", 0x0000, 0x0400, CRC(1074d103) SHA1(e558279cff5744acef4eccf30759a9508b7f8750) )

	ROM_REGION( 0x400, "keyboard", 0 )
	ROM_LOAD( "keyboard.bin", 0x0000, 0x0400, BAD_DUMP CRC(daa06361) SHA1(b0299bd8d1686e05dbeeaed54f6c41ae543be20c) ) // typed in from manual
ROM_END

/* System Drivers */

/*    YEAR  NAME        PARENT  COMPAT  MACHINE     INPUT       INIT    CONFIG      COMPANY                             FULLNAME            FLAGS */
COMP( 1985, huebler,	0,		0,		huebler,	huebler,	0,		0,			"Bernd Hubler, Klaus-Peter Evert",	"Hubler/Everts",	GAME_NOT_WORKING )
