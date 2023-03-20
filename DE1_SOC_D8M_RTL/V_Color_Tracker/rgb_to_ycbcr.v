module rgb_to_ycbcr (
    input [7:0] iR,
    input [7:0] iG,
    input [7:0] iB,
    output [7:0] oY,
    output [7:0] oCb,
    output [7:0] oCr
);

    assign oY = 16 + (((iR<<6)+(iR<<1)+(iG<<7)+iG+(iB<<4)+(iB<<3)+iB)>>8);
    assign oCb = 128 + ((-((iR<<5)+(iR<<2)+(iR<<1))-((iG<<6)+(iG<<3)+(iG<<1))+(iB<<7)-(iB<<4))>>8);
    assign oCr = 128 + (((iR<<7)-(iR<<4)-((iG<<6)+(iG<<5)-(iG<<1))-((iB<<4)+(iB<<1)))>>8);

endmodule