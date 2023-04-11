
module nios2_system (
	clk_clk,
	coords_ram_read_addr,
	coords_ram_read_data,
	coords_ram_write_addr,
	coords_ram_write_en,
	coords_ram_write_data,
	esp_uart_rxd,
	esp_uart_txd,
	reset_reset_n);	

	input		clk_clk;
	output	[4:0]	coords_ram_read_addr;
	input	[31:0]	coords_ram_read_data;
	output	[4:0]	coords_ram_write_addr;
	output		coords_ram_write_en;
	output	[31:0]	coords_ram_write_data;
	input		esp_uart_rxd;
	output		esp_uart_txd;
	input		reset_reset_n;
endmodule
