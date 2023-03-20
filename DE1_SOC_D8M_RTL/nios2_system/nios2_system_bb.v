
module nios2_system (
	clk_clk,
	reset_reset_n,
	esp_uart_rxd,
	esp_uart_txd,
	coords_ram_addr);	

	input		clk_clk;
	input		reset_reset_n;
	input		esp_uart_rxd;
	output		esp_uart_txd;
	output	[4:0]	coords_ram_addr;
endmodule
