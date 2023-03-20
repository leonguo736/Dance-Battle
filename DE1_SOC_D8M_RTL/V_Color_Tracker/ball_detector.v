// This module looks at the video, selects red pixels, low-pass filters that, and
//   then finds the center of the red object.
// The output is the placement of a marker (+ like symbol) overlaying the camera video.

module ball_detector (
   input reset, 
   input [35:0] iVideo12bRgb,     // Post-de-bayer 12-bit RGB video data from the camera.
   input [19:0] iPixelAddress,    // Raw address of the current pixel. 20 bits, 10 bits for each axis.
   input iVgaHRequest,
   input iVgaVRequest,
   input iVgaClk,
   input iFreezeRam,              // Stop filling the ball_ram RAM with new image data. Freezes the display.
   input iVgaRequest,             // high when VGA is in the active area (not Hsync,Vsync...)
   input iVideoSelect,            // Selects camera video, or ball_ram video image.
   input iFilterOn,               // Turns on evaluation for red pixels
   output reg [23:0] oVideo8bRgb  // 8-bit RGB output to VGA generator
);

wire [8:0] horz_line;  // row of red middle.
wire [9:0] vert_line;  // column of red middle.

wire isPixelRed;
wire [11:0] RED;
wire [11:0] GREEN;
wire [11:0] BLUE;

wire ram_seems_red;   // stored value of isPixelRed in the ball_ram.
wire [9:0] h_value; // current horizontal pixel location.
wire [8:0] v_value; // current vertical pixel location.

assign RED = iVideo12bRgb[35:24]; // 12-bit R
assign GREEN = iVideo12bRgb[23:12];
assign BLUE = iVideo12bRgb[11:0];

red_frame u1 ( 
   .VGA_clock( iVgaClk ),
   .reset( reset ),
   .pixel_data( {RED[11:4], GREEN[11:4], BLUE[11:4]} ), // convert to 8-bit RGB
   .iVgaHRequest( iVgaHRequest ),
   .iVgaVRequest( iVgaVRequest ),
   .x_cont( h_value ),
   .y_cont( v_value ),
   .red_pixel( isPixelRed ),  // This is the output of red_frame. Low-pass filtered red detection.
   .horz_line( horz_line ),
   .vert_line( vert_line ),
   .filter_on( iFilterOn ), 
   .EX_IO( EX_IO )
);

// Mux. Combinatorial logic to select the camera video, or, the red ball detector video frame.
always @(*) begin
   if( iVideoSelect == 1'b0 ) begin
         if( (h_value == vert_line) || (v_value == horz_line) )
            oVideo8bRgb = { 8'b0111_1111, 8'b0111_1111, 8'b0111_1111 };
         else
            oVideo8bRgb = { RED[7:0], GREEN[7:0], BLUE[7:0] };
      end
   else begin
      if( (h_value == vert_line) || (v_value == horz_line) )
         oVideo8bRgb = { 8'b0111_1111, 8'b0111_1111, 8'b0111_1111 };
      else
         oVideo8bRgb = { 1'b0, {7{ram_seems_red}}, {2{1'b0}}, {6{ram_seems_red}}, {2{1'b0}}, {6{ram_seems_red}}  };
      end
   end   

// Create a two-port RAM (ball_ram)). 1x640x480 bits.
// Input = Camera image, but red-detected and low-pass filtered by red_frame.v.
// Each pixel is 1 or 0, depending on red value.
// Output feeds the VGA waveform generator.
ball_ram this_ball_ram( 
	.clock( iVgaClk ),
	.data( isPixelRed ),
	.rdaddress( iPixelAddress ),
	.rden( iVgaHRequest || iVgaVRequest ),   // is high when VGA generator is in the active area
	.wraddress( iPixelAddress /*>= 1 ? VGA_ADDRESS-1 : VGA_ADDRESS*/ ),   // counters that track the output of the SDRAM
	.wren( iVgaRequest & ~iFreezeRam ),
	.q( ram_seems_red )
);
   
Mod_counter #(.N(10), .M(640)) h_count (
		.clk( iVgaClk ),
		.clk_en( iVgaHRequest  ),
		.reset( ~iVgaHRequest ),
		.max_tick( ),      // output when final count is reached
		.q( h_value )    // horizontal (column) pixel counter, 0 to 639
);

Mod_counter #(.N(9), .M(480)) v_count (
		.clk( iVgaHRequest ),
		.clk_en( iVgaVRequest ),
		.reset( ~iVgaVRequest ),
		.max_tick( ),      // output when final count is reached
		.q( v_value )    // vertical (row) pixel counter, 0 to 479
);

Mod_counter #(.N(19), .M(307200)) pixel_count (
		.clk( iVgaClk ),
		.clk_en( iVgaHRequest & iVgaVRequest ),
		.reset( ~iVgaVRequest ),
		.max_tick( ),      // output when final count is reached
		.q()    // counter output [N-1:0]
);

endmodule