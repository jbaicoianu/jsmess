/***************************************************************************

    Babbage-2nd skeleton driver (19/OCT/2011)

    http://homepage3.nifty.com/takeda-toshiya/babbage/index.html

    ToDo:
    - Keyboard not working - uses IM2 and it isn't causing an IRQ yet.
    - Move the 8 LEDs to be under the numbers

***************************************************************************/
#define ADDRESS_MAP_MODERN

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/z80ctc.h"
#include "machine/z80pio.h"
#include "cpu/z80/z80daisy.h"
#include "babbage.lh"

#define MACHINE_RESET_MEMBER(name) void name::machine_reset()
#define MACHINE_START_MEMBER(name) void name::machine_start()
#define MAIN_CLOCK 25e5

class babbage_state : public driver_device
{
public:
	babbage_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_pio_1(*this, "z80pio1"),
	m_pio_2(*this, "z80pio2"),
	m_ctc(*this, "z80ctc")
	{ }

	required_device<cpu_device> m_maincpu;
	required_device<device_t> m_pio_1;
	required_device<device_t> m_pio_2;
	required_device<device_t> m_ctc;
	DECLARE_READ8_MEMBER( pio2_a_r );
	DECLARE_WRITE8_MEMBER( pio1_b_w );
	DECLARE_WRITE8_MEMBER( pio2_b_w );
	DECLARE_WRITE_LINE_MEMBER( ctc_z0_w );
	DECLARE_WRITE_LINE_MEMBER( ctc_z1_w );
	DECLARE_WRITE_LINE_MEMBER( ctc_z2_w );
	UINT8 m_segment;
	UINT8 m_digit;
	UINT8 m_key;
	bool m_step;
	virtual void machine_reset();
	virtual void machine_start();
};


/***************************************************************************

    Keyboard

***************************************************************************/




/***************************************************************************

    Machine

***************************************************************************/

MACHINE_START_MEMBER( babbage_state )
{
}

MACHINE_RESET_MEMBER( babbage_state )
{
}



/***************************************************************************

    Address Map

***************************************************************************/

static ADDRESS_MAP_START( babbage_map, AS_PROGRAM, 8, babbage_state )
	ADDRESS_MAP_GLOBAL_MASK(0x3fff)
	AM_RANGE(0x0000, 0x07ff) AM_ROM
	AM_RANGE(0x1000, 0x17ff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( babbage_io, AS_IO, 8, babbage_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x03) AM_DEVREADWRITE_LEGACY("z80ctc", z80ctc_r, z80ctc_w)
	AM_RANGE(0x10, 0x13) AM_DEVREADWRITE_LEGACY("z80pio1", z80pio_ba_cd_r, z80pio_ba_cd_w)
	AM_RANGE(0x20, 0x23) AM_DEVREADWRITE_LEGACY("z80pio2", z80pio_ba_cd_r, z80pio_ba_cd_w)
ADDRESS_MAP_END



/**************************************************************************

    Keyboard Layout

***************************************************************************/

// no idea of the actual key matrix
static INPUT_PORTS_START( babbage )
	PORT_START("X0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INC") PORT_CODE(KEYCODE_EQUALS)

	PORT_START("X1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DA") PORT_CODE(KEYCODE_MINUS)

	PORT_START("X2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("AD") PORT_CODE(KEYCODE_J)

	PORT_START("X3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("GO") PORT_CODE(KEYCODE_ENTER)
INPUT_PORTS_END

/* Z80-CTC Interface */

WRITE_LINE_MEMBER( babbage_state::ctc_z0_w )
{
}

WRITE_LINE_MEMBER( babbage_state::ctc_z1_w )
{
}

WRITE_LINE_MEMBER( babbage_state::ctc_z2_w )
{
}

static Z80CTC_INTERFACE( babbage_ctc_intf )
{
	0,              	/* timer disables */
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_IRQ0),	/* interrupt handler */
	DEVCB_DRIVER_LINE_MEMBER(babbage_state, ctc_z0_w),	/* ZC/TO0 callback */
	DEVCB_DRIVER_LINE_MEMBER(babbage_state, ctc_z1_w),	/* ZC/TO1 callback */
	DEVCB_DRIVER_LINE_MEMBER(babbage_state, ctc_z2_w)	/* ZC/TO2 callback */
};

/* Z80-PIO Interface */

// The 8 LEDs
// no idea what order they are in
// bios never writes here
WRITE8_MEMBER( babbage_state::pio1_b_w )
{
	char ledname[8];
	for (int i = 0; i < 8; i++)
	{
		sprintf(ledname,"led%d",i);
		output_set_value(ledname, BIT(data, i));
	}
}

static Z80PIO_INTERFACE( babbage_z80pio1_intf )
{
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_IRQ0),	/* int callback */
	DEVCB_NULL,	/* port a read */
	DEVCB_NULL,	/* port a write */
	DEVCB_NULL,	/* ready a */
	DEVCB_NULL,	/* port b read */
	DEVCB_DRIVER_MEMBER(babbage_state, pio1_b_w),	/* port b write */
	DEVCB_NULL	/* ready b */
};

READ8_MEMBER( babbage_state::pio2_a_r)
{
	cputag_set_input_line(machine(), "maincpu", 0, CLEAR_LINE);
	return m_key;
}

WRITE8_MEMBER( babbage_state::pio2_b_w)
{
	if (BIT(data, 7))
		m_step = 0;
	else
	if (!m_step)
	{
		m_segment = data;
		m_step++;
	}
	else
	{
		m_digit = data;
		output_set_digit_value(m_digit, m_segment);
	}
}

static Z80PIO_INTERFACE( babbage_z80pio2_intf )
{
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_IRQ0),	/* int callback */
	DEVCB_DRIVER_MEMBER(babbage_state, pio2_a_r),	/* port a read */
	DEVCB_NULL,	/* port a write */
	DEVCB_NULL,	/* ready a */
	DEVCB_NULL,	/* port b read */
	DEVCB_DRIVER_MEMBER(babbage_state, pio2_b_w),	/* port b write */
	DEVCB_NULL	/* ready b */
};

static const z80_daisy_config babbage_daisy_chain[] =
{// need to check the order
	{ "z80pio1" },
	{ "z80pio2" },
	{ "z80ctc" },
	{ NULL }
};

static TIMER_DEVICE_CALLBACK( keyboard_callback )
{
	babbage_state *state = timer.machine().driver_data<babbage_state>();

	UINT8 i, j, inp;
	char kbdrow[6];
	state->m_key = 0xff;

	for (i = 0; i < 4; i++)
	{
		sprintf(kbdrow,"X%X",i);
		inp = input_port_read(timer.machine(), kbdrow);

		for (j = 0; j < 5; j++)
			if (BIT(inp, j))
				state->m_key = (j << 2) | i;
	}

	if (state->m_key < 0xff)
		//cputag_set_input_line(timer.machine(), "maincpu", 0, HOLD_LINE);
		z80pio_pb_w(state->m_pio_2, 1, state->m_key); // initiate an interrupt
}


/***************************************************************************

    Machine driver

***************************************************************************/

static MACHINE_CONFIG_START( babbage, babbage_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, MAIN_CLOCK) //2.5MHz
	MCFG_CPU_PROGRAM_MAP(babbage_map)
	MCFG_CPU_IO_MAP(babbage_io)
	MCFG_CPU_CONFIG(babbage_daisy_chain)

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_babbage)

	/* Devices */
	MCFG_Z80CTC_ADD( "z80ctc", MAIN_CLOCK, babbage_ctc_intf)
	MCFG_Z80PIO_ADD( "z80pio1", MAIN_CLOCK, babbage_z80pio1_intf )
	MCFG_Z80PIO_ADD( "z80pio2", MAIN_CLOCK, babbage_z80pio2_intf )

	MCFG_TIMER_ADD_PERIODIC("keyboard_timer", keyboard_callback, attotime::from_hz(25))
MACHINE_CONFIG_END


/***************************************************************************

    Game driver

***************************************************************************/

ROM_START(babbage)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("mon.rom",    0x0000, 0x0200, CRC(469bd607) SHA1(8f3489a0f96de6a03b05c1ee01b89d9848f4b152) )
ROM_END


/*    YEAR  NAME      PARENT  COMPAT  MACHINE     INPUT    INIT       COMPANY           FULLNAME */
COMP( 1986, babbage,  0,      0,      babbage,    babbage, 0,   "Mr Takafumi Aihara",  "Babbage-2nd" , GAME_NOT_WORKING | GAME_NO_SOUND_HW )
