// coords_slave.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module coords_slave (
        input  wire [7:0]  avs_s0_address,     //     avs_s0.address
        input  wire        avs_s0_read,        //           .read
        output wire [31:0] avs_s0_readdata,    //           .readdata
        input  wire        avs_s0_write,       //           .write
        input  wire [31:0] avs_s0_writedata,   //           .writedata
        output wire        avs_s0_waitrequest, //           .waitrequest
        input  wire        clock_clk,          //      clock.clk
        input  wire        reset_reset,        //      reset.reset
        output wire [4:0]  read_addr,          // coords_ram.read_addr
        input  wire [31:0] read_data,          //           .read_data
        output wire [4:0]  write_addr,         //           .write_addr
        output wire        write_en,           //           .write_en
        output wire [31:0] write_data          //           .write_data
    );

    assign avs_s0_waitrequest = 1'b0;

    // Read Coords
    assign read_addr = avs_s0_address; 
    assign avs_s0_readdata = read_data;

    // Write Thresholds
    assign write_addr = avs_s0_address;
    assign write_en = avs_s0_write;
    assign write_data = avs_s0_writedata;

endmodule
