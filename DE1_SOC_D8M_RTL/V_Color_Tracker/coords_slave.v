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
        output wire [31:0] write_data,         //           .write_data
        output wire [4:0]  write_addr          //           .write_addr
    );

    // TODO: Auto-generated HDL template

    // assign avs_s0_readdata = 32'b00000000000000000000000000000000;

    // assign avs_s0_waitrequest = 1'b0;

    // assign read_addr = 5'b00000;

    // assign write_data = 32'b00000000000000000000000000000000;

    // assign write_addr = 5'b00000;
    
    assign avs_s0_waitrequest = 1'b0;
    reg [4:0] stored_read_addr; 
    reg [4:0] stored_write_addr;
    reg write_request; 

    assign write_addr = stored_write_addr;
    assign read_addr = stored_read_addr;
    assign avs_s0_readdata = read_data;

    always @(posedge clock_clk) begin
        write_request <= 0;
        if (reset_reset) begin
            stored_read_addr <= 5'b00000;
            stored_write_addr <= 5'b00000;
        end else begin
            if (avs_s0_read) begin
                stored_read_addr <= avs_s0_address;
            end else if (avs_s0_write) begin
                stored_write_addr <= avs_s0_address;
                write_data <= avs_s0_writedata;
                write_request <= 1; 
            end
        end
    end

endmodule
