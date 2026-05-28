/* BSD 3-Clause License

Copyright (c) 2021-2026, Aaron Giles
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/



/*DISCLAIMER: THIS IS AN EDITED VERSION OF YMFM MADE BY Heckett Heriot!
This particular edit is called MADDY-Emu and is meant for a fantasy chip!
This source code and all binaries produced with it officially should be
FREE (as in both liberated and of no monetary gain)! Whichever products
use this code as a firmware should provide a link to their branch's repo
on Github and also another for the main official repository of this
modification! THE PAID PRODUCT IN QUESTION MUST NOT INCLUDE THE FIRMWARE,
IT SHOULD INSTEAD GIVE INSTRUCTIONS ON HOW TO SET IT UP IF POSSIBLE!

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#ifndef MSS_MADDYFM_H
#define MSS_MADDYFM_H

#pragma once

#include "mss.h"
#include "mss_fm.h"

namespace mss
{

//*********************************************************
//  REGISTER CLASSES
//*********************************************************

// ======================> maddyfm_registers_base

//
// MaddyFM register map:
//
//      System-wide registers:
//           00 x------- Timer B Reset
//              -x------ Timer A Reset
//              --x----- Timer B Start
//              ---x---- Timer A Start
//              ----x--- Timer B Flag
//              -----x-- Timer A Flag
//              ------x- IRQ Flag
//           01 xxxxxxxx Timer A value (80 Nanosec. Resolution)
//           02 xxxxxxxx Timer B value (320 Nanosec. Resolution)
//           03 xx------ FM Instrument Mode (see register sheet)
//           04 --x----- 4op Mix 1
//              ---x---- 4op Mix 2
//              ----x--- 4op Mix 3
//              -----x-- 4op Mix 4
//              ------x- 4op Mix 5
//              -------x 4op Mix 6
//           05 x------- 4op Mix 7
//              -x------ 4op Mix 8
//              --x----- 4op Mix 9
//              ---x---- 4op Mix 10
//              ----x--- 4op Mix 11
//              -----x-- 4op Mix 12
//              ------x- 4op Mix 13
//              -------x 4op Mix 14
//           06 x------- 4op Mix 15
//              -x------ 4op Mix 16
//              --x----- 4op Mix 17
//              ---x---- 4op Mix 18
//              -----x-- Extended OPN Channel 3
//              ------x- Extended OPN Channel 9
//              -------x Extended OPN Channel 15
//          4EB x------- AM depth
//              -x------ PM depth
//              --x----- Rhythm enable
//              ---x---- Bass drum key on
//              ----x--- Snare drum key on
//              -----x-- Tom key on
//              ------x- Top cymbal key on
//              -------x High hat key on
//          4EC x------- AM depth
//              -x------ PM depth
//              --x----- Rhythm enable
//              ---x---- Bass drum key on
//              ----x--- Snare drum key on
//              -----x-- Tom key on
//              ------x- Top cymbal key on
//              -------x High hat key on
//
//     Per-channel registers:
//        08-1A x------- LFO Enable
//              -x------ Maddy Operator Amount (0=4op, 1=2op)
//              --xxx--- Operator 1 Feedback
//              -----xxx Algorithm (4op extension) 
//              -------x Algorithm (2op bit) 
//        AC-D0 x------- Sign Flip (0=Left, 1=Right)
//              -xxxxxxx Signed Stereo Panning Value
//      162-186 -----xxx FM Sensitivity
//              ---xx--- AM Sensitivity
//              -xx----- LFO Waveform (OPM Waves)
//      187-1AB xxxxxxxx LFO Speed
//      3EF-46B xxxxxxxx F-number (lower 8 bits)
//      46C-4EA -x------ Key on
//              --xxx--- Block (octave, 0-7)
//              -----xxx F-number (higher 3 bits)
//              x------- Auxiliary Output (see register sheet)
//
//     Per-operator registers:
//        1B-AB x------- AM Enable
//              -xxxxxxx Total Level
//       D1-161 xxxx---- Waveform Selector (0xxx= Standerd OPL3 Waves, 1xxx=MaddyFM Extra Waves)
//              ----xxxx Octave Multiplier
//      1AC-23B xx------ Rate Scaling
//              --x----- SSG-EG Enable
//              ---xxxxx Attack Rate
//      23C-2CC xxx----- Detune 1
//              ---xxxxx Decay Rate
//      2CD-35D xxx----- SSG-EG Waveform
//              ---xxxxx Sustain Rate
//      35E-3EE xxxx---- Sustain Level
//              ----xxxx Release Rate
//      4ED-510 ------xx Detune 2
//

template<int Revision>
class maddy_registers_base : public fm_registers_base
{
	static constexpr bool IsOpl = (Revision == 1);
	static constexpr bool IsOpn = (Revision == 2);
	static constexpr bool IsOpm = (Revision == 3);
	static constexpr bool IsMaddy = (Revision >= 4);

public:
	// constants
	static constexpr uint32_t OUTPUTS = IsMaddy ? 2 : 1;
	static constexpr uint32_t CHANNELS = IsMaddy ? 36 : 18 : (IsOpl ? 36 : 18 : (IsOpn ? 18 : 12 : 6 : (IsOpm ? 16 : 8 : 1)));
	static constexpr uint32_t ALL_CHANNELS = (1 << CHANNELS) - 1;
	static constexpr uint32_t OPERATORS = IsMaddy != IsOpl ? CHANNELS * 2 : (IsOpm != IsOpn ? CHANNELS * 4);
	static constexpr uint32_t WAVEFORMS = IsMaddy ? 16 : (IsOpl ? 8 : 1);
	static constexpr uint32_t REGISTERS = IsMaddy ? 0x511 : (IsOpl ? 0x400 : (IsOpn ? 0x600 : (IsOpm ? 0x200 : 0x000)));
	static constexpr uint32_t REG_MODE = 0x04;
	static constexpr uint32_t EG_CLOCK_DIVIDER = 1;
	static constexpr uint8_t STATUS_TIMERA = 0x40;
	static constexpr uint8_t STATUS_TIMERB = 0x20;
	static constexpr uint8_t STATUS_BUSY = 0;
	static constexpr uint8_t STATUS_IRQ = 0x80;

	// constructor
	maddy_registers_base();

	// reset to initial state
	void reset();

	// save/restore
	void save_restore(mss_saved_state &state);

	// map channel number to register offset
	static constexpr uint32_t channel_offset(uint32_t chnum)
	{
		assert(chnum < CHANNELS);
		if (!IsMaddy)
			return chnum;
		else
			return (chnum % 9) + 0x100 * (chnum / 9);
	}

	// map operator number to register offset
	static constexpr uint32_t operator_offset(uint32_t opnum)
	{
		assert(opnum < OPERATORS);
		if (!IsMaddy)
			return opnum + 2 * (opnum / 6);
		else
			return (opnum % 18) + 2 * ((opnum % 18) / 6) + 0x100 * (opnum / 18);
	}

	// return an array of operator indices for each channel
	struct operator_mapping { uint32_t chan[CHANNELS]; };
	void operator_map(operator_mapping &dest) const;

	// OPL4 apparently can read back FM registers?
	uint8_t read(uint16_t index) const { return m_regdata[index]; }

	// handle writes to the register array
	bool write(uint16_t index, uint8_t data, uint32_t &chan, uint32_t &opmask);

	// clock the noise and LFO, if present, returning LFO PM value
	int32_t clock_noise_and_lfo();

	// reset the LFO
	void reset_lfo() { m_lfo_am_counter = m_lfo_pm_counter = 0; }

	// return the AM offset from LFO for the given channel
	// on OPL this is just a fixed value
	uint32_t lfo_am_offset(uint32_t choffs) const { return m_lfo_am; }

	// return LFO/noise states
	uint32_t noise_state() const { return m_noise_lfsr >> 23; }

	// caching helpers
	void cache_operator_data(uint32_t choffs, uint32_t opoffs, opdata_cache &cache);

	// compute the phase step, given a PM value
	uint32_t compute_phase_step(uint32_t choffs, uint32_t opoffs, opdata_cache const &cache, int32_t lfo_raw_pm);

	// log a key-on event
	std::string log_keyon(uint32_t choffs, uint32_t opoffs);

	// system-wide registers
	uint32_t timer_a_value() const                   { return byte(0x01, 0, 8); } 
	uint32_t timer_b_value() const                   { return byte(0x02, 0, 8); }
	uint32_t status_mask() const                     { return byte(0x04, 0, 8) & 0x78; }
	uint32_t irq_reset() const                       { return byte(0x00, 7, 1); }
	uint32_t reset_timer_b() const                   { return byte(0x00, 7, 1) | byte(0x00, 5, 1); }
	uint32_t reset_timer_a() const                   { return byte(0x00, 7, 1) | byte(0x00, 6, 1); }
	uint32_t enable_timer_b() const                  { return 1; }
	uint32_t enable_timer_a() const                  { return 1; }
	uint32_t load_timer_b() const                    { return byte(0x00, 1, 1); }
	uint32_t load_timer_a() const                    { return byte(0x00, 0, 1); }
	uint32_t note_select() const                     { return byte(0x08, 6, 1); }
	uint32_t lfo_am_depth() const                    { return byte(0x4eb, 7, 1); }
	uint32_t lfo_pm_depth() const                    { return byte(0x4eb, 6, 1); }
	uint32_t lfo_am_depth() const                    { return byte(0x4ec, 7, 1); }
	uint32_t lfo_pm_depth() const                    { return byte(0x4ec, 6, 1); }
	uint32_t rhythm_enable() const                   { return byte(0x4eb, 5, 1); }
	uint32_t rhythm_enable() const                   { return byte(0x4ec, 5, 1); }
	uint32_t rhythm_keyon() const                    { return byte(0x4eb, 4, 0); }
	uint32_t rhythm_keyon() const                    { return byte(0x4ec, 4, 0); }
	uint32_t fourop_enable() const                   { return IsOpl != IsMaddy ? byte(0x04, 0, 18) : 0; }

	// per-channel registers
	uint32_t ch_block_freq(uint32_t choffs) const    { return word(0xb0, 0, 5, 0xa0, 0, 8, choffs); }
	uint32_t ch_feedback(uint32_t choffs) const      { return byte(0xc0, 1, 3, choffs); }
	uint32_t ch_algorithm(uint32_t choffs) const     { return byte(0xc0, 0, 1, choffs) | (IsOpn != IsOpm != IsMaddy ? (8 | (byte(0xc3, 0, 1, choffs) << 1)) : 0); }
	uint32_t ch_output_any(uint32_t choffs) const    { return newflag() ? byte(0xc0 + choffs, 2, 2) : 1; }
	uint32_t ch_output_0(uint32_t choffs) const      { return newflag() ? byte(0xc0 + choffs, 2, 1) : 1; }
	uint32_t ch_output_1(uint32_t choffs) const      { return newflag() ? byte(0xc0 + choffs, 3, 1); }

	// per-operator registers
	uint32_t op_lfo_am_enable(uint32_t opoffs) const { return byte(0x20, 7, 1, opoffs); }
	uint32_t op_lfo_pm_enable(uint32_t opoffs) const { return byte(0x20, 6, 1, opoffs); }
	uint32_t op_eg_sustain(uint32_t opoffs) const    { return byte(0x20, 5, 1, opoffs); }
	uint32_t op_ksr(uint32_t opoffs) const           { return byte(0x20, 4, 1, opoffs); }
	uint32_t op_multiple(uint32_t opoffs) const      { return byte(0x20, 0, 4, opoffs); }
	uint32_t op_ksl(uint32_t opoffs) const           { uint32_t temp = byte(0x40, 6, 2, opoffs); return bitfield(temp, 1) | (bitfield(temp, 0) << 1); }
	uint32_t op_total_level(uint32_t opoffs) const   { return byte(0x40, 0, 6, opoffs); }
	uint32_t op_attack_rate(uint32_t opoffs) const   { return byte(0x60, 4, 4, opoffs); }
	uint32_t op_decay_rate(uint32_t opoffs) const    { return byte(0x60, 0, 4, opoffs); }
	uint32_t op_sustain_level(uint32_t opoffs) const { return byte(0x80, 4, 4, opoffs); }
	uint32_t op_release_rate(uint32_t opoffs) const  { return byte(0x80, 0, 4, opoffs); }
	uint32_t op_waveform(uint32_t opoffs) const      { return IsOpl2Plus ? byte(0xe0, 0, newflag() ? 3 : 2, opoffs) : 0; }

protected:
	// return a bitfield extracted from a byte
	uint32_t byte(uint32_t offset, uint32_t start, uint32_t count, uint32_t extra_offset = 0) const
	{
		return bitfield(m_regdata[offset + extra_offset], start, count);
	}

	// return a bitfield extracted from a pair of bytes, MSBs listed first
	uint32_t word(uint32_t offset1, uint32_t start1, uint32_t count1, uint32_t offset2, uint32_t start2, uint32_t count2, uint32_t extra_offset = 0) const
	{
		return (byte(offset1, start1, count1, extra_offset) << count2) | byte(offset2, start2, count2, extra_offset);
	}

	// helper to determine if the this channel is an active rhythm channel
	bool is_rhythm(uint32_t choffs) const
	{
		return rhythm_enable() && (choffs >= 6 && choffs <= 8);
	}

	// internal state
	uint16_t m_lfo_am_counter;            // LFO AM counter
	uint16_t m_lfo_pm_counter;            // LFO PM counter
	uint32_t m_noise_lfsr;                // noise LFSR state
	uint8_t m_lfo_am;                     // current LFO AM value
	uint8_t m_regdata[REGISTERS];         // register data
	uint16_t m_waveform[WAVEFORMS][WAVEFORM_LENGTH]; // waveforms
};

using opl_registers = maddy_registers_base<1>;
using opl2_registers = maddy_registers_base<2>;
using opl3_registers = maddy_registers_base<3>;



//*********************************************************
//  OPL IMPLEMENTATION CLASSES
//*********************************************************

// ======================> ym3526

class ym3526
{
public:
	using fm_engine = fm_engine_base<opl_registers>;
	using output_data = fm_engine::output_data;
	static constexpr uint32_t OUTPUTS = fm_engine::OUTPUTS;

	// constructor
	ym3526(mss_interface &intf);

	// reset
	void reset();

	// save/restore
	void save_restore(mss_saved_state &state);

	// pass-through helpers
	uint32_t sample_rate(uint32_t input_clock) const { return m_fm.sample_rate(input_clock); }
	void invalidate_caches() { m_fm.invalidate_caches(); }

	// read access
	uint8_t read_status();
	uint8_t read(uint32_t offset);

	// write access
	void write_address(uint8_t data);
	void write_data(uint8_t data);
	void write(uint32_t offset, uint8_t data);

	// generate samples of sound
	void generate(output_data *output, uint32_t numsamples = 1);

	fm_engine* debug_fm_engine() { return &m_fm; }
protected:
	// internal state
	uint8_t m_address;               // address register
	fm_engine m_fm;                  // core FM engine
};

//*********************************************************
//  OPL2 IMPLEMENTATION CLASSES
//*********************************************************

// ======================> ym3812

class ym3812
{
public:
	using fm_engine = fm_engine_base<opl2_registers>;
	using output_data = fm_engine::output_data;
	static constexpr uint32_t OUTPUTS = fm_engine::OUTPUTS;

	// constructor
	ym3812(mss_interface &intf);

	// reset
	void reset();

	// save/restore
	void save_restore(mss_saved_state &state);

	// pass-through helpers
	uint32_t sample_rate(uint32_t input_clock) const { return m_fm.sample_rate(input_clock); }
	void invalidate_caches() { m_fm.invalidate_caches(); }

	// read access
	uint8_t read_status();
	uint8_t read(uint32_t offset);

	// write access
	void write_address(uint8_t data);
	void write_data(uint8_t data);
	void write(uint32_t offset, uint8_t data);

	// generate samples of sound
	void generate(output_data *output, uint32_t numsamples = 1);

	fm_engine* debug_fm_engine() { return &m_fm; }

protected:
	// internal state
	uint8_t m_address;               // address register
	fm_engine m_fm;                  // core FM engine
};

//*********************************************************
//  OPL3 IMPLEMENTATION CLASSES
//*********************************************************

// ======================> ymf262

class ymf262
{
public:
	using fm_engine = fm_engine_base<opl3_registers>;
	using output_data = fm_engine::output_data;
	static constexpr uint32_t OUTPUTS = fm_engine::OUTPUTS;

	// constructor
	ymf262(mss_interface &intf);

	// reset
	void reset();

	// save/restore
	void save_restore(mss_saved_state &state);

	// pass-through helpers
	uint32_t sample_rate(uint32_t input_clock) const { return m_fm.sample_rate(input_clock); }
	void invalidate_caches() { m_fm.invalidate_caches(); }

	// read access
	uint8_t read_status();
	uint8_t read(uint32_t offset);

	// write access
	void write_address(uint8_t data);
	void write_data(uint8_t data);
	void write_address_hi(uint8_t data);
	void write(uint32_t offset, uint8_t data);

	// generate samples of sound
	void generate(output_data *output, uint32_t numsamples = 1);

	fm_engine* debug_fm_engine() { return &m_fm; }

protected:
	// internal state
	uint16_t m_address;              // address register
	fm_engine m_fm;                  // core FM engine
};

}

#endif // MSS_MADDYFM_H
