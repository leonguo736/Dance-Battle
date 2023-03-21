
module nios2_system (
	clk_clk,
	coords_ram_addr,
	coords_ram_data,
	esp_uart_rxd,
	esp_uart_txd,
	reset_reset_n);	

	input		clk_clk;
	output	[4:0]	coords_ram_addr;
	input	[31:0]	coords_ram_data;
	input		esp_uart_rxd;
	output		esp_uart_txd;
	input		reset_reset_n;
endmodule
