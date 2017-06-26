/* toolbox/GraphPrinter.hpp - Graph Printer

   Copyright (C) 2013
   CACAOVM - Verein zur Foerderung der freien virtuellen Maschine CACAO

   This file is part of CACAO.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#ifndef _JSON_GRAPH_PRINTER_HPP
#define _JSON_GRAPH_PRINTER_HPP

#include "toolbox/OStream.hpp"

#include <string>
#include <set>
#include <map>
#include <utility>

#include <cstdio>

#include "vm/jit/compiler2/JITData.hpp"
#include "vm/jit/compiler2/MethodC2.hpp"
#include "vm/jit/compiler2/Instruction.hpp"
#include "vm/jit/compiler2/Instructions.hpp"
#include "vm/jit/compiler2/PassManager.hpp"
#include "vm/jit/compiler2/PassUsage.hpp"
#include "vm/jit/compiler2/Type.hpp"

// #include "vm/rt-timing.hpp"

namespace cacao {
namespace jit {
namespace compiler2 {

class JsonGraphPrinter {
private:
	FILE* file;
    int passNr;
    int instrNr;
    int edgeNr;

public:
	void initialize(JITData &JD);
	void printPass(JITData &JD, const char* passName);
	void close();

protected:
	void printInstruction(OStream &OS, Instruction *i, bool isRoot);
    void printEdges(OStream &OS, Instruction *i);
};

} // end namespace compiler2
} // end namespace jit
} // end namespace cacao

#endif  // _JSON_GRAPH_PRINTER_HPP

/*
 * These are local overrides for various environment variables in Emacs.
 * Please do not remove this and leave it at the end of the file, where
 * Emacs will automagically detect them.
 * ---------------------------------------------------------------------
 * Local variables:
 * mode: c++
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim:noexpandtab:sw=4:ts=4:
 */
