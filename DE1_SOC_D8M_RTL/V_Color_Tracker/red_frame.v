// This module low-pass filters the red iPixel12bRgb data.
// The input is a three-element column of data above the current camera iPixel12bRgb.
// This module fills a 1-bit frame buffer.
module red_frame ( 
   input iVgaClk,
   input reset,
   input [35:0] iPixel12bRgb,
   input [9:0] iHIndex,
   input [8:0] iVIndex,
   input iVgaHRequest,
   input iVgaVRequest,
   output reg oIsPixelRed,
   output reg [8:0] oRedPixelHIndex,   // passes up the value of the number of the horizontal line of the center of the red object.
   output reg [9:0] oRedPixelVIndex,
   input iFilterOn, 
   input [7:0] iCrLow, 
   input [7:0] iCrHigh, 
   input [7:0] iCbLow,
   input [7:0] iCbHigh
);

localparam START_UP = 0, WAIT = 1, IS_RED = 2;

wire oTapTop, oTapMiddle, oTapBottom;
reg d1_top, d2_top, d1_middle, d2_middle, d1_bottom, d2_bottom;
reg [3:0] sum;
reg [9:0] cntr;    // counts consecutive red pixels on the active video row (line)). Max value of 640.
reg [9:0] max_ever; // Holds the value of the largest number of consecutive pixels encountered in any line so far, this video frame.
reg [9:0] end_x;      // column that the red streak ended on.
reg [8:0] line_of_max;  // video line (0-479) that contained the maximum number of sequential red pixels.
reg [1:0] state;

// This is the value that gets passed up a level. It represents the iPixel12bRgb two rows above the current
//   camera iPixel12bRgb location. (The filtering creates 2 raster lines of latency.)
always @ (*) begin
   sum = d2_top  +  d1_top  +  oTapTop +
         d2_middle + d1_middle + oTapMiddle +
         d2_bottom + d1_bottom + oTapBottom;
   if( iFilterOn == 1'b1 ) begin
      if( sum >= 5 ) oIsPixelRed = 1'b1;
      else oIsPixelRed = 1'b0;
      end
   else 
      oIsPixelRed = oTapMiddle;
end

// Counts the number of consecutive horizontal red pixels.
// If greater than ever encountered so far, save the line number (iVIndex) into the line_of_max register.
// The line_of_max gets transferred into the horiz_line register at the end of each video frame.
always @ (posedge iVgaClk or negedge reset)
   if( ~reset ) begin
      cntr <= 0;     // counts consecutive red pixels.
      oRedPixelHIndex <= 120;
      state <= START_UP;
      end
   else
      case( state )
         START_UP : begin
                       cntr <= 0;
                       if( iVgaVRequest == 1'b0 ) state <= START_UP;   // Wait for start of new video frame.
                       else state <= WAIT;
                    end
 
         WAIT : begin        // waiting for iVgaHRequest to go high, to start evaluating the next line.
                   cntr <= 0;
                   if( iVgaVRequest == 1'b0 ) begin
                      state <= START_UP;   // If iVgaVRequest is low, then current video frame has ended.
                      oRedPixelHIndex <= line_of_max;   // output the line number that had the max sequential red pixels.
                      oRedPixelVIndex <= end_x - ( max_ever >> 1 );
                      end
                   else if( iVgaHRequest == 1'b0 ) state <= WAIT;
                   else state <= IS_RED;
                end
                   
         IS_RED : begin      // in active video area. start counting red pixels
                      if( iVgaHRequest == 1'b0 ) state <= WAIT;
                      else if( oIsPixelRed == 1'b1 ) begin
                              cntr <= cntr + 1;
                              state <= IS_RED;
                              end
                           else begin     // Did not see a red iPixel12bRgb, and still in active area.
                              cntr <= 0;
                              state <= IS_RED;
                              end
                  end
       endcase

always @ (posedge iVgaClk or negedge reset)  // If cntr (consequtive red pixels) sets a new record, save the record value, and the line it occured on.
   if( ~reset ) begin
      max_ever <= 0;  // new video frame, so reset the "max number of sequential red pixels in this video frame" register.
      line_of_max <= 240;  // default is middle of screen, if no red object is ever detected.
      end
   else begin
      if( (state == START_UP) ) begin
         max_ever <= 0;  // new video frame, so reset the "max number of sequential red pixels in this video frame" register.
         line_of_max <= 240;  // default is middle of screen, if no red object is ever detected.
         end
      else if( cntr > max_ever ) begin
         end_x <= iHIndex;     // save the column the red streak ends on (so far).
         max_ever <= cntr;
         line_of_max <= iVIndex;
         end
      end


// Create a 3x3 array from the 1x3 incoming column of iPixel12bRgb data.
always @ (posedge iVgaClk) begin
   d2_top <= d1_top;
   d1_top <= oTapTop;
   d2_middle <= d1_middle;
   d1_middle <= oTapMiddle;
   d2_bottom <= d1_bottom;
   d1_bottom <= oTapBottom;
end
   

// This is the module that buffers three lines of video data so that the 3x3 low-pass
//    algorithm can operate.
red_line_buffer u1 ( 
   .iVgaClk( iVgaClk ),
   .iPixel12bRgb( iPixel12bRgb ),
   .iVgaHRequest( iVgaHRequest ),
   .iHIndex( iHIndex ),
   // The tap outputs are the column of 3 pixels above the current camera iPixel12bRgb location.
   .oTapTop( oTapTop ),
   .oTapMiddle( oTapMiddle ),
   .oTapBottom( oTapBottom ), 
   .iCrLow( iCrLow ),
   .iCrHigh( iCrHigh ),
   .iCbLow( iCbLow ),
   .iCbHigh( iCbHigh )
);

endmodule