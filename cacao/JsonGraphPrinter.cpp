/* src/vm/jit/compiler2/ArrayBoundsCheckRemoverPassss.cpp - JsonGraphPrinter

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

#include "vm/jit/compiler2/JsonGraphPrinter.hpp"

namespace cacao {
namespace jit {
namespace compiler2 {

 void JsonGraphPrinter::initialize(JITData &JD) {
    Method *M = JD.get_Method();

    // todo create name
    file = fopen("xyz.json","w");
    passNr = 0;
    instrNr = 0;
    edgeNr = 0;

    OStream	OS(file);
    OS << "{ \n\t\"class\": \"" << M->get_class_name_utf8() << "\", ";
    OS << "\n\t\"method\": \"" << M->get_name_utf8() << "\",";
    OS << "\n\t\"desc\": \""<< M->get_desc_utf8() << "\",";
    OS << "\n\t\"passes\": [\n";
 }

 void JsonGraphPrinter::close() {
    OStream	OS(file);
    OS << "\n\t]\n}";
    fclose(file);
 }

 void JsonGraphPrinter::printPass(JITData &JD, const char* passName) {

    OStream	OS(file);
    Method *M = JD.get_Method();
    if (passNr++ > 0) {
       OS << ", \n";
    }
    OS<<"\t\t{\n\t\t\"name\": \"" << passName<< "\", \n";
    // nodes
    instrNr = 0;
    OS<<"\t\t\"nodes\": [";
    for (Method::const_iterator i = M->begin(), e = M->end(); i != e; i++) {
       Instruction *I = *i;
       bool isRoot = M->get_init_bb() == I;
       printInstruction(OS, I, isRoot);
    }
    OS<<"],\n";

    // edges
    edgeNr = 0;
    OS<<"\t\t\"edges\": [";
    for (Method::const_iterator i = M->begin(), e = M->end(); i != e; i++) {
       printEdges(OS, *i);
    }
    OS<<"]\n\t\t}";
 }

 void JsonGraphPrinter::printInstruction(OStream &OS, Instruction *I, bool isRoot) {

    if (instrNr++ > 0) {
       OS << ", ";
    }
    OS<<"\n\t\t\t{\"id\": " << I->get_id() << ", ";
    OS<<"\"name\": \"" << I->get_name() << "\", ";
    OS<<"\"type\": \"" << get_type_name(I->get_type()) << "\"";
    if (isRoot) {
       OS<<", \"root\": " << isRoot << "";
    }

    if (I->get_BeginInst()) {
       OS<<", \"BB\": "  << I->get_BeginInst()->get_id();
    }

    if (I->has_side_effects()) {
       OS << ", \"sideEffects\": " << I->has_side_effects();
    }

    if (I->to_IFInst()) {
       Conditional::CondID C = I->to_IFInst()->get_condition();
       OS << ", \"condition\": \"" << C << "\"";
    }

    if (I->to_CONSTInst()) {
       CONSTInst* inst = I->to_CONSTInst();
       OS << ", \"value\": ";
       switch(inst->get_type()) {
          case Type::LongTypeID:   OS << inst->get_Long(); break;
          case Type::IntTypeID:    OS << inst->get_Int(); break;
          case Type::FloatTypeID:  OS << inst->get_Float(); break;
          case Type::DoubleTypeID: OS << inst->get_Double(); break;
          default: assert(0);
       }
    }

    if (I->op_size() > 0) {
       int operandNr = 0;
       OS << ", \"operands\": [";
       for (Instruction::OperandListTy::const_iterator ii = I->op_begin(), ee = I->op_end(); ii != ee; ++ii) {
          Instruction *II = (*ii)->to_Instruction();
          if (II) {
             if (operandNr++ > 0) {
                OS << ", ";
             }
             OS << II->get_id();
          }
       }
       OS << "]";
    }
    OS<<"}";
 }

 void JsonGraphPrinter::printEdges(OStream &OS, Instruction *I) {

    // operands
    if (I->op_size() > 0) {
       for (Instruction::OperandListTy::const_iterator ii = I->op_begin(), ee = I->op_end(); ii != ee; ++ii) {
          Instruction *II = (*ii)->to_Instruction();
          if (II) {
             if (edgeNr++ > 0) {
                OS << ", ";
             }
             OS << "\n\t\t\t{\"from\": " << II->get_id() << ", \"to\": " << I->get_id() << ", \"type\": \"op\"}";
          }
       }
    }

    // control flow
    BeginInst *BI = I->to_BeginInst();
    if (BI) {
       EndInst *EI = BI->get_EndInst();
       if (edgeNr++ > 0) {
          OS << ", ";
       }
       OS << "\n\t\t\t{\"from\": " << BI->get_id() << ", \"to\": " << EI->get_id() << ", \"type\": \"bb\"}";
    }

    EndInst *EI = I->to_EndInst();
    if (EI) {
       for (EndInst::SuccessorListTy::const_iterator ii = EI->succ_begin(), ee = EI->succ_end(); ii != ee; ++ii) {
          Instruction *II = ii->get()->to_Instruction();
          if (II) {
             if (edgeNr++ > 0) {
                OS << ", ";
             }
             OS << "\n\t\t\t{\"from\": " << EI->get_id() << ", \"to\": " << II->get_id() << ", \"type\": \"cfg\"";

             if (EI->to_IFInst()) {
                IFInst *ifInst = EI->to_IFInst();
                bool isTrueBranch = ifInst->get_then_target() == II;
                OS << ", \"trueBranch\": " << isTrueBranch;
             }
             OS << "}";
          }
       }
    }

    // scheduling dependencies
    if (I->dep_size() > 0) {
       for (Instruction::DepListTy::const_iterator ii = I->dep_begin(), ee = I->dep_end(); ii != ee; ++ii) {
          Instruction *II = (*ii);
          if (II) {
             if (edgeNr++ > 0) {
                OS << ", ";
             }
             OS << "\n\t\t\t{\"from\": " << II->get_id() << ", \"to\": " << I->get_id() <<
             ", \"type\": \"sched\"}";
          }
       }
    }
 }

} // end namespace compiler2
} // end namespace jit
} // end namespace cacao


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