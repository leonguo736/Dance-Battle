// This module buffers four lines of camera iPixel12bRgb data AFTER deciding if the iPixel12bRgb is red or not.
//    So the 4 line buffers hold 1 bit at each address. Each buffer holds 1 line of camera data.
// The output of this module is a column (three pixels) of the already-filled data, above
//    the currently filling line.
module red_line_buffer ( 
   input iVgaClk,
   input [35:0] iPixel12bRgb,
   input iVgaHRequest,
   input [9:0] iHIndex,
   input [7:0] iCbLow, 
   input [7:0] iCrLow,
   input [7:0] iCbHigh,
   input [7:0] iCrHigh,
   // The taps are the column of 3 pixels above the current camera iPixel12bRgb location.
   output reg oTapTop,
   output reg oTapMiddle,
   output reg oTapBottom
);

localparam NUM_BUFFERS = 4;  // number of line buffers.

reg [1:0] WR;
reg [NUM_BUFFERS-1:0] EN;   // one-hot-coded write-enable (wren) signal for the line buffers.

wire [11:0] RED;
wire [11:0] GREEN;
wire [11:0] BLUE;
reg seems_red;

wire T0, T1, T2, T3;

wire [7:0] RED_OTHER; 
wire [7:0] GREEN_OTHER;
wire [7:0] BLUE_OTHER;

assign RED = iPixel12bRgb[35:24];
assign GREEN = iPixel12bRgb[23:12];
assign BLUE = iPixel12bRgb[11:0];

assign RED_OTHER = RED[11:4]; 
assign GREEN_OTHER = GREEN[11:4];
assign BLUE_OTHER = BLUE[11:4];

wire [7:0] y; 
wire [7:0] cb;
wire [7:0] cr;

rgb_to_ycbcr u1 ( .iR( RED ), .iG( GREEN ), .iB( BLUE ), .oY( y ), .oCb( cb ), .oCr( cr ) );

// Detect red. Put a '1' in the line buffer if this iPixel12bRgb seems very red.
always @ (*) begin
   if( ({1'b0,RED_OTHER[6:1]} > BLUE_OTHER[7:0]) && ({1'b0,RED_OTHER[6:1]} > GREEN_OTHER[7:0]) ) seems_red = 1'b1; else seems_red = 1'b0;
   // if (cb >= iCbLow && cb <= iCbHigh && cr >= iCrLow && cr <= iCrHigh) seems_red = 1'b1; else seems_red = 1'b0;
end

// WR is a pointer to the line buffer that is currently being filled.
always @ (posedge iVgaHRequest) begin
   if( ~iVgaHRequest ) begin
      WR <= 0;
      end
   else begin
      if( WR >= (NUM_BUFFERS-1) ) WR <= 0;  // counts from 0 up to the number of buffers - 1.
      else WR <= WR + 1;
   end
end

integer i;
always @(*)
for( i=0; i<NUM_BUFFERS; i=i+1) begin
   if( WR == i ) EN[i] = 1; else EN[i] = 0;
end

// Organize the outputs to allow for easy low-pass filtering
always @ (*)
   case( WR )    // synthesis full_case parallel_case
      0 : begin
             oTapTop = T1;
             oTapMiddle = T2;
             oTapBottom = T3;
          end
      1 : begin
             oTapTop = T2;
             oTapMiddle = T3;
             oTapBottom = T0;
          end
      2 : begin
             oTapTop = T3;
             oTapMiddle = T0;
             oTapBottom = T1;
          end
      3 : begin
             oTapTop = T0;
             oTapMiddle = T1;
             oTapBottom = T2;
          end
      endcase

// Line buffers.
red_line_x line0 ( 
   .clock( iVgaClk ),
	.data( seems_red ),
	.rdaddress( iHIndex ),
	.wraddress( iHIndex ),
	.wren( EN[0] & iVgaHRequest ),
	.q( T0 )
);

red_line_x line1 ( 
   .clock( iVgaClk ),
	.data( seems_red ),
	.rdaddress( iHIndex ),
	.wraddress( iHIndex ),
	.wren( EN[1] & iVgaHRequest ),
	.q( T1 )
);

red_line_x line2 ( 
   .clock( iVgaClk ),
	.data( seems_red ),
	.rdaddress( iHIndex ),
	.wraddress( iHIndex ),
	.wren( EN[2] & iVgaHRequest ),
	.q( T2 )
);

red_line_x line3 ( 
   .clock( iVgaClk ),
	.data( seems_red ),
	.rdaddress( iHIndex ),
	.wraddress( iHIndex ),
	.wren( EN[3] & iVgaHRequest ),
	.q( T3 )
);
 
endmodule

