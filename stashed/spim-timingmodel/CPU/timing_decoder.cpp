#include "syscall.h"
#include "timing_decoder.h"
#include "timing_core.h"
#include "log.h"

void TimingDecoder::Issue(TimingEvent *event) {
    if (event->inst == 0x0) {
        printf("[warning] null instruction detected. Fetching & Decoding terminated.\n");
        event->state = TES_Committed;
        return;
    }
    short opcode = OPCODE(event->inst);
    unsigned char rs = RS(event->inst);
    unsigned char rd = RD(event->inst);
    unsigned char rt = RT(event->inst);
    short imm = IMM(event->inst);
    mem_addr target = TARGET(event->inst);

    // Here are local control signals. You should specify in your code what their values mean, as I did.
    // ALUSrc: -1 -> invalid, 0/1 -> following lecture slides, 2 -> alu_src_b = PC+8, for JAL
    int ALUSrc = -1;
    // RegDst: -1 -> invalid, 0/1 -> following lecture slides, 2-> wb_id = $ra, for JAL
    int RegDst = -1;
    // Branch follows lecture slides
    bool Branch = false;
    // Jump follows lecture slides
    bool Jump = false;
    // Shift: true -> sll/srl, one input of ALU is shamt
    bool Shift = false;
    // Jr: true -> jump register
    bool Jr = false;
    // ZeroExt: true -> zero extension on imm
    bool ZeroExt = false;
    // Comp: 0 -> nothing, 1 -> beq, 2 -> bne, 3 -> bgez
    int Comp = 0;
    // Mem: true -> a memory instruction, only for the report
    bool Mem = false;
    // Reg: true -> a register instruction, only for the report
    bool Reg = false;

    ncycle_t temp_current_cycle = event->current_cycle;
    available_cycle = MAX(event->current_cycle, available_cycle) + c_decode_latency;
    event->execute_cycles += c_decode_latency;
    event->current_cycle = available_cycle;

    switch (opcode) {
        case Y_ADD_OP:  // !!! overflow exception !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_ADD;
            Reg = true;
            break;
        case Y_ADDI_OP:  // !!! overflow exception !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_ADD;
            Reg = true;
            break;
        case Y_ADDIU_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_ADDU;
            Reg = true;
            break;
        case Y_ADDU_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_ADDU;
            Reg = true;
            break;
        case Y_AND_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_AND;
            Reg = true;
            break;
        case Y_ANDI_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_AND;
            ZeroExt = true;
            Reg = true;
            break;
        case Y_BEQ_OP:
            //event->ALUOp = ALUOP_EQ;
            Branch = true;
            ALUSrc = 0x0;
            //FetchNextPC = false;
            Comp = 1;
            break;
        case Y_BNE_OP:
            //event->ALUOp = ALUOP_NEQ;
            Branch = true;
            ALUSrc = 0x0;
            //FetchNextPC = false;
            Comp = 2;
            break;
        case Y_J_OP:
            Jump = true;
            break;
        case Y_JAL_OP:
            rs = 0;  // trick: we force rs to 0. This leads ALU to calculate (0 + (PC+4))
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x2;  // Look! We just extended and defined our own control signal different from professor's material. This is the fascinating point of processor design: everyone has their own design.
            RegDst = 0x2;
            event->ALUOp = ALUOP_ADD;
            Jump = true;
            break;
        case Y_JR_OP:
            Jr = true;
            Jump = true;
            break;
        case Y_LBU_OP:
            RegDst = 0x0;
            event->RegWrite = 0x3; // 8bit
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemRead = 0x3; // 8bit
            event->MemToReg = 0x1;
            Mem = true;
            break;
        case Y_LHU_OP:
            RegDst = 0x0;
            event->RegWrite = 0x2; // 16bit
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemRead = 0x2; // 16bit
            event->MemToReg = 0x1;
            Mem = true;
            break;
        case Y_LL_OP:
            RegDst = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemRead = 0x1; // 32bit
            event->MemToReg = 0x1;
            Mem = true;
            break;
        case Y_LUI_OP:
            RegDst = 0x0;
            event->MemToReg = 0x0;
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_LUI;
            event->RegWrite = 0x1; // 32bit
            Mem = true;
            break;
        case Y_LW_OP:
            event->MemRead = 0x1; // 32bit
            event->MemToReg = 0x1;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_ADD;
            Mem = true;
            break;
        case Y_NOR_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_NOR;
            Reg = true;
            break;
        case Y_OR_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_OR;
            Reg = true;
            break;
        case Y_ORI_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_OR;
            ZeroExt = true;
            Reg = true;
            break;
        case Y_SLT_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_LESS;
            Reg = true;
            break;
        case Y_SLTI_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_LESS;
            Reg = true;
            break;
        case Y_SLTIU_OP: // !!! unsigned !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_LESSU;
            Reg = true;
            break;
        case Y_SLTU_OP: // !!! unsigned !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_LESSU;
            Reg = true;
            break;
        case Y_SLL_OP:
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SLL;
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            Shift = true;
            Reg = true;
            break;
        case Y_SRL_OP:
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SRL;
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            Shift = true;
            Reg = true;
            break;
        case Y_SB_OP:
            event->RegWrite = 0x0;
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemWrite = 0x3; // 8bit
            Mem = true;
            break;
        case Y_SC_OP: // !!! do not use !!!
            break;
        case Y_SH_OP:
            event->RegWrite = 0x0;
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemWrite = 0x2; // 16bit
            Mem = true;
            break;
        case Y_SW_OP:
            event->RegWrite = 0x0;
            ALUSrc = 0x1;
            event->ALUOp = ALUOP_ADD;
            event->MemWrite = 0x1; // 32bit
            Mem = true;
            break;
        case Y_SUB_OP:  // !!! overflow exception !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SUB;
            Reg = true;
            break;
        /* no subi or subiu
        case Y_SUBI_OP:  // !!! overflow exception !!!
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_SUB;
            Reg = true;
            break;
        case Y_SUBIU_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_SUBU;
            Reg = true;
            break;
        */
        case Y_SUBU_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SUBU;
            Reg = true;
            break;
        case Y_SYSCALL_OP:
            event->inst_is_syscall = true;
            break;
        case Y_BGEZ_OP:
            event->ALUOp = ALUOP_GEZ;
            Branch = true;
            ALUSrc = 0x1;
            //FetchNextPC = false;
            Comp = 3;
            break;
        case Y_SLLV_OP:
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SLL;
            Reg = true;
            break;
        case Y_SRA_OP:
            ALUSrc = 0x0;
            RegDst = 0x1;
            event->ALUOp = ALUOP_SRA;
            event->MemToReg = 0x0;
            event->RegWrite = 0x1; // 32bit
            Shift = true;
            Reg = true;
            break;
    }

    event->Branch = Branch || Jump || Jr;
    event->Mem = Mem;
    event->Reg = Reg || event->inst_is_syscall;

    // printf("\tdecoding addr:%p inst:%s\n", event->pc_addr, inst_to_string(event->pc_addr));
    if (Shift) {
        event->alu_src_1 = (imm >> 8) & 0x1f;
        //fprintf(stderr, "shamt = %08x\n", imm);
    }
    else {
        if (core->regfile->reg_used[rs]) {
            //fprintf(stderr, "find stall, rs reg %d!\n", rs);
            event->stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        core->regfile->Load(rs, event->alu_src_1);
    }

    // assert_msg_ifnot((ALUSrc != -1), "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (ALUSrc == 0x0) {
        if (core->regfile->reg_used[rt]) {
            //fprintf(stderr, "find stall, rt reg %d!\n", rt);
            event->stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        core->regfile->Load(rt, event->alu_src_2);
    } else if (ALUSrc == 0x1) {
        if (ZeroExt)
            event->alu_src_2 = imm & 0xffff;
        else
            event->alu_src_2 = (short)imm;
    }
    else if (ALUSrc == 0x2)
        event->alu_src_2 = event->pc_addr + BYTES_PER_WORD;  // This is a jal with **no delayed branches**.

    // assert_msg_ifnot((RegDst != -1), "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (RegDst == 0x0)
        event->reg_wb_id = rt;
    else if (RegDst == 0x1)
        event->reg_wb_id = rd;
    else if (RegDst == 0x2)
        event->reg_wb_id = 31;  // $ra
    
    if (event->MemWrite) {
        if (core->regfile->reg_used[rt]) {
            //fprintf(stderr, "find stall, rt reg %d!\n", rt);
            event->stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        core->regfile->Load(rt, event->mem_write_data);
    }

    bool IsSyscall = false;

    if (event->inst_is_syscall) {
        reg_word val_rs = 0x0;
        if (core->regfile->reg_used[REG_RES]) {
            //fprintf(stderr, "find stall, REG_RES reg %d!\n", REG_RES);
            event->stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        core->regfile->Load(REG_RES, val_rs);
        if (val_rs == READ_INT_SYSCALL)
            ++core->regfile->reg_used[REG_RES];
        if (val_rs == EXIT_SYSCALL) {
            // We reset this variable to prevent PC fetching after last instruction.
            IsSyscall = true;
        }
    }

    // This fetches the next pc
    
    if (event->RegWrite) {
        /* no need to handle WAW
        if (core->regfile->reg_used[event->reg_wb_id]) {
            //fprintf(stderr, "find stall, write_back reg %d!\n", event->reg_wb_id);
            if (Branch) branch_stall += event -> current_cycle - temp_current_cycle;
            if (Mem) mem_stall += event -> current_cycle - temp_current_cycle;
            if (Reg) reg_stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        */
        //fprintf(stderr, "reg %d is locked!\n", event->reg_wb_id);
        ++core->regfile->reg_used[event->reg_wb_id];
    }

    switch (Comp) {
        case 1:
            Branch &= (event->alu_src_1 == event->alu_src_2);
            break;
        case 2:
            Branch &= (event->alu_src_1 != event->alu_src_2);
            break;
        case 3:
            Branch &= (event->alu_src_1 >= 0);
            break;
    } 

    if (Jr) {
        reg_word t;
        if (core->regfile->reg_used[rs]) {
            //fprintf(stderr, "find stall, rs reg %d!\n", rs);
            event->stall += event -> current_cycle - temp_current_cycle;
            return;
        }
        core->regfile->Load(rs, t);
        core->next_pc_gen->GenerateNextPC(event->pc_addr, imm, t, Branch, Jump, temp_current_cycle, true);
    } else if (!IsSyscall) // not syscall
        //if (FetchNextPC)
        core->next_pc_gen->GenerateNextPC(event->pc_addr, imm, target, Branch, Jump, temp_current_cycle, false);
        /*
        else {
            //fprintf(stderr, "delayed branch due to beq/bne/bgez\n");
            //if (!event->dont_gen)
            //    core->next_pc_gen->GenerateNextPC(event->pc_addr, imm, target, false, false, temp_current_cycle, false, true);
            event->delay_branch = true;
            //event->stall += event -> current_cycle - temp_current_cycle;
            event->extended_imm = imm;
        }
        */

    assert(event->state == TES_WaitDecoder);
    event->state = TES_WaitExecutor;
}

TimingDecoder::TimingDecoder(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}