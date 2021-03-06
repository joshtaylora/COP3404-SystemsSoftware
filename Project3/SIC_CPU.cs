using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using System.Reflection.Emit;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace SIC_Simulator
{

    [Serializable()]
    class SIC_CPU

    {
        public readonly static int NumDevices = 65;
        public int PC = 0;
        public int A = 0;
        public int X = 0;
        public int L = 0;
        public int SW = 0;

        public byte[] MemoryBytes;

        public SIC_Device[] Devices;


        public bool MachineStateIsNotSaved = false;

        private StringBuilder MicroSteps;

        public String MicrocodeSteps
        {
            get { return this.MicroSteps.ToString(); }
        }


        /// <summary>
        /// Constructs a SIC VM (CPU and Memory)
        /// </summary>
        /// <param name="ZeroizeBytes"></param>
        public SIC_CPU(bool ZeroizeBytes)
        {
            PC = A = X = L = SW = 0;
            MemoryBytes = new byte[32768];

            if (ZeroizeBytes == true)
            {
                this.ZeroizeMemory();

            }
            else
            {
                this.RandomizeMemory();
            }

            this.Devices = new SIC_Device[NumDevices];

            for (int i = 0; i < NumDevices; i++)
            {
                this.Devices[i] = new SIC_Device(i);
            }
            this.MicroSteps = new StringBuilder();
            MachineStateIsNotSaved = true;
        }



        /// <summary>
        /// Randomize all memory bytes in the SIC
        /// Simulates filling memory with "JUNK"
        /// Way to see if programs specifically set memory when needed
        /// and do not rely on "JUNK" values
        /// </summary>
        public void RandomizeMemory()
        {
            Random rnd = new Random(Guid.NewGuid().GetHashCode());
            rnd.NextBytes(this.MemoryBytes);
            MachineStateIsNotSaved = true;
        }

        /// <summary>
        /// Sets all memory bits to ZERO in the SIC
        /// </summary>
        public void ZeroizeMemory()
        {
            byte zero;
            zero = 0;
            for (int x = 0; x < 32768; x++) { this.MemoryBytes[x] = zero; }
            MachineStateIsNotSaved = true;
        }

        /// <summary>
        /// Performs a "hard" reset of the virtual machine.
        /// 1. All registers and SW are set to zero
        /// 2. All Memory is zeroed.
        /// </summary>
        public void ResetVM()
        {
            this.PC = 0;
            this.A = 0;
            this.X = 0;
            this.L = 0;
            this.SW = 0;
            this.ZeroizeMemory();
            this.MicroSteps = new StringBuilder();
        }



        /// <summary>
        /// Fetches a 24-bit word from Memory and places it into an integer
        /// </summary>
        /// <param name="Address">Address of Memory Location in SIC [0..32767]</param>
        /// <returns></returns>
        public int FetchWord(int Address)
        {
            int num1 = 0;
            int num2 = 0;
            for (int i = 0; i < 3; ++i)
            {
                char ch = (char)this.MemoryBytes[Address++];
                num2 = (int)ch;
                num2 = num2 & 0x000000FF;
                num1 = num1 | num2;
                num1 = num1 << 8;
            }
            num1 = num1 >> 8;

            return num1;

        }

        /// <summary>
        /// Fetches Byte in Memory and returns character type
        /// </summary>
        /// <param name="Address"></param>
        /// <returns></returns>
        public char FetchByte(int Address)
        {
            return (char)this.MemoryBytes[Address];
        }


        public void StoreWord(int Address, int data)
        {
            byte b;
            for (int i = 2; i >= 0; --i)
            {
                b = (byte)(data & 0xFF);
                MemoryBytes[Address + i] = b;
                data = data >> 8;
            }
            MachineStateIsNotSaved = true;
        }


        public void StoreByte(int address, byte data)
        {
            this.MemoryBytes[address] = data;
            MachineStateIsNotSaved = true;
        }


        /// <summary>
        /// Load's an Object File Representation from a File
        /// into this SIC VM's Memory
        /// NO RELOCATION IS PERFORMACE (ABSOLUTE LOADING)
        /// </summary>
        /// <param name="S"></param>
        public void LoadObjectFile(String AbsoluteFilePath)
        {


        }


        private void ReadHeaderRecord(System.IO.Stream s)
        {

        }

        private void ReadTextRecord(System.IO.Stream s)
        {

        }


        public void InitializePC(int PCValue)
        {
            this.PC = PCValue;
        }


        /// <summary>
        /// This method steps the CPU one time. 
        /// FETCH->DECODE->EXECUTE
        /// </summary>
        public void PerformStep()
        {
            // 1. Fetch the next instruction by pulling the WORD from memory
            //      pointed to by PC (Address of Next Instruction)
            //
            // 2. Decode the instruction
            //
            // 3. Execute Instruction
            //      --> Execute will advance PC by 3 bytes(1 word) or change it to
            //      -->   a target address if we have a JUMP, or RSUB

            int NextInstruction;
            int op = 0;
            int TA = 0;

            NextInstruction = this.FetchWord(PC);

            this.DecodeInstruction(NextInstruction, ref op, ref TA);

            this.ExecuteInstruction(op, TA);
            MachineStateIsNotSaved = true;
        }





        /// <summary>
        /// Returns a human-readable  description of the instruction and operand value located
        /// at Address. The string has two parts, delimited by a |
        /// mnemonic target address | complete description of instruction and result
        /// </summary>
        /// <param name="Address">Absolute address of 3-byte instruction</param>
        /// <returns>String with description</returns>
        public String GetInstructionDescription(int Address)
        {
            string Result = string.Empty;
            string Details = string.Empty;
            string Effect = string.Empty;
            int Word;
            Word = this.FetchWord(Address); // Fetch the Word at Address
            int TargetAddress;
            int OpCode;
            bool INDEXED = false;
            int XBit = 0;
            XBit = (Word & 0x8000);
            INDEXED = (XBit > 0);

            TargetAddress = Word & 0x7FFF;
            OpCode = Word & 0xFF0000;
            OpCode = OpCode >> 16;


            switch (OpCode)
            {
                case 0x18: //   ADD
                    Result = "ADD";
                    Details = "Add Value in Target Address to Register A";
                    Effect = "A <- (A) + (TA)";
                    break;

                case 0x40: //   AND
                    Result = "AND";
                    Details = "Perform Bitwise AND on Value in Target Address and Register A, store result in A";
                    Effect = "A <- (A) && (TA)";
                    break;

                case 0x28:  // CMP   (Compare and set Status Word SW)
                    Result = "CMP";
                    break;

                case 0x24: // DIV 
                    Result = "DIV";
                    Details = "Divide Register A by Value in Target Address ";
                    Effect = "A <- (A) / (TA)";
                    break;

                case 0x3C: //   J 
                    Result = "J";
                    Details = "Perform Unconditional Jump to Target Address";
                    Effect = "PC <- (TA)";
                    break;

                case 0x30: //   JEQ 
                    Result = "JEQ";
                    Details = "Perform Conditional Jump to Target Address when CC = 00";
                    Effect = "PC <- (TA) if CC = 00";
                    break;

                case 0x34: //   JGT 
                    Result = "JGT";
                    Details = "Perform Conditional Jump to Target Address when CC = 10";
                    Effect = "PC <- (TA) if CC = 10";
                    break;

                case 0x38: //   JLT 
                    Result = "JLT";
                    Details = "Perform Conditional Jump to Target Address when CC = 01";
                    Effect = "PC <- (TA) if CC = 01";
                    break;

                case 0x48: // JSUB      (Jump to subroutine starting at TA. Preserve PC by storing in L)
                    Result = "JSUB";
                    Details = "Jump to Subroutine at Target Address. Preserve PC By Storing in L";
                    Effect = "L <- PC; PC <- (TA)";
                    break;

                case 0x00: // LDA 
                    Result = "LDA";
                    Details = "Load Value in Target Address to Register A";
                    Effect = "A <- (TA)";
                    break;

                case 0x50: //  LDCH
                    Result = "LDCH";
                    Details = "Load Character from Device Specified in Target Address to Rightmost Byte in A";
                    Effect = "A[rightmost byte] <- Device(TA)";
                    break;

                case 0x08: //  LDL 
                    Result = "LDL";
                    Details = "Load Value in Target Address to Register L";
                    Effect = "L <- (TA)";
                    break;

                case 0x04: //  LDX 
                    Result = "LDX";
                    Details = "Load Value in Target Address to Register X";
                    Effect = "X <- (TA)";
                    break;

                case 0x20:  // MUL 
                    Result = "MUL";
                    Details = "Multiple Value in Target Address by Register A Store in A";
                    Effect = "A <- (A) * (TA)";
                    break;

                case 0x44: //   OR 
                    Result = "OR";
                    Details = "Perform Bitwise OR on Value in Target Address and Register A, store result in A";
                    Effect = "A <- (A) || (TA)";
                    break;

                case 0x4C: //    RSUB
                    Result = "RSUB";
                    Details = "Return from Subroutine. ";
                    Effect = "PC <- (L)";
                    break;

                case 0x0C: //   STA         (Stores contents of A in Target Address)
                    Result = "STA";
                    Details = "Store Value in Register A to Target Address";
                    Effect = "(TA) <- A";
                    break;

                case 0x54: //   STCH 
                    Result = "STCH";
                    break;

                case 0x14: //   STL 
                    Result = "STL";
                    Details = "Store Value in Register L to Target Address";
                    Effect = "(TA) <- L";
                    break;

                case 0x10: //   STX         (Stores contents of X in Target Address)
                    Result = "STX";
                    Details = "Store Value in Register X to Target Address";
                    Effect = "(TA) <- X";
                    break;

                case 0x1C: // SUB
                    Result = "SUB";
                    Details = "Sub Value in Target Address to Register A";
                    Effect = "A <- (A) - (TA)";
                    break;

                case 0xE0: //   TD          (Tests to see if a device is busy).
                    Result = "TD";
                    Details = "Test Device Number Specified in Target Address";
                    Effect = "Set SW";
                    break;

                case 0x2C: //   TIX 
                    Result = "TIX";
                    Details = "Increment value in X Register. Compare to value in Target Address";
                    Effect = "X <- X + 1; COMP X to M set CC";
                    break;

                case 0xDC: //   WD          (Write to Device)
                    Result = "WD";
                    Details = "Write rightmost byte in A to Device Number in Target Address";
                    Effect = " Device(TA) <- A[rightmost byte]";
                    break;

                default:
                    Result = "";
                    break;
            }
            Result += " ";
            if (INDEXED == true)
            {
                Result += "TA = TA + X ->" + TargetAddress.ToString(("X6")) + '+' + X.ToString(("X6")) + "->" + (TargetAddress + TargetAddress + X).ToString(("X6"));
                TargetAddress += this.X;   // Add contents of X register to address for indexed Mode
            }
            else
            {
                Result += "TA = " + TargetAddress.ToString("X6");
            }
            Result = Result + "|" + Details + "|" + Effect;
            return Result;
        }

        /// <summary>
        /// Executes Single Operation Code using Target Address as Operand.
        /// This Method is the "microcode" steps in the SIC CPU
        /// to execute mnemonics
        /// </summary>
        /// <param name="OpCode">Opcode for Instruction to Execute</param>
        /// <param name="TA">Calculated Target Address</param>
        public void ExecuteInstruction(int OpCode, int TA)
        {

            switch (OpCode)
            {
                case 0x18: //   ADD
                    this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") +  " + " + this.FetchWord(TA).ToString("X6"));
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.A += this.FetchWord(TA);
                    this.PC += 3;
                    break;

                case 0x40: //   AND
                    this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") + " && " + this.FetchWord(TA).ToString("X6"));
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.A &= this.FetchWord(TA);
                    this.PC += 3;
                    break;

                case 0x28:  // CMP   (Compare and set Status Word SW)
                    int Data;
                    Data = this.FetchWord(TA);

                    if (A < Data)
                    {
                        this.SW = this.SW | 0x40;
                        this.SW = this.SW & 0xFFFF7F;
                        this.MicroSteps.AppendLine("CC <- 01");
                    }
                    else if (A == Data)
                    {
                        this.SW = this.SW & 0xFFFF3F;
                        this.MicroSteps.AppendLine("CC <- 00");
                    }
                    else
                    {
                        this.SW = this.SW | 0x80;
                        this.SW = this.SW & 0xFFFFBF;
                        this.MicroSteps.AppendLine("CC <- 10");
                    }
                    // Condition Code Values
                    // CC = 00 -> Equal
                    // CC = 01 -> Less than
                    // CC = 10 -> Greater than
                    // CC = 11 -> Not used
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x24: // DIV 
                    // We don't want to crash the VM.
                    // IF we divide by zero, we should do something in the VM.

                    if (this.FetchWord(TA) == 0)
                    {
                        // NO exception. We should set status WORD and NOT DO THE DIV
                    }

                    else
                    {
                        this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") + " / " + this.FetchWord(TA).ToString("X6"));
                        this.A /= this.FetchWord(TA);
                    }
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x3C: //   J 
                    this.MicroSteps.AppendLine("PC <- " + TA.ToString("X6"));
                    this.PC = TA;
                    break;

                case 0x30: //   JEQ 
                    //MessageBox.Show(SW.ToString() + " & " + 0xC0 + " = " + (SW&0xC0));    # Done to compare values
                    if ((SW & 0xC0) == 0)
                    {
                        this.MicroSteps.AppendLine("PC <- " + TA.ToString("X6"));
                        PC = TA;
                    }
                    else {
                        this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                        this.PC += 3; }
                    break;

                case 0x34: //   JGT 
                    int TempJGT;
                    TempJGT = (SW & 0xC0) >> 6;
                    if (TempJGT == 2)
                    {
                        this.MicroSteps.AppendLine("PC <- " + TA.ToString("X6"));
                        this.PC = TA;
                    }
                    else {
                        this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                        this.PC += 3; }
                    break;

                case 0x38: //   JLT 
                    int TempJLT;
                    TempJLT = (SW & 0xC0) >> 6;
                    if (TempJLT == 1)
                    {
                        this.MicroSteps.AppendLine("PC <- " + TA.ToString("X6") );
                        this.PC = TA;
                    }
                    else {
                        this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                        this.PC += 3; }
                    break;

                case 0x48: // JSUB      (Jump to subroutine starting at TA. Preserve PC by storing in L)
                    this.MicroSteps.AppendLine("L  <- " + PC.ToString("X6") + " + 3");
                    this.L = ( this.PC + 3);
                    this.MicroSteps.AppendLine("PC <- " + TA.ToString("X6"));
                    this.PC = TA;
                    break;

                case 0x00: // LDA 
                    this.MicroSteps.AppendLine("A  <- " + this.FetchWord(TA).ToString("X6") );
                    this.A = this.FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x50: //  LDCH
                    byte ByteLoad;
                    ByteLoad = (byte)FetchByte(TA);

                    //TODO -> Wire in character reads from device objects
                    this.A = ByteLoad;

                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x08: //  LDL 
                    this.MicroSteps.AppendLine("L  <- " + this.FetchWord(TA).ToString("X6"));
                    this.L = this.FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x04: //  LDX 
                    this.MicroSteps.AppendLine("X  <- " + this.FetchWord(TA).ToString("X6"));
                    this.X = this.FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x20:  // MUL 
                    this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") + " * " + this.FetchWord(TA).ToString("X6"));
                    this.A *= FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x44: //   OR 
                    this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") + " OR " + this.FetchWord(TA).ToString("X6"));
                    this.A |= this.FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0xD8: // RD 
                    byte dataByte;
                    dataByte = (byte)this.A;
                    int DeviceNumberToRead;
                    DeviceNumberToRead = this.FetchWord(TA);

                    // Set Device's Status Word to BUSY
                    this.Devices[DeviceNumberToRead].DeviceSW &= 0xFFFF3F;

                    // Write the byte to the device
                    dataByte = this.Devices[DeviceNumberToRead].ReadByte();

                    // Set Device's Status Word to AVAILABLE
                    this.Devices[DeviceNumberToRead].DeviceSW |= 0x40;
                    this.Devices[DeviceNumberToRead].DeviceSW &= 0xFFFF7F;
                    int tmp;
                    tmp = (int)dataByte;
                    tmp &= 0xFF;
                    this.A = this.A & 0xFFFF00;

                    this.A |= tmp;
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;


                case 0x4C: //    RSUB
                    if (this.L == 0)
                    {
                        this.MicroSteps.AppendLine("PC <- (-1) PROGRAM HALTED" );
                        this.PC = -1;   // Program Halted.
                    }
                    else
                    {
                        this.MicroSteps.AppendLine("PC <- " + this.L.ToString("X6") );
                        this.PC = this.L;
                    }
                    break;

                case 0x0C: //   STA         (Stores contents of A in Target Address)
                    this.MicroSteps.AppendLine("TA <- " + A.ToString("X6") + " : TA = " + TA.ToString("X6"));
                    this.StoreWord(TA, A);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x54: //   STCH 
                    int tempChar;
                    char dataSTCHByte;
                    tempChar = this.A & 0xFF;
                    dataSTCHByte = (char)tempChar;
                    this.StoreByte(TA, (byte)dataSTCHByte);
                    this.MicroSteps.AppendLine("TA <- " + tempChar.ToString("X6") + " : TA = " + TA.ToString("X6"));

                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x14: //   STL 
                    this.MicroSteps.AppendLine("TA <- " + L.ToString("X6") + " : TA = " + TA.ToString("X6"));
                    this.StoreWord(TA, L);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x10: //   STX         (Stores contents of X in Target Address)
                    this.MicroSteps.AppendLine("TA <- " + X.ToString("X6") + " : TA = " + TA.ToString("X6"));
                    this.StoreWord(TA, X);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x1C: // SUB           (Subtract Value in TA from A )
                    this.MicroSteps.AppendLine("A  <- " + this.A.ToString("X6") + " - " + this.FetchWord(TA).ToString("X6"));
                    this.A -= this.FetchWord(TA);
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0xE0: //   TD          (Tests to see if a device is busy).
                    this.SW = this.SW | 0x40;
                    this.SW = this.SW & 0xFFFF7F; //CC is <
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0x2C: //   TIX 
                    int DataW;
                    int tempTIX;
                    DataW = this.FetchWord(TA);
                    this.MicroSteps.AppendLine("X <- " + X.ToString("X6") + " + 1");

                    tempTIX = ++this.X - DataW;
                    if (tempTIX < 0)
                    {
                        this.SW = this.SW | 0x40;
                        this.SW = this.SW & 0xFFFF7F;
                    }
                    else if (tempTIX == 0)
                    {
                        this.SW = this.SW & 0xFFFF3F;
                    }
                    else
                    {
                        this.SW = this.SW | 0x80;
                        this.SW = this.SW & 0xFFFFBF;
                    }
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

                case 0xDC: //   WD          (Write to Device)
                    /*** WD ***/

                    byte dataByteW;
                    dataByteW = (byte)this.A;
                    int DeviceNumberToWriteTo;
                    DeviceNumberToWriteTo = this.FetchWord(TA);

                    // Set Device's Status Word to BUSY
                    this.Devices[DeviceNumberToWriteTo].DeviceSW &= 0xFFFF3F;

                    // Write the byte to the device
                    this.Devices[DeviceNumberToWriteTo].WriteByte(dataByteW);

                    // Set Device's Status Word to AVAILABLE
                    this.Devices[DeviceNumberToWriteTo].DeviceSW |= 0x40;
                    this.Devices[DeviceNumberToWriteTo].DeviceSW &= 0xFFFF7F;
                    this.MicroSteps.AppendLine("Device" + DeviceNumberToWriteTo.ToString() + " <- " + dataByteW.ToString("X6") );
                    this.MicroSteps.AppendLine("PC <- " + this.PC.ToString("X6") + " + 3");
                    this.PC += 3;
                    break;

            }




        }


        public void LoadToMemory(String line, int StartAddress, int Length)
        {
            int i = 9, num = 0, BytesRead = 0, index = StartAddress;
            while (BytesRead < Length)
            {
                char ch = line[i++];
                if (ch >= 'A')
                {
                    ch -= (char)7;
                }
                ch -= (char)48;
                num = ch << 4;

                ch = line[i++];
                if (ch >= 'A')
                {
                    ch -= (char)7;
                }
                ch -= (char)48;
                num += ch;
                BytesRead++;
                StoreByte(index++, (byte)num);
            }

        }

        public void DecodeInstruction(int FullInstruction, ref int OpCode, ref int TargetAddress)
        {
            bool INDEXED = false;
            int XBit = 0;
            XBit = (FullInstruction & 0x8000);
            INDEXED = (XBit > 0);

            TargetAddress = FullInstruction & 0x7FFF;
            OpCode = FullInstruction & 0xFF0000;
            OpCode = OpCode >> 16;
            if (INDEXED == true)
            {
                TargetAddress += this.X;   // Add contents of X register to address for indexed Mode
            }
        }



    }


}
