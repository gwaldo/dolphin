/*====================================================================

   filename:     gdsp_memory.cpp
   project:      GCemu
   created:      2004-6-18
   mail:		  duddie@walla.com

   Copyright (c) 2005 Duddie & Tratax

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   ====================================================================*/

#include <stdio.h>

#include "Globals.h"
#include "gdsp_interpreter.h"
#include "gdsp_memory.h"
#include "gdsp_interface.h"

u16 dsp_swap16(u16 x)
{
	return (x >> 8) | (x << 8);
}

u16 dsp_imem_read(u16 addr)
{
	if (g_dsp.pc & 0x8000)
		return dsp_swap16(g_dsp.irom[addr & DSP_IROM_MASK]);
	else
		return dsp_swap16(g_dsp.iram[addr & DSP_IRAM_MASK]);
}

u16 dsp_dmem_read(u16 addr)
{
	switch (addr >> 12)
	{
	    case 0x0: // 0xxx DRAM
		    return dsp_swap16(g_dsp.dram[addr & DSP_DRAM_MASK]);

	    case 0x8: // 8xxx DROM
		    ERROR_LOG(DSPHLE, "someone reads from ROM");
		    return dsp_swap16(g_dsp.drom[addr & DSP_DROM_MASK]);

		case 0x1: // 1xxx COEF
		    return dsp_swap16(g_dsp.coef[addr & DSP_COEF_MASK]);

	    case 0xf: // Fxxx HW regs
		    return gdsp_ifx_read(addr);

	    default: // error
			ERROR_LOG(DSPHLE, "%04x DSP ERROR: Read from UNKNOWN (%04x) memory", g_dsp.pc, addr);
		    return 0;
	}
}


void dsp_dmem_write(u16 addr, u16 val)
{
	switch (addr >> 12)
	{
	    case 0x8: // 8xxx DROM
		    ERROR_LOG(DSPHLE, "someone writes to ROM");
		    /* val = dsp_swap16(val);
		       g_dsp.drom[addr & DSP_DROM_MASK] = val;*/
		    break;

	    case 0xf: // Fxxx HW regs
		    gdsp_ifx_write(addr, val);
		    break;

	    case 0x0: // 0xxx DRAM
		    g_dsp.dram[addr & DSP_DRAM_MASK] = dsp_swap16(val);
		    break;

	    default: // error
		    ERROR_LOG(DSPHLE, "%04x DSP ERROR: Write to UNKNOWN (%04x) memory", g_dsp.pc, addr);
		    break;
	}
}

u16 dsp_fetch_code()
{
	u16 opc = dsp_imem_read(g_dsp.pc);
	g_dsp.pc++;
	return opc;
}

u16 dsp_peek_code()
{
	return dsp_imem_read(g_dsp.pc);
}
