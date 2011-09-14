#ifndef WD1772_H
#define WD1772_H

#include "emu.h"
#include "imagedev/floppy.h"

#define MCFG_WD1772x_ADD(_tag, _clock)	\
	MCFG_DEVICE_ADD(_tag, WD1772x, _clock)

class wd1772_t : public device_t {
public:
	typedef delegate<void (bool state)> line_cb;

	wd1772_t(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	void dden_w(bool dden);
	void set_floppy(floppy_image_device *floppy);
	void setup_intrq_cb(line_cb cb);
	void setup_drq_cb(line_cb cb);

	void cmd_w(UINT8 val);
	UINT8 status_r();

	void track_w(UINT8 val);
	UINT8 track_r();

	void sector_w(UINT8 val);
	UINT8 sector_r();

	void data_w(UINT8 val);
	UINT8 data_r();

	void gen_w(int reg, UINT8 val);
	UINT8 gen_r(int reg);

	bool intrq_r();
	bool drq_r();

protected:
	virtual void device_start();
	virtual void device_reset();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	enum { TM_GEN, TM_CMD, TM_TRACK, TM_SECTOR };

	enum {
		// General "nothing doing" state
		IDLE,

		// Main states - the commands
		RESTORE,
		SEEK,
		STEP,
		READ_SECTOR,

		// Sub states - steps within the commands, which change only
		// on globally synchronizing event, such as timers or index
		// callback (which is timer-triggered)

		SPINUP,
		SPINUP_DONE,

		SEEK_MOVE,
		SEEK_WAIT_STEP_TIME,
		SEEK_WAIT_STEP_TIME_DONE,
		SEEK_WAIT_STABILIZATION_TIME,
		SEEK_WAIT_STABILIZATION_TIME_DONE,
		SEEK_DONE,

		SCAN_ID,
		SCAN_ID_FAILED,

		SECTOR_READ,

		COMMAND_DONE,

		// Live states - steps withing the sub-states, which change
		// continually depending on the disk contents until the next
		// externally discernable event is found.  Requires
		// checkpointing, running until an event is found, then
		// waiting for it.  Then the changes are either committed or
		// replayed until the sync event time.

		// The transition to LIVE_DONE is only done on a synced event.

		SEARCH_ADDRESS_MARK,
		READ_BLOCK_HEADER,
		READ_ID_BLOCK_TO_LOCAL,
		READ_SECTOR_DATA,
		READ_SECTOR_DATA_BYTE,
		READ_SECTOR_DATA_BYTE_DONE,
		LIVE_DONE,
	};

	struct pll_t {
		UINT16 counter;
		UINT16 increment;
		UINT16 transition_time;
		UINT8 history;
		UINT8 slot;
		UINT8 phase_add, phase_sub, freq_add, freq_sub;
		attotime ctime;

		attotime delays[38];

		void set_clock(attotime period);
		void reset(attotime when);
		int get_next_bit(attotime &tm, floppy_image_device *floppy, attotime limit);
	};

	struct live_info {
		attotime tm;
		int state, next_state;
		UINT16 shift_reg;
		UINT16 crc;
		int bit_counter;
		bool data_separator_phase;
		UINT8 data_reg;
		UINT8 idbuf[6];
		pll_t pll;
	};

	enum {
		S_BUSY = 0x01,
		S_DRQ  = 0x02,
		S_IP   = 0x02,
		S_TR00 = 0x04,
		S_LOST = 0x04,
		S_CRC  = 0x08,
		S_RNF  = 0x10,
		S_SPIN = 0x20,
		S_DDM  = 0x20,
		S_WP   = 0x40,
		S_MON  = 0x80
	};

	const static int step_times[4];

	floppy_image_device *floppy;

	emu_timer *t_gen, *t_cmd, *t_track, *t_sector;

	bool dden, status_type_1, intrq, drq;
	int main_state, sub_state;
	UINT8 command, track, sector, data, status;
	int last_dir;

	int counter, motor_timeout, sector_size;

	int cmd_buffer, track_buffer, sector_buffer;

	live_info cur_live, checkpoint_live;
	line_cb intrq_cb, drq_cb;

	static astring tts(attotime t);
	astring ttsn();

	void delay_cycles(emu_timer *tm, int cycles);

	void do_cmd_w();
	void do_track_w();
	void do_sector_w();
	void command_end();

	void seek_start(int state);
	void seek_continue();

	void read_sector_start();
	void read_sector_continue();

	void interrupt_start();

	void general_continue();
	void do_generic();

	void spinup();
	void index_callback(floppy_image_device *floppy, int state);

	void start_io(int live_state);
	void end_io();
	void checkpoint();
	void rollback();
	void live_delay(int state);
	void live_run(attotime limit = attotime::never);
	bool live_running();
	bool read_one_bit(attotime limit);
	void drop_drq();
	void set_drq();
};

extern const device_type WD1772x;

#endif
