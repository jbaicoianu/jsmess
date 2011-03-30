/***************************************************************************

  cgenie.c

  Functions to emulate the video controller 6845.

***************************************************************************/

#include "emu.h"
#include "includes/cgenie.h"





/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/
VIDEO_START( cgenie )
{
	cgenie_state *state = machine.driver_data<cgenie_state>();
	screen_device *screen = machine.first_screen();
	int width = screen->width();
	int height = screen->height();

	VIDEO_START_CALL(generic_bitmapped);

	state->dlybitmap = auto_bitmap_alloc(machine, width, height, BITMAP_FORMAT_INDEXED16);
}

/***************************************************************************

  Calculate the horizontal and vertical offset for the
  current register settings of the 6845 CRTC

***************************************************************************/
static void cgenie_offset_xy(cgenie_state *state)
{
	if( state->crt.horizontal_sync_pos )
		state->off_x = state->crt.horizontal_total - state->crt.horizontal_sync_pos - 14;
	else
		state->off_x = -15;

	state->off_y = (state->crt.vertical_total - state->crt.vertical_sync_pos) *
		(state->crt.scan_lines + 1) + state->crt.vertical_adjust
		- 32;

	if( state->off_y < 0 )
		state->off_y = 0;

	if( state->off_y > 128 )
		state->off_y = 128;

// logerror("cgenie offset x:%d  y:%d\n", state->off_x, state->off_y);
}


/***************************************************************************
  Write to an indexed register of the 6845 CRTC
***************************************************************************/
WRITE8_HANDLER ( cgenie_register_w )
{
	cgenie_state *state = space->machine().driver_data<cgenie_state>();
	//int addr;

	switch (state->crt.idx)
	{
		case 0:
			if( state->crt.horizontal_total == data )
				break;
			state->crt.horizontal_total = data;
			cgenie_offset_xy(state);
			break;
		case 1:
			if( state->crt.horizontal_displayed == data )
				break;
			state->crt.horizontal_displayed = data;
			break;
		case 2:
			if( state->crt.horizontal_sync_pos == data )
				break;
			state->crt.horizontal_sync_pos = data;
			cgenie_offset_xy(state);
			break;
		case 3:
			state->crt.horizontal_length = data;
			break;
		case 4:
			if( state->crt.vertical_total == data )
				break;
			state->crt.vertical_total = data;
			cgenie_offset_xy(state);
			break;
		case 5:
			if( state->crt.vertical_adjust == data )
				break;
			state->crt.vertical_adjust = data;
			cgenie_offset_xy(state);
			break;
		case 6:
			if( state->crt.vertical_displayed == data )
				break;
			state->crt.vertical_displayed = data;
			break;
		case 7:
			if( state->crt.vertical_sync_pos == data )
				break;
			state->crt.vertical_sync_pos = data;
			cgenie_offset_xy(state);
			break;
		case 8:
			state->crt.crt_mode = data;
			break;
		case 9:
			data &= 15;
			if( state->crt.scan_lines == data )
				break;
			state->crt.scan_lines = data;
			cgenie_offset_xy(state);
			break;
		case 10:
			if( state->crt.cursor_top == data )
				break;
			state->crt.cursor_top = data;
			//addr = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;
            break;
		case 11:
			if( state->crt.cursor_bottom == data )
				break;
			state->crt.cursor_bottom = data;
			//addr = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;
            break;
		case 12:
			data &= 63;
			if( state->crt.screen_address_hi == data )
				break;
			state->crt.screen_address_hi = data;
			break;
		case 13:
			if( state->crt.screen_address_lo == data )
				break;
			state->crt.screen_address_lo = data;
			break;
		case 14:
			data &= 63;
			if( state->crt.cursor_address_hi == data )
				break;
			state->crt.cursor_address_hi = data;
			//addr = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;
            break;
		case 15:
			if( state->crt.cursor_address_lo == data )
				break;
			state->crt.cursor_address_lo = data;
			//addr = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;
            break;
	}
}

/***************************************************************************
  Write to the index register of the 6845 CRTC
***************************************************************************/
WRITE8_HANDLER ( cgenie_index_w )
{
	cgenie_state *state = space->machine().driver_data<cgenie_state>();
	state->crt.idx = data & 15;
}

/***************************************************************************
  Read from an indexed register of the 6845 CRTC
***************************************************************************/
 READ8_HANDLER ( cgenie_register_r )
{
	cgenie_state *state = space->machine().driver_data<cgenie_state>();
	return cgenie_get_register(space->machine(), state->crt.idx);
}

/***************************************************************************
  Read from a register of the 6845 CRTC
***************************************************************************/
int cgenie_get_register(running_machine &machine, int indx)
{
	cgenie_state *state = machine.driver_data<cgenie_state>();
	switch (indx)
	{
		case 0:
			return state->crt.horizontal_total;
		case 1:
			return state->crt.horizontal_displayed;
		case 2:
			return state->crt.horizontal_sync_pos;
		case 3:
			return state->crt.horizontal_length;
		case 4:
			return state->crt.vertical_total;
		case 5:
			return state->crt.vertical_adjust;
		case 6:
			return state->crt.vertical_displayed;
		case 7:
			return state->crt.vertical_sync_pos;
		case 8:
			return state->crt.crt_mode;
		case 9:
			return state->crt.scan_lines;
		case 10:
			return state->crt.cursor_top;
		case 11:
			return state->crt.cursor_bottom;
		case 12:
			return state->crt.screen_address_hi;
		case 13:
			return state->crt.screen_address_lo;
		case 14:
			return state->crt.cursor_address_hi;
		case 15:
			return state->crt.cursor_address_lo;
	}
	return 0;
}

/***************************************************************************
  Read the index register of the 6845 CRTC
***************************************************************************/
 READ8_HANDLER ( cgenie_index_r )
{
	cgenie_state *state = space->machine().driver_data<cgenie_state>();
	return state->crt.idx;
}

/***************************************************************************
  Switch mode between character generator and graphics
***************************************************************************/
void cgenie_mode_select(running_machine &machine, int mode)
{
	cgenie_state *state = machine.driver_data<cgenie_state>();
	state->graphics = (mode) ? 1 : 0;
}


static void cgenie_refresh_monitor(running_machine &machine, bitmap_t * bitmap, const rectangle *cliprect)
{
	cgenie_state *state = machine.driver_data<cgenie_state>();
	UINT8 *videoram = state->videoram;
	int i, address, offset, cursor, size, code, x, y;
    rectangle r;

	bitmap_fill(bitmap, cliprect, get_black_pen(machine));

	if(state->crt.vertical_displayed || state->crt.horizontal_displayed)
	{
		offset = 256 * state->crt.screen_address_hi + state->crt.screen_address_lo;
		size = state->crt.horizontal_displayed * state->crt.vertical_displayed;
		cursor = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;

		/*
         * for every character in the Video RAM, check if it has been modified since
         * last time and update it accordingly.
         */
		for( address = 0; address < size; address++ )
		{
			i = (offset + address) & 0x3fff;
			x = address % state->crt.horizontal_displayed + state->off_x;
			y = address / state->crt.horizontal_displayed;

			r.min_x = x * 8;
			r.max_x = r.min_x + 7;
			r.min_y = y * (state->crt.scan_lines + 1) + state->off_y;
			r.max_y = r.min_y + state->crt.scan_lines;

			if( state->graphics )
			{
				/* get graphics code */
				code = videoram[i];
				drawgfx_opaque(bitmap, &r, machine.gfx[1], code, 0,
					0, 0, r.min_x, r.min_y);
			}
			else
			{
				/* get character code */
				code = videoram[i];

				/* translate defined character sets */
				code += state->font_offset[(code >> 6) & 3];
				drawgfx_opaque(bitmap, &r, machine.gfx[0], code, state->colorram[i&0x3ff],
					0, 0, r.min_x, r.min_y);
			}

			if( i == cursor )
			{
			rectangle rc;

			/* check if cursor turned off */
				if( (state->crt.cursor_top & 0x60) == 0x20 )
					continue;

				if( (state->crt.cursor_top & 0x60) == 0x60 )
				{
					state->crt.cursor_visible = 1;
				}
				else
				{
					state->crt.cursor_phase++;
					state->crt.cursor_visible = (state->crt.cursor_phase >> 3) & 1;
				}

				if( !state->crt.cursor_visible )
					continue;

				rc.min_x = r.min_x;
				rc.max_x = r.max_x;
				rc.min_y = r.min_y + (state->crt.cursor_top & 15);
				rc.max_y = r.min_y + (state->crt.cursor_bottom & 15);
				drawgfx_opaque(bitmap, &rc, machine.gfx[0], 0x7f, state->colorram[i&0x3ff],
					0, 0, rc.min_x, rc.min_y);
			}
		}
	}
}

static void cgenie_refresh_tv_set(running_machine &machine, bitmap_t * bitmap, const rectangle *cliprect)
{
	cgenie_state *state = machine.driver_data<cgenie_state>();
	UINT8 *videoram = state->videoram;
	int i, address, offset, cursor, size, code, x, y;
	rectangle r;

	bitmap_fill(machine.generic.tmpbitmap, cliprect, get_black_pen(machine));
	bitmap_fill(state->dlybitmap, cliprect, get_black_pen(machine));

	if(state->crt.vertical_displayed || state->crt.horizontal_displayed)
	{
		offset = 256 * state->crt.screen_address_hi + state->crt.screen_address_lo;
		size = state->crt.horizontal_displayed * state->crt.vertical_displayed;
		cursor = 256 * state->crt.cursor_address_hi + state->crt.cursor_address_lo;

		/*
         * for every character in the Video RAM, check if it has been modified since
         * last time and update it accordingly.
         */
		for( address = 0; address < size; address++ )
		{
			i = (offset + address) & 0x3fff;
			x = address % state->crt.horizontal_displayed + state->off_x;
			y = address / state->crt.horizontal_displayed;

			r.min_x = x * 8;
			r.max_x = r.min_x + 7;
			r.min_y = y * (state->crt.scan_lines + 1) + state->off_y;
			r.max_y = r.min_y + state->crt.scan_lines;

			if( state->graphics )
			{
				/* get graphics code */
				code = videoram[i];
				drawgfx_opaque(machine.generic.tmpbitmap, &r, machine.gfx[1], code, 1,
					0, 0, r.min_x, r.min_y);
				drawgfx_opaque(state->dlybitmap, &r, machine.gfx[1], code, 2,
					0, 0, r.min_x, r.min_y);
			}
			else
			{
				/* get character code */
				code = videoram[i];

				/* translate defined character sets */
				code += state->font_offset[(code >> 6) & 3];
				drawgfx_opaque(machine.generic.tmpbitmap, &r, machine.gfx[0], code, state->colorram[i&0x3ff] + 16,
					0, 0, r.min_x, r.min_y);
				drawgfx_opaque(state->dlybitmap, &r, machine.gfx[0], code, state->colorram[i&0x3ff] + 32,
					0, 0, r.min_x, r.min_y);
			}

			if( i == cursor )
			{
				rectangle rc;

				/* check if cursor turned off */
				if( (state->crt.cursor_top & 0x60) == 0x20 )
					continue;

				if( (state->crt.cursor_top & 0x60) == 0x60 )
				{
					state->crt.cursor_visible = 1;
				}
				else
				{
					state->crt.cursor_phase++;
					state->crt.cursor_visible = (state->crt.cursor_phase >> 3) & 1;
				}

				if( !state->crt.cursor_visible )
					continue;

				rc.min_x = r.min_x;
				rc.max_x = r.max_x;
				rc.min_y = r.min_y + (state->crt.cursor_top & 15);
				rc.max_y = r.min_y + (state->crt.cursor_bottom & 15);

				drawgfx_opaque(machine.generic.tmpbitmap, &rc, machine.gfx[0], 0x7f, state->colorram[i&0x3ff] + 16,
					0, 0, rc.min_x, rc.min_y);
				drawgfx_opaque(state->dlybitmap, &rc, machine.gfx[0], 0x7f, state->colorram[i&0x3ff] + 32,
					0, 0, rc.min_x, rc.min_y);
			}
		}
	}

	copybitmap(bitmap, machine.generic.tmpbitmap, 0, 0, 0, 0, cliprect);
	copybitmap_trans(bitmap, state->dlybitmap, 0, 0, 1, 0, cliprect, 0);
}

/***************************************************************************
  Draw the game screen in the given bitmap_t.
***************************************************************************/
SCREEN_UPDATE( cgenie )
{
	cgenie_state *state = screen->machine().driver_data<cgenie_state>();
    if( state->tv_mode )
		cgenie_refresh_tv_set(screen->machine(), bitmap, cliprect);
	else
		cgenie_refresh_monitor(screen->machine(), bitmap, cliprect);
	return 0;
}
